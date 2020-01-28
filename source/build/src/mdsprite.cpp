//------------------------------------- MD2/MD3 LIBRARY BEGINS -------------------------------------

#ifdef USE_OPENGL

#include "compat.h"
#include "build.h"
#include "pragmas.h"
#include "baselayer.h"
#include "engine_priv.h"
#include "hightile.h"
#include "polymost.h"
#include "mdsprite.h"

#include "common.h"
#include "palette.h"
#include "textures.h"
#include "bitmap.h"
#include "v_video.h"
#include "flatvertices.h"
#include "../../glbackend/glbackend.h"

static int32_t curextra=MAXTILES;

#define MIN_CACHETIME_PRINT 10


static int32_t addtileP(int32_t model,int32_t tile,int32_t pallet)
{
    // tile >= 0 && tile < MAXTILES

    UNREFERENCED_PARAMETER(model);
    if (curextra==MAXTILES+EXTRATILES-1)
    {
        initprintf("warning: max EXTRATILES reached\n");
        return curextra;
    }

    if (tile2model[tile].modelid==-1)
    {
        tile2model[tile].pal=pallet;
        return tile;
    }

    if (tile2model[tile].pal==pallet)
        return tile;

    while (tile2model[tile].nexttile!=-1)
    {
        tile=tile2model[tile].nexttile;
        if (tile2model[tile].pal==pallet)
            return tile;
    }

    tile2model[tile].nexttile=curextra;
    tile2model[curextra].pal=pallet;

    return curextra++;
}

int32_t Ptile2tile(int32_t tile,int32_t palette)
{
    int t = tile;
    while ((tile = tile2model[tile].nexttile) != -1)
        if (tile2model[tile].pal == palette)
        {
            t = tile;
            break;
        }
    return t;
}

#define MODELALLOCGROUP 256
static int32_t nummodelsalloced = 0;

static int32_t maxmodelverts = 0, allocmodelverts = 0;
static int32_t maxmodeltris = 0, allocmodeltris = 0;
vec3f_t *vertlist = NULL; //temp array to store interpolated vertices for drawing

#ifdef USE_GLEXT
static int32_t allocvbos = 0, curvbo = 0;
static GLuint *vertvbos = NULL;
static GLuint *indexvbos = NULL;
#endif

#ifdef POLYMER
static int32_t *tribuf = NULL;
static int32_t tribufverts = 0;
#endif

static mdmodel_t *mdload(const char *);
static void mdfree(mdmodel_t *);

void freeallmodels()
{
    int32_t i;

    if (models)
    {
        for (i=0; i<nextmodelid; i++) mdfree(models[i]);
        DO_FREE_AND_NULL(models);
        nummodelsalloced = 0;
        nextmodelid = 0;
    }

    Bmemset(tile2model,-1,sizeof(tile2model));
    for (i=0; i<MAXTILES; i++)
        Bmemset(tile2model[i].hudmem, 0, sizeof(tile2model[i].hudmem));

    curextra=MAXTILES;

    if (vertlist)
    {
        DO_FREE_AND_NULL(vertlist);
        allocmodelverts = maxmodelverts = 0;
        allocmodeltris = maxmodeltris = 0;
    }

#ifdef POLYMER
    DO_FREE_AND_NULL(tribuf);
#endif
}

void mdinit()
{
    freeallmodels();
    mdinited = 1;
}

int32_t md_loadmodel(const char *fn)
{
    mdmodel_t *vm, **ml;

    if (!mdinited) mdinit();

    if (nextmodelid >= nummodelsalloced)
    {
        ml = (mdmodel_t **)Xrealloc(models,(nummodelsalloced+MODELALLOCGROUP)*sizeof(void *));
        models = ml; nummodelsalloced += MODELALLOCGROUP;
    }

    vm = mdload(fn); if (!vm) return -1;
    models[nextmodelid++] = vm;
    return nextmodelid-1;
}

int32_t md_setmisc(int32_t modelid, float scale, int32_t shadeoff, float zadd, float yoffset, int32_t flags)
{
    mdmodel_t *m;

    if (!mdinited) mdinit();

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;
    m = models[modelid];
    m->bscale = scale;
    m->shadeoff = shadeoff;
    m->zadd = zadd;
    m->yoffset = yoffset;
    m->flags = flags;

    return 0;
}

static int32_t framename2index(mdmodel_t *vm, const char *nam)
{
    int32_t i = 0;

    switch (vm->mdnum)
    {
    case 2:
    {
        md2model_t *m = (md2model_t *)vm;
        md2frame_t *fr;
        for (i=0; i<m->numframes; i++)
        {
            fr = (md2frame_t *)&m->frames[i*m->framebytes];
            if (!Bstrcmp(fr->name, nam)) break;
        }
    }
    break;
    case 3:
    {
        md3model_t *m = (md3model_t *)vm;
        for (i=0; i<m->numframes; i++)
            if (!Bstrcmp(m->head.frames[i].nam,nam)) break;
    }
    break;
    }
    return i;
}

int32_t md_defineframe(int32_t modelid, const char *framename, int32_t tilenume, int32_t skinnum, float smoothduration, int32_t pal)
{
    md2model_t *m;
    int32_t i;

    if (!mdinited) mdinit();

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;
    if ((uint32_t)tilenume >= (uint32_t)MAXTILES) return -2;
    if (!framename) return -3;

    tilenume=addtileP(modelid,tilenume,pal);
    m = (md2model_t *)models[modelid];
    if (m->mdnum == 1)
    {
        tile2model[tilenume].modelid = modelid;
        tile2model[tilenume].framenum = tile2model[tilenume].skinnum = 0;
        return 0;
    }

    i = framename2index((mdmodel_t *)m,framename);
    if (i == m->numframes) return -3;   // frame name invalid

    tile2model[tilenume].modelid = modelid;
    tile2model[tilenume].framenum = i;
    tile2model[tilenume].skinnum = skinnum;
    tile2model[tilenume].smoothduration = Blrintf((float)UINT16_MAX * smoothduration);

    return i;
}

int32_t md_defineanimation(int32_t modelid, const char *framestart, const char *frameend, int32_t fpssc, int32_t flags)
{
    md2model_t *m;
    mdanim_t ma, *map;
    int32_t i;

    if (!mdinited) mdinit();

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;

    Bmemset(&ma, 0, sizeof(ma));
    m = (md2model_t *)models[modelid];
    if (m->mdnum < 2) return 0;

    //find index of start frame
    i = framename2index((mdmodel_t *)m,framestart);
    if (i == m->numframes) return -2;
    ma.startframe = i;

    //find index of finish frame which must trail start frame
    i = framename2index((mdmodel_t *)m,frameend);
    if (i == m->numframes) return -3;
    ma.endframe = i;

    ma.fpssc = fpssc;
    ma.flags = flags;

    map = (mdanim_t *)Xmalloc(sizeof(mdanim_t));

    Bmemcpy(map, &ma, sizeof(ma));

    map->next = m->animations;
    m->animations = map;

    return 0;
}

