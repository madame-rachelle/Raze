#ifndef polymost_h_
#define polymost_h_


#include "baselayer.h"  // glinfo
#include "hightile.h"
#include "mdsprite.h"
#include "build.h"

void Polymost_CacheHitList(uint8_t *hash);

typedef struct
{
    uint8_t r, g, b, a;
} coltype;
typedef struct
{
    float r, g, b, a;
} coltypef;

extern bool    playing_rr;
extern int32_t rendmode;
extern float   glox1, gloy1;
extern float   grhalfxdown10x;
extern float   gchang, gshang, gctang, gstang;
extern float   gvrcorrection;

//void phex(char v, char *s);
void polymost_fillpolygon(int32_t npoints);

float *multiplyMatrix4f(float m0[4 * 4], const float m1[4 * 4]);

void polymost_glinit(void);

enum
{
    INVALIDATE_ALL,
    INVALIDATE_ART,
    INVALIDATE_ALL_NON_INDEXED,
    INVALIDATE_ART_NON_INDEXED
};


extern float curpolygonoffset;

extern uint8_t alphahackarray[MAXTILES];

extern int32_t r_scenebrightness;
extern int32_t polymostcenterhoriz;

extern int16_t globalpicnum;

#define POLYMOST_CHOOSE_FOG_PAL(fogpal, pal) ((fogpal) ? (fogpal) : (pal))
static FORCE_INLINE int32_t get_floor_fogpal(usectorptr_t const sec) { return POLYMOST_CHOOSE_FOG_PAL(sec->fogpal, sec->floorpal); }
static FORCE_INLINE int32_t get_ceiling_fogpal(usectorptr_t const sec) { return POLYMOST_CHOOSE_FOG_PAL(sec->fogpal, sec->ceilingpal); }
static FORCE_INLINE int32_t fogshade(int32_t const shade, int32_t const pal) { return (globalflags & GLOBAL_NO_GL_FOGSHADE) ? 0 : shade; }

static FORCE_INLINE int check_nonpow2(int32_t const x) { return (x > 1 && (x & (x - 1))); }

static inline float polymost_invsqrt_approximation(float x)
{
#ifdef B_LITTLE_ENDIAN
    float const haf = x * .5f;
    union {
        float    f;
        uint32_t i;
    } n = { x };
    n.i = 0x5f375a86 - (n.i >> 1);
    return n.f * (1.5f - haf * (n.f * n.f));
#else
    // this is the comment
    return 1.f / Bsqrtf(x);
#endif
}

// Flags of the <dameth> argument of various functions
enum
{
    DAMETH_NOMASK = 0,
    DAMETH_MASK   = 1,
    DAMETH_TRANS1 = 2,
    DAMETH_TRANS2 = 3,

    DAMETH_MASKPROPS = 3,

    DAMETH_CLAMPED = 4,
    DAMETH_MODEL   = 8,
    DAMETH_SKY     = 16,

    DAMETH_WALL = 32,  // signals a texture for a wall (for r_npotwallmode)

    // used internally by polymost_domost
    DAMETH_BACKFACECULL = -1,
};

#define DAMETH_NARROW_MASKPROPS(dameth) (((dameth) & (~DAMETH_TRANS1)) | (((dameth)&DAMETH_TRANS1) >> 1))
EDUKE32_STATIC_ASSERT(DAMETH_NARROW_MASKPROPS(DAMETH_MASKPROPS) == DAMETH_MASK);

extern int32_t globalnoeffect;
extern int32_t drawingskybox;
extern int32_t hicprecaching;
extern float   fcosglobalang, fsinglobalang;
extern float   fxdim, fydim, fydimen, fviewingrange;

extern char ptempbuf[MAXWALLSB << 1];

extern hitdata_t polymost_hitdata;

