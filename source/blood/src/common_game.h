//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 Nuke.YKT

This file is part of NBlood.

NBlood is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License version 2
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
//-------------------------------------------------------------------------
#pragma once
#include "build.h"

#include "pragmas.h"
#include "misc.h"
#include "printf.h"
#include "v_text.h"

BEGIN_BLD_NS

#define BLOODWIDESCREENDEF "blood_widescreen.def"

#define BYTEVERSION 103
#define EXEVERSION 101

void _SetErrorLoc(const char *pzFile, int nLine);
void __dassert(const char *pzExpr, const char *pzFile, int nLine);
void QuitGame(void);
void _consoleSysMsg(const char* pMessage, ...);

#define ThrowError(...) \
	{ \
		_SetErrorLoc(__FILE__,__LINE__); \
		I_Error(__VA_ARGS__); \
	}

// print error to console only
#define consoleSysMsg(...) \
	{ \
		_SetErrorLoc(__FILE__,__LINE__); \
		_consoleSysMsg(__VA_ARGS__); \
	}

#define dassert(x) assert(x)


#define kMaxSectors MAXSECTORS
#define kMaxWalls MAXWALLS
#define kMaxSprites MAXSPRITES

#define kMaxTiles MAXTILES
#define kMaxStatus MAXSTATUS
#define kMaxPlayers 8
#define kMaxViewSprites maxspritesonscreen

#define kMaxVoxels MAXVOXELS

#define kTicRate 120
#define kTicsPerFrame 4
#define kTicsPerSec (kTicRate/kTicsPerFrame)

#define TILTBUFFER 4078

#define kExplodeMax 8

#define kLensSize 80
#define kViewEffectMax 19

#define kNoTile -1


// defined by NoOne:
// -------------------------------

#define kMaxPAL 5
#define kUserPLUStart  15

#define kDmgFall 0
#define kDmgBurn 1
#define kDmgBullet 2
#define kDmgExplode 3
#define kDmgChoke 4
#define kDmgSpirit 5
#define kDmgElectric 6
#define kDmgMax 7

// MEDIUM /////////////////////////////////////////////////////
enum {
kMediumNormal                   = 0,
kMediumWater                    = 1,
kMediumGoo                      = 2,
};

// STATNUMS /////////////////////////////////////////////////////
enum {
kStatNothing                    = -1,
kStatDecoration                 = 0,
kStatFX                         = 1,
kStatExplosion                  = 2,
kStatItem                       = 3,
kStatThing                      = 4,
kStatProjectile                 = 5,
kStatDude                       = 6,
kStatInactive                   = 7, // inactive (ambush) dudes
kStatRespawn                    = 8,
kStatPurge                      = 9,
kStatMarker                     = 10,
kStatTraps                      = 11,
kStatAmbience                   = 12,
kStatSpares                     = 13,
kStatFlare                      = 14,
kStatDebris                     = 15,
kStatPathMarker                 = 16,
kStatFree                       = 1024,
};

// POWERUPS /////////////////////////////////////////////////////
enum {
kPwUpFeatherFall        = 12,
kPwUpShadowCloak        = 13,
kPwUpDeathMask          = 14,
kPwUpJumpBoots          = 15,
kPwUpTwoGuns            = 17,
kPwUpDivingSuit         = 18,
kPwUpGasMask            = 19,
kPwUpCrystalBall        = 21,
kPwUpDoppleganger       = 23,
kPwUpReflectShots       = 24,
kPwUpBeastVision        = 25,
kPwUpShadowCloakUseless = 26,
kPwUpDeliriumShroom     = 28,
kPwUpGrowShroom         = 29,
kPwUpShrinkShroom       = 30,
kPwUpDeathMaskUseless   = 31,
kPwUpAsbestArmor        = 39,
kMaxPowerUps            = 51,
};

enum {
    kExplosionSmall = 0,
    kExplosionStandard = 1,
    kExplosionLarge = 2,
    kExplosionFireball = 3,
    kExplosionSpray = 4,
    kExplosion5 = 5,
    kExplosion6 = 6,
    kExplosionNapalm = 7,
    kExplosionMax = 8
};

// SPRITE TYPES /////////////////////////////////////////////////
enum {
    kSpriteDecoration = 0,