#if 0
// FIXME: CURRENTLY DISABLED: interpolation may access frames we consider 'unused'?
int32_t md_thinoutmodel(int32_t modelid, uint8_t *usedframebitmap)
{
    md3model_t *m;
    md3surf_t *s;
    mdanim_t *anm;
    int32_t i, surfi, sub, usedframes;
    static int16_t otonframe[1024];

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;
    m = (md3model_t *)models[modelid];
    if (m->mdnum != 3) return -2;

    for (anm=m->animations; anm; anm=anm->next)
    {
        if (anm->endframe <= anm->startframe)
        {
//            initprintf("backward anim %d-%d\n", anm->startframe, anm->endframe);
            return -3;
        }

        for (i=anm->startframe; i<anm->endframe; i++)
            usedframebitmap[i>>3] |= pow2char[i&7];
    }

    sub = 0;
    for (i=0; i<m->numframes; i++)
    {
        if (!(usedframebitmap[i>>3]&pow2char[i&7]))
        {
            sub++;
            otonframe[i] = -1;
            continue;
        }

        otonframe[i] = i-sub;
    }

    usedframes = m->numframes - sub;
    if (usedframes==0 || usedframes==m->numframes)
        return usedframes;

    //// THIN OUT! ////

    for (i=0; i<m->numframes; i++)
    {
        if (otonframe[i]>=0 && otonframe[i] != i)
        {
            if (m->muladdframes)
                Bmemcpy(&m->muladdframes[2*otonframe[i]], &m->muladdframes[2*i], 2*sizeof(vec3f_t));
            Bmemcpy(&m->head.frames[otonframe[i]], &m->head.frames[i], sizeof(md3frame_t));
        }
    }

    for (surfi=0; surfi < m->head.numsurfs; surfi++)
    {
        s = &m->head.surfs[surfi];

        for (i=0; i<m->numframes; i++)
            if (otonframe[i]>=0 && otonframe[i] != i)
                Bmemcpy(&s->xyzn[otonframe[i]*s->numverts], &s->xyzn[i*s->numverts], s->numverts*sizeof(md3xyzn_t));
    }

    ////// tweak frame indices in various places

    for (anm=m->animations; anm; anm=anm->next)
    {
        if (otonframe[anm->startframe]==-1 || otonframe[anm->endframe-1]==-1)
            initprintf("md %d WTF: anm %d %d\n", modelid, anm->startframe, anm->endframe);

        anm->startframe = otonframe[anm->startframe];
        anm->endframe = otonframe[anm->endframe-1];
    }

    for (i=0; i<MAXTILES+EXTRATILES; i++)
        if (tile2model[i].modelid == modelid)
        {
            if (otonframe[tile2model[i].framenum]==-1)
                initprintf("md %d WTF: tile %d, fr %d\n", modelid, i, tile2model[i].framenum);
            tile2model[i].framenum = otonframe[tile2model[i].framenum];
        }

    ////// realloc & change "numframes" everywhere

    if (m->muladdframes)
        m->muladdframes = Xrealloc(m->muladdframes, 2*sizeof(vec3f_t)*usedframes);
    m->head.frames = Xrealloc(m->head.frames, sizeof(md3frame_t)*usedframes);

    for (surfi=0; surfi < m->head.numsurfs; surfi++)
    {
        m->head.surfs[surfi].numframes = usedframes;
        // CAN'T do that because xyzn is offset from a larger block when loaded from md3:
//        m->head.surfs[surfi].xyzn = Xrealloc(m->head.surfs[surfi].xyzn, s->numverts*usedframes*sizeof(md3xyzn_t));
    }

    m->head.numframes = usedframes;
    m->numframes = usedframes;

    ////////////
    return usedframes;
}
#endif

int32_t md_defineskin(int32_t modelid, const char *skinfn, int32_t palnum, int32_t skinnum, int32_t surfnum, float param, float specpower, float specfactor, int32_t flags)
{
    mdskinmap_t *sk, *skl;
    md2model_t *m;

    if (!mdinited) mdinit();

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;
    if (!skinfn) return -2;
    if ((unsigned)palnum >= (unsigned)MAXPALOOKUPS) return -3;

    m = (md2model_t *)models[modelid];
    if (m->mdnum < 2) return 0;
    if (m->mdnum == 2) surfnum = 0;

    skl = NULL;
    for (sk = m->skinmap; sk; skl = sk, sk = sk->next)
        if (sk->palette == (uint8_t)palnum && skinnum == sk->skinnum && surfnum == sk->surfnum)
            break;
    if (!sk)
    {
        sk = (mdskinmap_t *)Xcalloc(1,sizeof(mdskinmap_t));

        if (!skl) m->skinmap = sk;
        else skl->next = sk;
    }

    sk->palette = (uint8_t)palnum;
    sk->flags = (uint8_t)flags;
    sk->skinnum = skinnum;
    sk->surfnum = surfnum;
    sk->param = param;
    sk->specpower = specpower;
    sk->specfactor = specfactor;
	sk->texture = TileFiles.GetTexture(skinfn);
	if (!sk->texture)
	{
		initprintf("Unable to load %s as model skin\n", skinfn);
	}

    return 0;
}

int32_t md_definehud(int32_t modelid, int32_t tilex, vec3f_t add, int32_t angadd, int32_t flags, int32_t fov)
{
    if (!mdinited) mdinit();

    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;
    if ((uint32_t)tilex >= (uint32_t)MAXTILES) return -2;

    tile2model[tilex].hudmem[(flags>>2)&1] = (hudtyp *)Xmalloc(sizeof(hudtyp));

    hudtyp * const hud = tile2model[tilex].hudmem[(flags>>2)&1];

    hud->add = add;
    hud->angadd = ((int16_t)angadd)|2048;
    hud->flags = (int16_t)flags;
    hud->fov = (int16_t)fov;

    return 0;
}

int32_t md_undefinetile(int32_t tile)
{
    if (!mdinited) return 0;
    if ((unsigned)tile >= (unsigned)MAXTILES) return -1;

    tile2model[tile].modelid = -1;
    tile2model[tile].nexttile = -1;
    DO_FREE_AND_NULL(tile2model[tile].hudmem[0]);
    DO_FREE_AND_NULL(tile2model[tile].hudmem[1]);
    return 0;
}

/* this function is problematic, it leaves NULL holes in model[]
 * (which runs from 0 to nextmodelid-1) */
int32_t md_undefinemodel(int32_t modelid)
{
    int32_t i;
    if (!mdinited) return 0;
    if ((uint32_t)modelid >= (uint32_t)nextmodelid) return -1;

    for (i=MAXTILES+EXTRATILES-1; i>=0; i--)
        if (tile2model[i].modelid == modelid)
        {
            tile2model[i].modelid = -1;
            DO_FREE_AND_NULL(tile2model[i].hudmem[0]);
            DO_FREE_AND_NULL(tile2model[i].hudmem[1]);
        }

    if (models)
    {
        mdfree(models[modelid]);
        models[modelid] = NULL;
    }

    return 0;
}