class Rendermode
{
  public:
    virtual void outputGLDebugMessage(uint8_t severity, const char *format, ...) = 0;
    virtual void gltexapplyprops() = 0;
    virtual void glreset() = 0;
    virtual void uploadbasepalette(int32_t basepalnum) = 0;
    virtual void uploadpalswaps(int count, int32_t *palookupnum) = 0;
    virtual int32_t maskWallHasTranslucency(uwalltype const *const wall) = 0;
    virtual int32_t spriteHasTranslucency(tspritetype const *const tspr) = 0;
    virtual void scansector(int32_t sectnum) = 0;
    virtual void drawrooms() = 0;
    virtual void drawmaskwall(int32_t damaskwallcnt) = 0;
    virtual void prepareMirror(int32_t dax, int32_t day, int32_t daz, fix16_t daang, fix16_t dahoriz, int16_t mirrorWall) = 0;
    virtual void completeMirror() = 0;
    virtual void prepare_loadboard() = 0;
    virtual void drawsprite(int32_t snum) = 0;
    virtual void dorotatespritemodel(int32_t sx, int32_t sy, int32_t z, int16_t a, int16_t picnum, int8_t dashade, uint8_t dapalnum, int32_t dastat, uint8_t daalpha, uint8_t dablend, int32_t uniqid) = 0;
    virtual void initosdfuncs() = 0;
    virtual void PrecacheHardwareTextures(int nTile) = 0;
    virtual void Startup() = 0;
    virtual int32_t voxdraw(voxmodel_t *m, tspriteptr_t const tspr) = 0;
    virtual int32_t md3draw(md3model_t *m, tspriteptr_t tspr) = 0;
    virtual void renderSetRollAngle(int32_t rolla) = 0;
};

extern Rendermode *rendermode;

class PolymostRendermode : public Rendermode
{
  public:
    void outputGLDebugMessage(uint8_t severity, const char *format, ...) override;
    void gltexapplyprops() override;
    void glreset() override;
    void uploadbasepalette(int32_t basepalnum) override;
    void uploadpalswaps(int count, int32_t *palookupnum) override;
    int32_t maskWallHasTranslucency(uwalltype const *const wall) override;
    int32_t spriteHasTranslucency(tspritetype const *const tspr) override;
    void scansector(int32_t sectnum) override;
    void drawrooms() override;
    void drawmaskwall(int32_t damaskwallcnt) override;
    void prepareMirror(int32_t dax, int32_t day, int32_t daz, fix16_t daang, fix16_t dahoriz, int16_t mirrorWall) override;
    void completeMirror() override;
    void prepare_loadboard() override;
    void drawsprite(int32_t snum) override;
    void dorotatespritemodel(int32_t sx, int32_t sy, int32_t z, int16_t a, int16_t picnum, int8_t dashade, uint8_t dapalnum, int32_t dastat, uint8_t daalpha, uint8_t dablend, int32_t uniqid) override;
    void initosdfuncs() override;
    void PrecacheHardwareTextures(int nTile) override;
    void Startup() override;
    int32_t voxdraw(voxmodel_t *m, tspriteptr_t const tspr) override;
    int32_t md3draw(md3model_t *m, tspriteptr_t tspr) override;
    void renderSetRollAngle(int32_t rolla) override;
};

class HardwareRendermode : public Rendermode
{
  public:
    void outputGLDebugMessage(uint8_t severity, const char *format, ...) override;
    void gltexapplyprops() override;
    void glreset() override;
    void uploadbasepalette(int32_t basepalnum) override;
    void uploadpalswaps(int count, int32_t *palookupnum) override;
    int32_t maskWallHasTranslucency(uwalltype const *const wall) override;
    int32_t spriteHasTranslucency(tspritetype const *const tspr) override;
    void scansector(int32_t sectnum) override;
    void drawrooms() override;
    void drawmaskwall(int32_t damaskwallcnt) override;
    void prepareMirror(int32_t dax, int32_t day, int32_t daz, fix16_t daang, fix16_t dahoriz, int16_t mirrorWall) override;
    void completeMirror() override;
    void prepare_loadboard() override;
    void drawsprite(int32_t snum) override;
    void dorotatespritemodel(int32_t sx, int32_t sy, int32_t z, int16_t a, int16_t picnum, int8_t dashade, uint8_t dapalnum, int32_t dastat, uint8_t daalpha, uint8_t dablend, int32_t uniqid) override;
    void initosdfuncs() override;
    void PrecacheHardwareTextures(int nTile) override;
    void Startup() override;
    int32_t voxdraw(voxmodel_t *m, tspriteptr_t const tspr) override;
    int32_t md3draw(md3model_t *m, tspriteptr_t tspr) override;
    void renderSetRollAngle(int32_t rolla) override;
};

extern PolymostRendermode polymost_rendermode;
extern HardwareRendermode hardware_rendermode;

#endif