    // markers
    kMarkerSPStart = 1,
    kMarkerMPStart = 2,
    kMarkerOff = 3,
    kMarkerOn = 4,
    kMarkerAxis = 5,
    kMarkerLowLink = 6,
    kMarkerUpLink = 7,
    kMarkerWarpDest = 8,
    kMarkerUpWater = 9,
    kMarkerLowWater = 10,
    kMarkerUpStack = 11,
    kMarkerLowStack = 12,
    kMarkerUpGoo = 13,
    kMarkerLowGoo = 14,
    kMarkerPath = 15,
    kMarkerDudeSpawn = 18,
    kMarkerEarthQuake = 19,

    // switches
    kSwitchBase = 20,
    kSwitchToggle = 20,
    kSwitchOneWay = 21,
    kSwitchCombo = 22,
    kSwitchPadlock = 23,
    kSwitchMax = 24,

    // decorations
    kDecorationTorch = 30,
    kDecorationCandle = 32,

    // (weapons)
    kItemWeaponBase = 40,
    kItemWeaponSawedoff = 41,
    kItemWeaponTommygun = 42,
    kItemWeaponVoodooDoll = 44,
    kItemWeaponLifeLeech = 50,
    kItemWeaponMax = 51,

    // items (ammos)
    kItemAmmoBase = 60,
    kItemAmmoSawedoffFew = 67,
    kItemAmmoTommygunFew = 69,
    kAmmoItemVoodooDoll = 70,
    kItemAmmoMax = 81,

    kItemBase = 100,
    
    // items (keys)
    kItemKeyBase = kItemBase,
    kItemKeySkull = kItemKeyBase,
    kItemKeyEye = 101,
    kItemKeyFire = 102,
    kItemKeyDagger = 103,
    kItemKeySpider = 104,
    kItemKeyMoon = 105,
    kItemKeyKey7 = 106,
    kItemKeyMax = 107,

    // items (health)
    kItemHealthDoctorBag = 107,
    kItemHealthMedPouch = 108,
    kItemHealthLifeEssense = 109,
    kItemHealthLifeSeed = 110,
    kItemHealthRedPotion = 111,

    // items (misc)
    kItemFeatherFall = 112,
    kItemShadowCloak = 113, // ltdInvisibility
    kItemDeathMask = 114, // invulnerability
    kItemJumpBoots = 115,
    kItemTwoGuns = 117,
    kItemDivingSuit = 118,
    kItemGasMask = 119,
    kItemCrystalBall = 121,
    kItemReflectShots = 124,
    kItemBeastVision = 125,
    kItemShroomDelirium = 128,

    kItemArmorAsbest = 139,
    kItemArmorBasic = 140,
    kItemArmorBody = 141,
    kItemArmorFire = 142,
    kItemArmorSpirit = 143,
    kItemArmorSuper = 144,

    kItemFlagABase = 145,
    kItemFlagBBase = 146,
    kItemFlagA = 147,
    kItemFlagB = 148,
    kItemMax = 151,

    // dudes
    kDudeBase = 200,
    kDudeCultistTommy = 201,
    kDudeCultistShotgun = 202,
    kDudeZombieAxeNormal = 203,
    kDudeZombieButcher = 204,
    kDudeZombieAxeBuried = 205,
    kDudeGargoyleFlesh = 206,
    kDudeGargoyleStone = 207,
    kDudeGargoyleStatueFlesh = 208,
    kDudeGargoyleStatueStone = 209,
    kDudePhantasm = 210,
    kDudeHellHound = 211,
    kDudeHand = 212,
    kDudeSpiderBrown = 213,
    kDudeSpiderRed = 214,
    kDudeSpiderBlack = 215,
    kDudeSpiderMother = 216,
    kDudeGillBeast = 217,
    kDudeBoneEel = 218,
    kDudeBat = 219,
    kDudeRat = 220,
    kDudePodGreen = 221,
    kDudeTentacleGreen = 222,
    kDudePodFire = 223,
    kDudeTentacleFire = 224,
    kDudePodMother = 225,
    kDudeTentacleMother = 226,
    kDudeCerberusTwoHead = 227,
    kDudeCerberusOneHead = 228,
    kDudeTchernobog = 229,
    kDudeCultistTommyProne = 230,
    kDudePlayer1 = 231,
    kDudePlayer2 = 232,
    kDudePlayer3 = 233,
    kDudePlayer4 = 234,
    kDudePlayer5 = 235,
    kDudePlayer6 = 236,
    kDudePlayer7 = 237,
    kDudePlayer8 = 238,
    kDudeBurningInnocent = 239,
    kDudeBurningCultist = 240,
    kDudeBurningZombieAxe = 241,
    kDudeBurningZombieButcher = 242,
    kDudeCultistReserved = 243, // unused
    kDudeZombieAxeLaying = 244,
    kDudeInnocent = 245,
    kDudeCultistShotgunProne = 246,
    kDudeCultistTesla = 247,
    kDudeCultistTNT = 248,
    kDudeCultistBeast = 249,
    kDudeTinyCaleb = 250,
    kDudeBeast = 251,
    kDudeBurningTinyCaleb = 252,
    kDudeBurningBeast = 253,
    kDudeVanillaMax = 254,
    kDudeMax = 256,
    