//Note: even though it says md2model, it works for both md2model&md3model
FTexture *mdloadskin(idmodel_t *m, int32_t number, int32_t pal, int32_t surf, bool *exact)
{
    int32_t i;
    mdskinmap_t *sk, *skzero = NULL;
    int32_t doalloc = 1;

    if (m->mdnum == 2)
        surf = 0;

    if ((unsigned)pal >= (unsigned)MAXPALOOKUPS)
        return 0;

    i = -1;
    for (sk = m->skinmap; sk; sk = sk->next)
    {
        if (sk->palette == pal && sk->skinnum == number && sk->surfnum == surf)
        {
			if (exact) *exact = true;
            //OSD_Printf("Using exact match skin (pal=%d,skinnum=%d,surfnum=%d) %s\n",pal,number,surf,skinfile);
            return sk->texture;
        }
        //If no match, give highest priority to number, then pal.. (Parkar's request, 02/27/2005)
        else if ((sk->palette ==   0) && (sk->skinnum == number) && (sk->surfnum == surf) && (i < 5)) { i = 5; skzero = sk; }
        else if ((sk->palette == pal) && (sk->skinnum ==      0) && (sk->surfnum == surf) && (i < 4)) { i = 4; skzero = sk; }
        else if ((sk->palette ==   0) && (sk->skinnum ==      0) && (sk->surfnum == surf) && (i < 3)) { i = 3; skzero = sk; }
        else if ((sk->palette ==   0) && (sk->skinnum == number) && (i < 2)) { i = 2; skzero = sk; }
        else if ((sk->palette == pal) && (sk->skinnum ==      0) && (i < 1)) { i = 1; skzero = sk; }
        else if ((sk->palette ==   0) && (sk->skinnum ==      0) && (i < 0)) { i = 0; skzero = sk; }
    }

	// Special palettes do not get replacements
	if (pal >= (MAXPALOOKUPS - RESERVEDPALS))
		return 0;

	if (skzero)
	{
		//OSD_Printf("Using def skin 0,0 as fallback, pal=%d\n", pal);
		if (exact) *exact = false;
		return skzero->texture;
	}
	else
		return nullptr;
}

//Note: even though it says md2model, it works for both md2model&md3model
void updateanimation(md2model_t *m, tspriteptr_t tspr, uint8_t lpal)
{
    if (m->numframes < 2)
    {
        m->interpol = 0;
        return;
    }

    int32_t const tile = Ptile2tile(tspr->picnum,lpal);
    m->cframe = m->nframe = tile2model[tile].framenum;

#ifdef DEBUGGINGAIDS
    if (m->cframe >= m->numframes)
        OSD_Printf("1: c > n\n");
#endif

    int32_t const smoothdurationp = (hw_animsmoothing && (tile2model[tile].smoothduration != 0));
    spritesmooth_t * const smooth = &spritesmooth[((unsigned)tspr->owner < MAXSPRITES+MAXUNIQHUDID) ? tspr->owner : MAXSPRITES+MAXUNIQHUDID-1];
    spriteext_t * const sprext = &spriteext[((unsigned)tspr->owner < MAXSPRITES+MAXUNIQHUDID) ? tspr->owner : MAXSPRITES+MAXUNIQHUDID-1];

    const mdanim_t *anim;
    for (anim = m->animations; anim && anim->startframe != m->cframe; anim = anim->next)
    {
        /* do nothing */;
    }

    int32_t i, j, k;
    int32_t fps;

    if (!anim)
    {
        if (!smoothdurationp || ((smooth->mdoldframe == m->cframe) && (smooth->mdcurframe == m->cframe)))
        {
            m->interpol = 0;
            return;
        }

        // assert(smoothdurationp && ((smooth->mdoldframe != m->cframe) || (smooth->mdcurframe != m->cframe)))

        if (smooth->mdoldframe != m->cframe)
        {
            if (smooth->mdsmooth == 0)
            {
                sprext->mdanimtims = mdtims;
                m->interpol = 0;
                smooth->mdsmooth = 1;
                smooth->mdcurframe = m->cframe;
            }
            else if (smooth->mdcurframe != m->cframe)
            {
                sprext->mdanimtims = mdtims;
                m->interpol = 0;
                smooth->mdsmooth = 1;
                smooth->mdoldframe = smooth->mdcurframe;
                smooth->mdcurframe = m->cframe;
            }
        }
        else  // if (smooth->mdcurframe != m->cframe)
        {
            sprext->mdanimtims = mdtims;
            m->interpol = 0;
            smooth->mdsmooth = 1;
            smooth->mdoldframe = smooth->mdcurframe;
            smooth->mdcurframe = m->cframe;
        }
    }
    else if (/* anim && */ sprext->mdanimcur != anim->startframe)
    {
        //if (sprext->flags & SPREXT_NOMDANIM) OSD_Printf("SPREXT_NOMDANIM\n");
        //OSD_Printf("smooth launched ! oldanim %i new anim %i\n", sprext->mdanimcur, anim->startframe);
        sprext->mdanimcur = (int16_t)anim->startframe;
        sprext->mdanimtims = mdtims;
        m->interpol = 0;

        if (!smoothdurationp)
        {
            m->cframe = m->nframe = anim->startframe;

            goto prep_return;
        }

        m->nframe = anim->startframe;
        m->cframe = smooth->mdoldframe;

        smooth->mdsmooth = 1;
        goto prep_return;
    }

    fps = smooth->mdsmooth ? Blrintf((1.0f / ((float)tile2model[tile].smoothduration * (1.f / (float)UINT16_MAX))) * 66.f)
                                   : anim ? anim->fpssc : 1;

    i = (mdtims - sprext->mdanimtims) * ((fps * timerGetClockRate()) / 120);

    j = (smooth->mdsmooth || !anim) ? 65536 : ((anim->endframe + 1 - anim->startframe) << 16);

    // XXX: Just in case you play the game for a VERY long time...
    if (i < 0) { i = 0; sprext->mdanimtims = mdtims; }
    //compare with j*2 instead of j to ensure i stays > j-65536 for MDANIM_ONESHOT
    if (anim && (i >= j+j) && (fps) && !mdpause) //Keep mdanimtims close to mdtims to avoid the use of MOD
        sprext->mdanimtims += j/((fps*timerGetClockRate())/120);

    k = i;

    if (anim && (anim->flags&MDANIM_ONESHOT))
        { if (i > j-65536) i = j-65536; }
    else { if (i >= j) { i -= j; if (i >= j) i %= j; } }

    if (hw_animsmoothing && smooth->mdsmooth)
    {
        m->nframe = anim ? anim->startframe : smooth->mdcurframe;
        m->cframe = smooth->mdoldframe;

        //OSD_Printf("smoothing... cframe %i nframe %i\n", m->cframe, m->nframe);
        if (k > 65535)
        {
            sprext->mdanimtims = mdtims;
            m->interpol = 0;
            smooth->mdsmooth = 0;
            m->cframe = m->nframe; // = anim ? anim->startframe : smooth->mdcurframe;

            smooth->mdoldframe = m->cframe;
            //OSD_Printf("smooth stopped !\n");
            goto prep_return;
        }
    }
    else
    {
        if (anim)
            m->cframe = (i>>16)+anim->startframe;


        m->nframe = m->cframe+1;

        if (anim && m->nframe > anim->endframe)  // VERIFY: (!(hw_animsmoothing && smooth->mdsmooth)) implies (anim!=NULL) ?
            m->nframe = anim->startframe;

        smooth->mdoldframe = m->cframe;
        //OSD_Printf("not smoothing... cframe %i nframe %i\n", m->cframe, m->nframe);
    }

    m->interpol = ((float)(i&65535))/65536.f;
    //OSD_Printf("interpol %f\n", m->interpol);

prep_return:
    if (m->cframe >= m->numframes)
        m->cframe = 0;
    if (m->nframe >= m->numframes)
        m->nframe = 0;
}

//--------------------------------------- MD2 LIBRARY BEGINS ---------------------------------------
static md2model_t *md2load(FileReader & fil, const char *filnam)
{
    md2model_t *m;
    md3model_t *m3;
    md3surf_t *s;
    md2frame_t *f;
    md2head_t head;
    char st[BMAX_PATH];
    int32_t i, j, k;

    int32_t ournumskins, ournumglcmds;

    m = (md2model_t *)Xcalloc(1,sizeof(md2model_t));
    m->mdnum = 2; m->scale = .01f;

    fil.Read((char *)&head,sizeof(md2head_t));
#if B_BIG_ENDIAN != 0
    head.id = B_LITTLE32(head.id);                 head.vers = B_LITTLE32(head.vers);
    head.skinxsiz = B_LITTLE32(head.skinxsiz);     head.skinysiz = B_LITTLE32(head.skinysiz);
    head.framebytes = B_LITTLE32(head.framebytes); head.numskins = B_LITTLE32(head.numskins);
    head.numverts = B_LITTLE32(head.numverts);     head.numuv = B_LITTLE32(head.numuv);
    head.numtris = B_LITTLE32(head.numtris);       head.numglcmds = B_LITTLE32(head.numglcmds);
    head.numframes = B_LITTLE32(head.numframes);   head.ofsskins = B_LITTLE32(head.ofsskins);
    head.ofsuv = B_LITTLE32(head.ofsuv);           head.ofstris = B_LITTLE32(head.ofstris);
    head.ofsframes = B_LITTLE32(head.ofsframes);   head.ofsglcmds = B_LITTLE32(head.ofsglcmds);
    head.ofseof = B_LITTLE32(head.ofseof);
#endif

    if ((head.id != IDP2_MAGIC) || (head.vers != 8)) { Xfree(m); return 0; } //"IDP2"

    ournumskins = head.numskins ? head.numskins : 1;
    ournumglcmds = head.numglcmds ? head.numglcmds : 1;

    m->numskins = head.numskins;
    m->numframes = head.numframes;
    m->numverts = head.numverts;
    m->numglcmds = head.numglcmds;
    m->framebytes = head.framebytes;

    m->frames = (char *)Xmalloc(m->numframes*m->framebytes);
    m->glcmds = (int32_t *)Xmalloc(ournumglcmds*sizeof(int32_t));
    m->tris = (md2tri_t *)Xmalloc(head.numtris*sizeof(md2tri_t));
    m->uv = (md2uv_t *)Xmalloc(head.numuv*sizeof(md2uv_t));

    fil.Seek(head.ofsframes,FileReader::SeekSet);
    if (fil.Read((char *)m->frames,m->numframes*m->framebytes) != m->numframes*m->framebytes)
        { Xfree(m->uv); Xfree(m->tris); Xfree(m->glcmds); Xfree(m->frames); Xfree(m); return 0; }

    if (m->numglcmds > 0)
    {
        fil.Seek(head.ofsglcmds,FileReader::SeekSet);
        if (fil.Read((char *)m->glcmds,m->numglcmds*sizeof(int32_t)) != (int32_t)(m->numglcmds*sizeof(int32_t)))
            { Xfree(m->uv); Xfree(m->tris); Xfree(m->glcmds); Xfree(m->frames); Xfree(m); return 0; }
    }

    fil.Seek(head.ofstris,FileReader::SeekSet);
    if (fil.Read((char *)m->tris,head.numtris*sizeof(md2tri_t)) != (int32_t)(head.numtris*sizeof(md2tri_t)))
        { Xfree(m->uv); Xfree(m->tris); Xfree(m->glcmds); Xfree(m->frames); Xfree(m); return 0; }

    fil.Seek(head.ofsuv,FileReader::SeekSet);
    if (fil.Read((char *)m->uv,head.numuv*sizeof(md2uv_t)) != (int32_t)(head.numuv*sizeof(md2uv_t)))
        { Xfree(m->uv); Xfree(m->tris); Xfree(m->glcmds); Xfree(m->frames); Xfree(m); return 0; }

#if B_BIG_ENDIAN != 0
    {
        char *f = (char *)m->frames;
        int32_t *l,j;
        md2frame_t *fr;

        for (i = m->numframes-1; i>=0; i--)
        {
            fr = (md2frame_t *)f;
            l = (int32_t *)&fr->mul;
            for (j=5; j>=0; j--) l[j] = B_LITTLE32(l[j]);
            f += m->framebytes;
        }

        for (i = m->numglcmds-1; i>=0; i--)
        {
            m->glcmds[i] = B_LITTLE32(m->glcmds[i]);
        }
        for (i = head.numtris-1; i>=0; i--)
        {
            m->tris[i].v[0] = B_LITTLE16(m->tris[i].v[0]);
            m->tris[i].v[1] = B_LITTLE16(m->tris[i].v[1]);
            m->tris[i].v[2] = B_LITTLE16(m->tris[i].v[2]);
            m->tris[i].u[0] = B_LITTLE16(m->tris[i].u[0]);
            m->tris[i].u[1] = B_LITTLE16(m->tris[i].u[1]);
            m->tris[i].u[2] = B_LITTLE16(m->tris[i].u[2]);
        }
        for (i = head.numuv-1; i>=0; i--)
        {
            m->uv[i].u = B_LITTLE16(m->uv[i].u);
            m->uv[i].v = B_LITTLE16(m->uv[i].v);
        }
    }
#endif

    Bstrcpy(st,filnam);
    for (i=strlen(st)-1; i>0; i--)
        if ((st[i] == '/') || (st[i] == '\\')) { i++; break; }
    if (i<0) i=0;
    st[i] = 0;
    m->basepath = (char *)Xmalloc(i+1);
    Bstrcpy(m->basepath, st);

    m->skinfn = (char *)Xmalloc(ournumskins*64);
    if (m->numskins > 0)
    {
        fil.Seek(head.ofsskins,FileReader::SeekSet);
        if (fil.Read(m->skinfn,64*m->numskins) != 64*m->numskins)
            { Xfree(m->glcmds); Xfree(m->frames); Xfree(m); return 0; }
    }

    maxmodelverts = max(maxmodelverts, m->numverts);
    maxmodeltris = max(maxmodeltris, head.numtris);

    //return m;

    // the MD2 is now loaded internally - let's begin the MD3 conversion process
    //OSD_Printf("Beginning md3 conversion.\n");
    m3 = (md3model_t *)Xcalloc(1, sizeof(md3model_t));
	m3->mdnum = 3; m3->texture = nullptr; m3->scale = m->scale;
    m3->head.id = IDP3_MAGIC; m3->head.vers = 15;

    m3->head.flags = 0;

    m3->head.numframes = m->numframes;
    m3->head.numtags = 0; m3->head.numsurfs = 1;
    m3->head.numskins = 0;

    m3->numskins = m3->head.numskins;
    m3->numframes = m3->head.numframes;

    m3->head.frames = (md3frame_t *)Xcalloc(m3->head.numframes, sizeof(md3frame_t));
    m3->muladdframes = (vec3f_t *)Xcalloc(m->numframes * 2, sizeof(vec3f_t));

    f = (md2frame_t *)(m->frames);

    // frames converting
    i = 0;
    while (i < m->numframes)
    {
        f = (md2frame_t *)&m->frames[i*m->framebytes];
        Bstrcpy(m3->head.frames[i].nam, f->name);
        //OSD_Printf("Copied frame %s.\n", m3->head.frames[i].nam);
        m3->muladdframes[i*2] = f->mul;
        m3->muladdframes[i*2+1] = f->add;
        i++;
    }

    m3->head.tags = NULL;

    m3->head.surfs = (md3surf_t *)Xcalloc(1, sizeof(md3surf_t));
    s = m3->head.surfs;

    // model converting
    s->id = IDP3_MAGIC; s->flags = 0;
    s->numframes = m->numframes; s->numshaders = 0;
    s->numtris = head.numtris;
    s->numverts = head.numtris * 3; // oh man talk about memory effectiveness :((((
    // MD2 is actually more accurate than MD3 in term of uv-mapping, because each triangle has a triangle counterpart on the UV-map.
    // In MD3, each vertex unique UV coordinates, meaning that you have to duplicate vertices if you need non-seamless UV-mapping.

    maxmodelverts = max(maxmodelverts, s->numverts);

    Bstrcpy(s->nam, "Dummy surface from MD2");

    s->shaders = NULL;

    s->tris = (md3tri_t *)Xcalloc(head.numtris, sizeof(md3tri_t));
    s->uv = (md3uv_t *)Xcalloc(s->numverts, sizeof(md3uv_t));
    s->xyzn = (md3xyzn_t *)Xcalloc(s->numverts * m->numframes, sizeof(md3xyzn_t));

    //memoryusage += (s->numverts * m->numframes * sizeof(md3xyzn_t));
    //OSD_Printf("Current model geometry memory usage : %i.\n", memoryusage);

    //OSD_Printf("Number of frames : %i\n", m->numframes);
    //OSD_Printf("Number of triangles : %i\n", head.numtris);
    //OSD_Printf("Number of vertices : %i\n", s->numverts);

    // triangle converting
    i = 0;
    while (i < head.numtris)
    {
        j = 0;
        //OSD_Printf("Triangle : %i\n", i);
        while (j < 3)
        {
            // triangle vertex indexes
            s->tris[i].i[j] = i*3 + j;

            // uv coords
            s->uv[i*3+j].u = (float)(m->uv[m->tris[i].u[j]].u) / (float)(head.skinxsiz);
            s->uv[i*3+j].v = (float)(m->uv[m->tris[i].u[j]].v) / (float)(head.skinysiz);

            // vertices for each frame
            k = 0;
            while (k < m->numframes)
            {
                f = (md2frame_t *)&m->frames[k*m->framebytes];
                s->xyzn[(k*s->numverts) + (i*3) + j].x = (int16_t) (((f->verts[m->tris[i].v[j]].v[0] * f->mul.x) + f->add.x) * 64.f);
                s->xyzn[(k*s->numverts) + (i*3) + j].y = (int16_t) (((f->verts[m->tris[i].v[j]].v[1] * f->mul.y) + f->add.y) * 64.f);
                s->xyzn[(k*s->numverts) + (i*3) + j].z = (int16_t) (((f->verts[m->tris[i].v[j]].v[2] * f->mul.z) + f->add.z) * 64.f);

                k++;
            }
            j++;
        }
        //OSD_Printf("End triangle.\n");
        i++;
    }
    //OSD_Printf("Finished md3 conversion.\n");

    {
        mdskinmap_t *sk;

        sk = (mdskinmap_t *)Xcalloc(1,sizeof(mdskinmap_t));
        sk->palette = 0;
        sk->skinnum = 0;
        sk->surfnum = 0;

        if (m->numskins > 0)
        {
			FStringf fn("%s%s", m->basepath, m->skinfn);
			sk->texture = TileFiles.GetTexture(fn);
			if (!sk->texture)
			{
				initprintf("Unable to load %s as model skin\n", m->skinfn);
			}
        }
        m3->skinmap = sk;
    }

    m3->indexes = (uint16_t *)Xmalloc(sizeof(uint16_t) * s->numtris);
    m3->vindexes = (uint16_t *)Xmalloc(sizeof(uint16_t) * s->numtris * 3);
    m3->maxdepths = (float *)Xmalloc(sizeof(float) * s->numtris);

    // die MD2 ! DIE !
    Xfree(m->skinfn); Xfree(m->basepath); Xfree(m->uv); Xfree(m->tris); Xfree(m->glcmds); Xfree(m->frames); Xfree(m);

    return ((md2model_t *)m3);
}
//---------------------------------------- MD2 LIBRARY ENDS ----------------------------------------