    kMissileBase = 300,
    kMissileButcherKnife = kMissileBase,
    kMissileFlareRegular = 301,
    kMissileTeslaAlt = 302,
    kMissileFlareAlt = 303,
    kMissileFlameSpray = 304,
    kMissileFireball = 305,
    kMissileTeslaRegular = 306,
    kMissileEctoSkull = 307,
    kMissileFlameHound = 308,
    kMissilePukeGreen = 309,
    kMissileUnused = 310,
    kMissileArcGargoyle = 311,
    kMissileFireballNapam = 312,
    kMissileFireballCerberus = 313,
    kMissileFireballTchernobog = 314,
    kMissileLifeLeechRegular = 315,
    kMissileLifeLeechAltNormal = 316,
    kMissileLifeLeechAltSmall = 317,
    kMissileMax = 318,

    // things
    kThingBase = 400,
    kThingTNTBarrel = 400,
    kThingArmedProxBomb = 401,
    kThingArmedRemoteBomb = 402,
    kThingCrateFace = 405,
    kThingGlassWindow = 406,
    kThingFluorescent = 407,
    kThingWallCrack = 408,
    kThingSpiderWeb = 410,
    kThingMetalGrate = 411,
    kThingFlammableTree = 412,
    kTrapMachinegun = 413, // not really a thing, should be in traps instead
    kThingFallingRock = 414,
    kThingKickablePail = 415,
    kThingObjectGib = 416,
    kThingObjectExplode = 417,
    kThingArmedTNTStick = 418,
    kThingArmedTNTBundle = 419,
    kThingArmedSpray = 420,
    kThingBone = 421,
    kThingDripWater = 423,
    kThingDripBlood = 424,
    kThingBloodBits = 425,
    kThingBloodChunks = 426,
    kThingZombieHead = 427,
    kThingNapalmBall = 428,
    kThingPodFireBall = 429,
    kThingPodGreenBall = 430,
    kThingDroppedLifeLeech = 431,
    kThingVoodooHead = 432, // unused
    kThingMax = 436,

    // traps
    kTrapFlame = 452,
    kTrapSawCircular = 454,
    kTrapZapSwitchable = 456,
    kTrapExploder = 459,

    // generators
    kGenTrigger = 700,
    kGenDripWater = 701,
    kGenDripBlood = 702,
    kGenMissileFireball = 703,
    kGenMissileEctoSkull = 704,
    kGenDart = 705,
    kGenBubble = 706,
    kGenBubbleMulti = 707,
    
    // sound sprites
    kGenSound = 708,
    kSoundSector = 709,
    kSoundPlayer = 711,
};

// WALL TYPES /////////////////////////////////////////////////
enum {
    kWallBase = 500,
    kWallStack = 501,
    kWallGib = 511,
    kWallMax = 512,
};


// SECTOR TYPES /////////////////////////////////////////////////
enum {
    kSectorBase = 600,
    kSectorZMotion = 600,
    kSectorZMotionSprite = 602,
    kSectorTeleport = 604,
    kSectorPath = 612,
    kSectorRotateStep = 613,
    kSectorSlideMarked = 614,
    kSectorRotateMarked = 615,
    kSectorSlide = 616,
    kSectorRotate = 617,
    kSectorDamage = 618,
    kSectorCounter = 619,
    kSectorMax = 620,
};

// ai state types
enum {
kAiStateOther           = -1,
kAiStateIdle            =  0,
kAiStateGenIdle         =  1,
kAiStateMove            =  2,
kAiStateSearch          =  3,
kAiStateChase           =  4,
kAiStateRecoil          =  5,
kAiStateAttack          =  6,
};

// sprite attributes
#define kHitagAutoAim 0x0008
#define kHitagRespawn 0x0010
#define kHitagFree 0x0020
#define kHitagSmoke 0x0100

// sprite physics attributes
#define kPhysMove 0x0001 // affected by movement physics
#define kPhysGravity 0x0002 // affected by gravity
#define kPhysFalling 0x0004 // currently in z-motion