//--------------------------------------- MD3 LIBRARY BEGINS ---------------------------------------

static md3model_t *md3load(FileReader & fil)
{
    int32_t i, surfi, ofsurf, offs[4], leng[4];
    int32_t maxtrispersurf;
    md3model_t *m;
    md3surf_t *s;

    m = (md3model_t *)Xcalloc(1,sizeof(md3model_t));
    m->mdnum = 3; m->texture = nullptr; m->scale = .01f;

    m->muladdframes = NULL;

    fil.Read(&m->head,SIZEOF_MD3HEAD_T);

#if B_BIG_ENDIAN != 0
    m->head.id = B_LITTLE32(m->head.id);             m->head.vers = B_LITTLE32(m->head.vers);
    m->head.flags = B_LITTLE32(m->head.flags);       m->head.numframes = B_LITTLE32(m->head.numframes);
    m->head.numtags = B_LITTLE32(m->head.numtags);   m->head.numsurfs = B_LITTLE32(m->head.numsurfs);
    m->head.numskins = B_LITTLE32(m->head.numskins); m->head.ofsframes = B_LITTLE32(m->head.ofsframes);
    m->head.ofstags = B_LITTLE32(m->head.ofstags); m->head.ofssurfs = B_LITTLE32(m->head.ofssurfs);
    m->head.eof = B_LITTLE32(m->head.eof);
#endif

    if ((m->head.id != IDP3_MAGIC) && (m->head.vers != 15)) { Xfree(m); return 0; } //"IDP3"

    m->numskins = m->head.numskins; //<- dead code?
    m->numframes = m->head.numframes;

    ofsurf = m->head.ofssurfs;

    fil.Seek(m->head.ofsframes,FileReader::SeekSet); i = m->head.numframes*sizeof(md3frame_t);
    m->head.frames = (md3frame_t *)Xmalloc(i);
    fil.Read(m->head.frames,i);

    if (m->head.numtags == 0) m->head.tags = NULL;
    else
    {
        fil.Seek(m->head.ofstags,FileReader::SeekSet); i = m->head.numtags*sizeof(md3tag_t);
        m->head.tags = (md3tag_t *)Xmalloc(i);
        fil.Read(m->head.tags,i);
    }

    fil.Seek(m->head.ofssurfs,FileReader::SeekSet);
    m->head.surfs = (md3surf_t *)Xcalloc(m->head.numsurfs, sizeof(md3surf_t));
    // NOTE: We assume that NULL is represented by all-zeros.
    // surfs[0].geometry is for POLYMER_MD_PROCESS_CHECK (else: crashes).
    // surfs[i].geometry is for FREE_SURFS_GEOMETRY.
    Bassert(m->head.surfs[0].geometry == NULL);

#if B_BIG_ENDIAN != 0
    {
        int32_t j, *l;

        for (i = m->head.numframes-1; i>=0; i--)
        {
            l = (int32_t *)&m->head.frames[i].min;
            for (j=3+3+3+1-1; j>=0; j--) l[j] = B_LITTLE32(l[j]);
        }

        for (i = m->head.numtags-1; i>=0; i--)
        {
            l = (int32_t *)&m->head.tags[i].p;
            for (j=3+3+3+3-1; j>=0; j--) l[j] = B_LITTLE32(l[j]);
        }
    }
#endif

    maxtrispersurf = 0;

    for (surfi=0; surfi<m->head.numsurfs; surfi++)
    {
        s = &m->head.surfs[surfi];
        fil.Seek(ofsurf,FileReader::SeekSet); fil.Read(s,SIZEOF_MD3SURF_T);

#if B_BIG_ENDIAN != 0
        {
            int32_t j, *l;
            s->id = B_LITTLE32(s->id);
            l =	(int32_t *)&s->flags;
            for	(j=1+1+1+1+1+1+1+1+1+1-1; j>=0; j--) l[j] = B_LITTLE32(l[j]);
        }
#endif

        offs[0] = ofsurf+s->ofstris;
        offs[1] = ofsurf+s->ofsshaders;
        offs[2] = ofsurf+s->ofsuv;
        offs[3] = ofsurf+s->ofsxyzn;

        leng[0] = s->numtris*sizeof(md3tri_t);
        leng[1] = s->numshaders*sizeof(md3shader_t);
        leng[2] = s->numverts*sizeof(md3uv_t);
        leng[3] = s->numframes*s->numverts*sizeof(md3xyzn_t);

        //memoryusage += (s->numverts * s->numframes * sizeof(md3xyzn_t));
        //OSD_Printf("Current model geometry memory usage : %i.\n", memoryusage);

        s->tris = (md3tri_t *)Xmalloc((leng[0] + leng[1]) + (leng[2] + leng[3]));

        s->shaders = (md3shader_t *)(((intptr_t)s->tris)+leng[0]);
        s->uv      = (md3uv_t *)(((intptr_t)s->shaders)+leng[1]);
        s->xyzn    = (md3xyzn_t *)(((intptr_t)s->uv)+leng[2]);

        fil.Seek(offs[0],FileReader::SeekSet); fil.Read(s->tris   ,leng[0]);
        fil.Seek(offs[1],FileReader::SeekSet); fil.Read(s->shaders,leng[1]);
        fil.Seek(offs[2],FileReader::SeekSet); fil.Read(s->uv     ,leng[2]);
        fil.Seek(offs[3],FileReader::SeekSet); fil.Read(s->xyzn   ,leng[3]);

#if B_BIG_ENDIAN != 0
        {
            int32_t j, *l;

            for (i=s->numtris-1; i>=0; i--)
            {
                for (j=2; j>=0; j--) s->tris[i].i[j] = B_LITTLE32(s->tris[i].i[j]);
            }
            for (i=s->numshaders-1; i>=0; i--)
            {
                s->shaders[i].i = B_LITTLE32(s->shaders[i].i);
            }
            for (i=s->numverts-1; i>=0; i--)
            {
                l = (int32_t *)&s->uv[i].u;
                l[0] = B_LITTLE32(l[0]);
                l[1] = B_LITTLE32(l[1]);
            }
            for (i=s->numframes*s->numverts-1; i>=0; i--)
            {
                s->xyzn[i].x = (int16_t)B_LITTLE16((uint16_t)s->xyzn[i].x);
                s->xyzn[i].y = (int16_t)B_LITTLE16((uint16_t)s->xyzn[i].y);
                s->xyzn[i].z = (int16_t)B_LITTLE16((uint16_t)s->xyzn[i].z);
            }
        }
#endif
        maxmodelverts = max(maxmodelverts, s->numverts);
        maxmodeltris = max(maxmodeltris, s->numtris);
        maxtrispersurf = max(maxtrispersurf, s->numtris);
        ofsurf += s->ofsend;
    }

    m->indexes = (uint16_t *)Xmalloc(sizeof(uint16_t) * maxtrispersurf);
    m->vindexes = (uint16_t *)Xmalloc(sizeof(uint16_t) * maxtrispersurf * 3);
    m->maxdepths = (float *)Xmalloc(sizeof(float) * maxtrispersurf);

    return m;
}

static void      md3postload_common(md3model_t *m)
{
    int         framei, surfi, verti;
    md3frame_t  *frame;
    md3xyzn_t   *frameverts;
    float       dist, vec1[3];

    // apparently we can't trust loaded models bounding box/sphere information,
    // so let's compute it ourselves

    framei = 0;

    while (framei < m->head.numframes)
    {
        frame = &m->head.frames[framei];

        Bmemset(&frame->min, 0, sizeof(vec3f_t));
        Bmemset(&frame->max, 0, sizeof(vec3f_t));

        frame->r        = 0.0f;

        surfi = 0;
        while (surfi < m->head.numsurfs)
        {
            frameverts = &m->head.surfs[surfi].xyzn[framei * m->head.surfs[surfi].numverts];

            verti = 0;
            while (verti < m->head.surfs[surfi].numverts)
            {
                if (!verti && !surfi)
                {
                    md3xyzn_t const & framevert = frameverts[0];

                    frame->min.x    = framevert.x;
                    frame->min.y    = framevert.y;
                    frame->min.z    = framevert.z;

                    frame->max      = frame->min;
                }
                else
                {
                    md3xyzn_t const & framevert = frameverts[verti];

                    if (frame->min.x > framevert.x)
                        frame->min.x = framevert.x;
                    if (frame->max.x < framevert.x)
                        frame->max.x = framevert.x;

                    if (frame->min.y > framevert.y)
                        frame->min.y = framevert.y;
                    if (frame->max.y < framevert.y)
                        frame->max.y = framevert.y;

                    if (frame->min.z > framevert.z)
                        frame->min.z = framevert.z;
                    if (frame->max.z < framevert.z)
                        frame->max.z = framevert.z;
                }

                ++verti;
            }

            ++surfi;
        }

        frame->cen.x = (frame->min.x + frame->max.x) * .5f;
        frame->cen.y = (frame->min.y + frame->max.y) * .5f;
        frame->cen.z = (frame->min.z + frame->max.z) * .5f;

        surfi = 0;
        while (surfi < m->head.numsurfs)
        {
            md3surf_t const & surf = m->head.surfs[surfi];

            frameverts = &surf.xyzn[framei * surf.numverts];

            verti = 0;
            while (verti < surf.numverts)
            {
                md3xyzn_t const & framevert = frameverts[verti];

                vec1[0] = framevert.x - frame->cen.x;
                vec1[1] = framevert.y - frame->cen.y;
                vec1[2] = framevert.z - frame->cen.z;

                dist = vec1[0] * vec1[0] + vec1[1] * vec1[1] + vec1[2] * vec1[2];

                if (dist > frame->r)
                    frame->r = dist;

                ++verti;
            }

            ++surfi;
        }

        frame->r = Bsqrtf(frame->r);

        ++framei;
    }
}