// sector cstat
#define kSecCParallax 0x01
#define kSecCSloped 0x02
#define kSecCSwapXY 0x04
#define kSecCExpand 0x08
#define kSecCFlipX 0x10
#define kSecCFlipY 0x20
#define kSecCFlipMask 0x34
#define kSecCRelAlign 0x40
#define kSecCFloorShade 0x8000

#define kAng5 28
#define kAng15 85
#define kAng30 170
#define kAng45 256
#define kAng60 341
#define kAng90 512
#define kAng120 682
#define kAng180 1024
#define kAng360 2048


// -------------------------------

// NUKE-TODO:



enum BLOOD_GLOBALFLAGS {
    BLOOD_FORCE_WIDELOADSCREEN = 1<<0,
};

enum searchpathtypes_t {
    SEARCHPATH_REMOVE = 1<<0,
};

extern void G_ExtInit(void);

extern void G_SetupGlobalPsky(void);


#pragma pack(push,1)

struct LOCATION {
    int x, y, z;
    int ang;
};

struct POINT2D {
    int x, y;
};

struct POINT3D {
    int x, y, z;
};

struct VECTOR2D {
    int dx, dy;
};

struct Aim {
    int dx, dy, dz;
};

#pragma pack(pop)

inline int ksgnf(float f)
{
    if (f < 0)
        return -1;
    if (f > 0)
        return 1;
    return 0;
}

inline int IncBy(int a, int b)
{
    a += b;
    int q = a % b;
    a -= q;
    if (q < 0)
        a -= b;
    return a;
}

inline int DecBy(int a, int b)
{
    a--;
    int q = a % b;
    a -= q;
    if (q < 0)
        a -= b;
    return a;
}

inline int ClipLow(int a, int b)
{
    if (a < b)
        return b;
    return a;
}

inline int ClipHigh(int a, int b)
{
    if (a >= b)
        return b;
    return a;
}

inline int ClipRange(int a, int b, int c)
{
    if (a < b)
        return b;
    if (a > c)
        return c;
    return a;
}

inline float ClipRangeF(float a, float b, float c)
{
    if (a < b)
        return b;
    if (a > c)
        return c;
    return a;
}

inline int interpolate(int a, int b, int c)
{
    return a+mulscale16(b-a,c);
}

inline double finterpolate(double a, double b, double c)
{
    return a+fmulscale16(b-a,c);
}

inline int interpolateang(int a, int b, int c)
{
    return a+mulscale16(((b-a+1024)&2047)-1024, c);
}

inline fixed_t interpolateangfix16(fixed_t a, fixed_t b, int c)
{
    return a+mulscale16(((b-a+0x4000000)&0x7ffffff)-0x4000000, c);
}

inline char Chance(int a1)
{
    return wrand() < (a1>>1);
}

inline unsigned int Random(int a1)
{
    return mulscale(wrand(), a1, 15);
}

inline int Random2(int a1)
{
    return mulscale(wrand(), a1, 14)-a1;
}

inline int Random3(int a1)
{
    return mulscale(wrand()+wrand(), a1, 15) - a1;
}

inline unsigned int QRandom(int a1)
{
    return mulscale(qrand(), a1, 15);
}

inline int QRandom2(int a1)
{
    return mulscale(qrand(), a1, 14)-a1;
}

template<class T>
inline void SetBitString(T *pArray, int nIndex)
{
	static_assert(sizeof(T) == 1, "Bit array element too large");
	pArray[nIndex>>3] |= 1<<(nIndex&7);
}

template<class T>
inline void ClearBitString(T *pArray, int nIndex)
{
	static_assert(sizeof(T) == 1, "Bit array element too large");
	pArray[nIndex >> 3] &= ~(1 << (nIndex & 7));
}

template<class T>
inline char TestBitString(T *pArray, int nIndex)
{
	static_assert(sizeof(T) == 1, "Bit array element too large");
	return pArray[nIndex>>3] & (1<<(nIndex&7));
}

// This is to override the namepace prioritization without altering the actual calls.
inline int scale(int a, int b, int c)
{
	return ::scale(a, b, c);
}

inline int scale(int a1, int a2, int a3, int a4, int a5)
{
    return a4 + (a5-a4) * (a1-a2) / (a3-a2);
}

inline int mulscale16r(int a, int b)
{
    int64_t acc = 1<<(16-1);
    acc += ((int64_t)a) * b;
    return (int)(acc>>16);
}

inline int mulscale30r(int a, int b)
{
    int64_t acc = 1<<(30-1);
    acc += ((int64_t)a) * b;
    return (int)(acc>>30);
}