#ifdef POLYMER
// pre-check success of conversion since it must not fail later.
// keep in sync with md3postload_polymer!
static int md3postload_polymer_check(md3model_t *m)
{
    ssize_t surfi, trii;
    md3surf_t   *s;

    surfi = 0;
    while (surfi < m->head.numsurfs)
    {
        s = &m->head.surfs[surfi];

        uint32_t const numverts = s->numverts;

        trii = 0;
        while (trii < s->numtris)
        {
            uint32_t const * const u = (uint32_t const *)s->tris[trii].i;

            // let the vertices know they're being referenced by a triangle
            if (u[0] >= numverts || u[1] >= numverts || u[2] >= numverts)
            {
                // corrupt model
                OSD_Printf("%s: Triangle index out of bounds!\n", m->head.nam);
                return 1;
            }

            ++trii;
        }

        ++surfi;
    }

    return 0;
}

// Precalculated cos/sin arrays.
static float g_mdcos[256], g_mdsin[256];
static int32_t mdtrig_init = 0;

static void init_mdtrig_arrays(void)
{
    int32_t i;

    for (i=0; i<256; i++)
    {
        float ang = i * (2.f * fPI) * (1.f/255.f);
        g_mdcos[i] = cosf(ang);
        g_mdsin[i] = sinf(ang);
    }

    mdtrig_init = 1;
}
#endif