inline int dmulscale30r(int a, int b, int c, int d)
{
    int64_t acc = 1<<(30-1);
    acc += ((int64_t)a) * b;
    acc += ((int64_t)c) * d;
    return (int)(acc>>30);
}

inline int approxDist(int dx, int dy)
{
    dx = klabs(dx);
    dy = klabs(dy);
    if (dx > dy)
        dy = (3*dy)>>3;
    else
        dx = (3*dx)>>3;
    return dx+dy;
}

class Rect {
public:
    int x0, y0, x1, y1;
    Rect(int _x0, int _y0, int _x1, int _y1)
    {
        x0 = _x0; y0 = _y0; x1 = _x1; y1 = _y1;
    }
    bool isValid(void) const
    {
        return x0 < x1 && y0 < y1;
    }
    char isEmpty(void) const
    {
        return !isValid();
    }
    bool operator!(void) const
    {
        return isEmpty();
    }

    Rect & operator&=(Rect &pOther)
    {
        x0 = ClipLow(x0, pOther.x0);
        y0 = ClipLow(y0, pOther.y0);
        x1 = ClipHigh(x1, pOther.x1);
        y1 = ClipHigh(y1, pOther.y1);
        return *this;
    }

    void offset(int dx, int dy)
    {
        x0 += dx;
        y0 += dy;
        x1 += dx;
        y1 += dy;
    }

    int height()
    {
        return y1 - y0;
    }

    int width()
    {
        return x1 - x0;
    }

    bool inside(Rect& other)
    {
        return (x0 <= other.x0 && x1 >= other.x1 && y0 <= other.y0 && y1 >= other.y1);
    }

    bool inside(int x, int y)
    {
        return (x0 <= x && x1 > x && y0 <= y && y1 > y);
    }
};

class BitReader {
public:
    int nBitPos;
    int nSize;
    char *pBuffer;
    BitReader(char *_pBuffer, int _nSize, int _nBitPos) { pBuffer = _pBuffer; nSize = _nSize; nBitPos = _nBitPos; nSize -= nBitPos>>3; }
    BitReader(char *_pBuffer, int _nSize) { pBuffer = _pBuffer; nSize = _nSize; nBitPos = 0; }
    int readBit()
    {
        if (nSize <= 0)
            ThrowError("Buffer overflow");
        int bit = ((*pBuffer)>>nBitPos)&1;
        if (++nBitPos >= 8)
        {
            nBitPos = 0;
            pBuffer++;
            nSize--;
        }
        return bit;
    }
    void skipBits(int nBits)
    {
        nBitPos += nBits;
        pBuffer += nBitPos>>3;
        nSize -= nBitPos>>3;
        nBitPos &= 7;
        if ((nSize == 0 && nBitPos > 0) || nSize < 0)
            ThrowError("Buffer overflow");
    }
    unsigned int readUnsigned(int nBits)
    {
        unsigned int n = 0;
        dassert(nBits <= 32);
        for (int i = 0; i < nBits; i++)
            n += readBit()<<i;
        return n;
    }
    int readSigned(int nBits)
    {
        dassert(nBits <= 32);
        int n = (int)readUnsigned(nBits);
        n <<= 32-nBits;
        n >>= 32-nBits;
        return n;
    }
};

class BitWriter {
public:
    int nBitPos;
    int nSize;
    char *pBuffer;
    BitWriter(char *_pBuffer, int _nSize, int _nBitPos) { pBuffer = _pBuffer; nSize = _nSize; nBitPos = _nBitPos; memset(pBuffer, 0, nSize); nSize -= nBitPos>>3; }
    BitWriter(char *_pBuffer, int _nSize) { pBuffer = _pBuffer; nSize = _nSize; nBitPos = 0; memset(pBuffer, 0, nSize); }
    void writeBit(int bit)
    {
        if (nSize <= 0)
            ThrowError("Buffer overflow");
        *pBuffer |= bit<<nBitPos;
        if (++nBitPos >= 8)
        {
            nBitPos = 0;
            pBuffer++;
            nSize--;
        }
    }
    void skipBits(int nBits)
    {
        nBitPos += nBits;
        pBuffer += nBitPos>>3;
        nSize -= nBitPos>>3;
        nBitPos &= 7;
        if ((nSize == 0 && nBitPos > 0) || nSize < 0)
            ThrowError("Buffer overflow");
    }
    void write(int nValue, int nBits)
    {
        dassert(nBits <= 32);
        for (int i = 0; i < nBits; i++)
            writeBit((nValue>>i)&1);
    }
};

END_BLD_NS