int      md3postload_polymer(md3model_t *m)
{
#ifdef POLYMER
    int         framei, surfi, verti, trii;
    float       vec1[5], vec2[5], mat[9], r;

    // POLYMER_MD_PROCESS_CHECK
    if (m->head.surfs[0].geometry)
        return -1;  // already postprocessed

    if (!mdtrig_init)
        init_mdtrig_arrays();

    // let's also repack the geometry to more usable formats

    surfi = 0;
    while (surfi < m->head.numsurfs)
    {
        handleevents();

        md3surf_t *const s = &m->head.surfs[surfi];
#ifdef DEBUG_MODEL_MEM
        i = (m->head.numframes * s->numverts * sizeof(float) * 15);
        if (i > 1<<20)
            initprintf("size %d (%d fr, %d v): md %s surf %d/%d\n", i, m->head.numframes, s->numverts,
                       m->head.nam, surfi, m->head.numsurfs);
#endif
        s->geometry = (float *)Xcalloc(m->head.numframes * s->numverts * 15, sizeof(float));

        if (s->numverts > tribufverts)
        {
            tribuf = (int32_t *) Xrealloc(tribuf, s->numverts * sizeof(int32_t));
            tribufverts = s->numverts;
        }

        Bmemset(tribuf, 0, s->numverts * sizeof(int32_t));

        verti = 0;
        while (verti < (m->head.numframes * s->numverts))
        {
            md3xyzn_t const & xyzn = s->xyzn[verti];

            // normal extraction from packed spherical coordinates
            // FIXME: swapping lat and lng because of npherno's compiler
            uint8_t lat = xyzn.nlng;
            uint8_t lng = xyzn.nlat;
            size_t verti15 = (size_t)verti * 15;

            s->geometry[verti15 + 0] = xyzn.x;
            s->geometry[verti15 + 1] = xyzn.y;
            s->geometry[verti15 + 2] = xyzn.z;

            s->geometry[verti15 + 3] = g_mdcos[lat] * g_mdsin[lng];
            s->geometry[verti15 + 4] = g_mdsin[lat] * g_mdsin[lng];
            s->geometry[verti15 + 5] = g_mdcos[lng];

            ++verti;
        }

        uint32_t numverts = s->numverts;

        trii = 0;
        while (trii < s->numtris)
        {
            int32_t const * const i = s->tris[trii].i;
            uint32_t const * const u = (uint32_t const *)i;

            // let the vertices know they're being referenced by a triangle
            if (u[0] >= numverts ||u[1] >= numverts || u[2] >= numverts)
            {
                // corrupt model
                return 0;
            }
            tribuf[u[0]]++;
            tribuf[u[1]]++;
            tribuf[u[2]]++;

            uint32_t const tris15[] = { u[0] * 15, u[1] * 15, u[2] * 15 };


            framei = 0;
            while (framei < m->head.numframes)
            {
                const uint32_t verti15 = framei * s->numverts * 15;

                vec1[0] = s->geometry[verti15 + tris15[1]]     - s->geometry[verti15 + tris15[0]];
                vec1[1] = s->geometry[verti15 + tris15[1] + 1] - s->geometry[verti15 + tris15[0] + 1];
                vec1[2] = s->geometry[verti15 + tris15[1] + 2] - s->geometry[verti15 + tris15[0] + 2];
                vec1[3] = s->uv[u[1]].u - s->uv[u[0]].u;
                vec1[4] = s->uv[u[1]].v - s->uv[u[0]].v;

                vec2[0] = s->geometry[verti15 + tris15[2]]     - s->geometry[verti15 + tris15[1]];
                vec2[1] = s->geometry[verti15 + tris15[2] + 1] - s->geometry[verti15 + tris15[1] + 1];
                vec2[2] = s->geometry[verti15 + tris15[2] + 2] - s->geometry[verti15 + tris15[1] + 2];
                vec2[3] = s->uv[u[2]].u - s->uv[u[1]].u;
                vec2[4] = s->uv[u[2]].v - s->uv[u[1]].v;

                r = (vec1[3] * vec2[4] - vec2[3] * vec1[4]);
                if (r != 0.0f)
                {
                    r = 1.f/r;

                    // tangent
                    mat[0] = (vec2[4] * vec1[0] - vec1[4] * vec2[0]) * r;
                    mat[1] = (vec2[4] * vec1[1] - vec1[4] * vec2[1]) * r;
                    mat[2] = (vec2[4] * vec1[2] - vec1[4] * vec2[2]) * r;

                    normalize(&mat[0]);

                    // bitangent
                    mat[3] = (vec1[3] * vec2[0] - vec2[3] * vec1[0]) * r;
                    mat[4] = (vec1[3] * vec2[1] - vec2[3] * vec1[1]) * r;
                    mat[5] = (vec1[3] * vec2[2] - vec2[3] * vec1[2]) * r;

                    normalize(&mat[3]);
                }
                else
                    Bmemset(mat, 0, sizeof(float) * 6);

                // T and B are shared for the three vertices in that triangle
                size_t const offs = (framei * numverts * 15) + 6;
                size_t j = 0;
                do
                {
                    size_t const offsi = offs + j;
                    s->geometry[offsi + tris15[0]] += mat[j];
                    s->geometry[offsi + tris15[1]] += mat[j];
                    s->geometry[offsi + tris15[2]] += mat[j];
                }
                while (++j < 6);

                ++framei;
            }

            ++trii;
        }

        // now that we accumulated the TBNs, average and invert them for each vertex
        int verti_end = m->head.numframes * s->numverts;

        verti = 0;
        while (verti < verti_end)
        {
            const int32_t curnumtris = tribuf[verti % s->numverts];
            uint32_t const verti15 = verti * 15;

            if (curnumtris > 0)
            {
                const float rfcurnumtris = 1.f/(float)curnumtris;
                size_t i = 6;
                do
                    s->geometry[i + verti15] *= rfcurnumtris;
                while (++i < 12);
            }
#ifdef DEBUG_MODEL_MEM
            else if (verti == verti%s->numverts)
            {
                OSD_Printf("%s: vert %d is unused\n", m->head.nam, verti);
            }
#endif
            // copy N over
            Bmemcpy(&s->geometry[verti15 + 12], &s->geometry[verti15 + 3], sizeof(float) * 3);
            invertmatrix(&s->geometry[verti15 + 6], mat);
            Bmemcpy(&s->geometry[verti15 + 6], mat, sizeof(float) * 9);

            ++verti;
        }

        ++surfi;
    }

#else
    UNREFERENCED_PARAMETER(m);
#endif

    return 1;
}

static void md3free(md3model_t *m)
{
    mdanim_t *anim, *nanim = NULL;
    mdskinmap_t *sk, *nsk = NULL;

    if (!m) return;

    for (anim=m->animations; anim; anim=nanim)
    {
        nanim = anim->next;
        Xfree(anim);
    }
    for (sk=m->skinmap; sk; sk=nsk)
    {
        nsk = sk->next;
        Xfree(sk);
    }

    if (m->head.surfs)
    {
        for (bssize_t surfi=m->head.numsurfs-1; surfi>=0; surfi--)
        {
            md3surf_t *s = &m->head.surfs[surfi];
            Xfree(s->tris);
            Xfree(s->geometry);  // FREE_SURFS_GEOMETRY
        }
        Xfree(m->head.surfs);
    }
    Xfree(m->head.tags);
    Xfree(m->head.frames);
	
    Xfree(m->muladdframes);

    Xfree(m->indexes);
    Xfree(m->vindexes);
    Xfree(m->maxdepths);

    Xfree(m);
}

//---------------------------------------- MD3 LIBRARY ENDS ----------------------------------------
//--------------------------------------- MD LIBRARY BEGINS  ---------------------------------------

static mdmodel_t *mdload(const char *filnam)
{
    mdmodel_t *vm;
    int32_t i;

    vm = (mdmodel_t *)voxload(filnam);
    if (vm) return vm;

    auto fil = fileSystem.OpenFileReader(filnam,0);

    if (!fil.isOpen())
        return NULL;

    fil.Read(&i,4);
    fil.Seek(0,FileReader::SeekSet);

    switch (B_LITTLE32(i))
    {
    case IDP2_MAGIC:
//        initprintf("Warning: model \"%s\" is version IDP2; wanted version IDP3\n",filnam);
        vm = (mdmodel_t *)md2load(fil,filnam);
        break; //IDP2
    case IDP3_MAGIC:
        vm = (mdmodel_t *)md3load(fil);
        break; //IDP3
    default:
        vm = NULL;
        break;
    }

    if (vm)
    {
        md3model_t *vm3 = (md3model_t *)vm;

        // smuggle the file name into the model struct.
        // head.nam is unused as far as I can tell
        Bstrncpyz(vm3->head.nam, filnam, sizeof(vm3->head.nam));

        md3postload_common(vm3);

    }

    return vm;
}


int32_t polymost_mddraw(tspriteptr_t tspr)
{
    if (maxmodelverts > allocmodelverts)
    {
        vertlist = (vec3f_t *) Xrealloc(vertlist, sizeof(vec3f_t)*maxmodelverts);
        allocmodelverts = maxmodelverts;
    }

    mdmodel_t *const vm = models[tile2model[Ptile2tile(tspr->picnum,
    (tspr->owner >= MAXSPRITES) ? tspr->pal : sprite[tspr->owner].pal)].modelid];
    if (vm->mdnum == 1)
        return rendermode->voxdraw((voxmodel_t *)vm, tspr);
    else if (vm->mdnum == 3)
        return rendermode->md3draw((md3model_t *)vm, tspr);
    return 0;
}

static void mdfree(mdmodel_t *vm)
{
    if (vm->mdnum == 1) { voxfree((voxmodel_t *)vm); return; }
    if (vm->mdnum == 2 || vm->mdnum == 3) { md3free((md3model_t *)vm); return; }
}

#endif

//---------------------------------------- MD LIBRARY ENDS  ----------------------------------------
