#ifndef duke3d_h_
#define duke3d_h_

#include "build.h"

#include "compat.h"

#include "pragmas.h"

#include "polymost.h"
#include "gamecvars.h"
#include "menu/menu.h"
#include "funct.h"
#include "gamecontrol.h"
#include "gamevar.h"
#include "global.h"
#include "names.h"
#include "quotemgr.h"
#include "rts.h"
#include "sounds.h"
#include "soundefs.h"
#include "binaryangle.h"

BEGIN_DUKE_NS

extern FFont* IndexFont;
extern FFont* DigiFont;

struct GameInterface : public ::GameInterface
{
	const char* Name() override { return "Duke"; }
	void app_init() override;
	void clearlocalinputstate() override;
	bool GenerateSavePic() override;
	void PlayHudSound() override;
	GameStats getStats() override;
	void DrawNativeMenuText(int fontnum, int state, double xpos, double ypos, float fontscale, const char* text, int flags) override;
	void MenuOpened() override;
	void MenuSound(EMenuSounds snd) override;
	void MenuClosed() override;
	bool CanSave() override;
	void StartGame(FNewGameStartup& gs) override;
	FSavegameInfo GetSaveSig() override;
	void DrawCenteredTextScreen(const DVector2& origin, const char* text, int position, bool bg) override;
	double SmallFontScale() override { return isRR() ? 0.5 : 1.; }
	void DrawMenuCaption(const DVector2& origin, const char* text) override;
	void SerializeGameState(FSerializer& arc) override;
	void QuitToTitle() override;
	FString GetCoordString() override;
	void ExitFromMenu() override;
	ReservedSpace GetReservedScreenSpace(int viewsize) override;
	void DrawPlayerSprite(const DVector2& origin, bool onteam) override;
	void GetInput(InputPacket* packet, ControlInfo* const hidInput) override;
	void UpdateSounds() override;
	void Startup() override;
	void DrawBackground() override;
	void Render() override;
	void Ticker() override;
	const char* GenericCheat(int player, int cheat) override;
	const char* CheckCheatMode() override;
	void NextLevel(MapRecord* map, int skill) override;
	void NewGame(MapRecord* map, int skill) override;
	void LevelCompleted(MapRecord* map, int skill) override;
	bool DrawAutomapPlayer(int x, int y, int z, int a) override;
	int playerKeyMove() override { return 40; }

};

struct Dispatcher
{
	// global stuff
	void (*ShowLogo)(const CompletionFunc& completion);
	void (*InitFonts)();
	void (*PrintPaused)();

	// sectors_?.cpp
	void (*think)();
	void (*initactorflags)();
	bool (*isadoorwall)(int dapic);
	void (*animatewalls)();
	void (*operaterespawns)(int low);
	void (*operateforcefields)(int s, int low);
	bool (*checkhitswitch)(int snum, int w, int switchtype);
	void (*activatebysector)(int sect, int j);
	void (*checkhitwall)(int spr, int dawallnum, int x, int y, int z, int atwith);
	void (*checkplayerhurt)(struct player_struct* p, int j);
	bool (*checkhitceiling)(int sn);
	void (*checkhitsprite)(int i, int sn);
	void (*checksectors)(int low);

	bool (*ceilingspace)(int sectnum);
	bool (*floorspace)(int sectnum);
	void (*addweapon)(struct player_struct *p, int weapon);
	void (*hitradius)(short i, int  r, int  hp1, int  hp2, int  hp3, int  hp4);
	int  (*movesprite)(short spritenum, int xchange, int ychange, int zchange, unsigned int cliptype);
	void (*lotsofmoney)(spritetype *s, short n);
	void (*lotsofmail)(spritetype *s, short n);
	void (*lotsofpaper)(spritetype *s, short n);
	void (*guts)(spritetype* s, short gtype, short n, short p);
	void (*gutsdir)(spritetype* s, short gtype, short n, short p);
	int  (*ifhitsectors)(int sectnum);
	int  (*ifhitbyweapon)(int sectnum);
	void (*fall)(int g_i, int g_p);
	bool (*spawnweapondebris)(int picnum, int dnum);
	void (*respawnhitag)(spritetype* g_sp);
	void (*checktimetosleep)(int g_i);
	void (*move)(int g_i, int g_p, int g_x);
	int (*spawn)(int j, int pn);
	void (*check_fta_sounds)(int i);

	// player
	void (*incur_damage)(struct player_struct* p);
	void (*shoot)(int, int);
	void (*selectweapon)(int snum, int j);
	int (*doincrements)(struct player_struct* p);
	void (*checkweapons)(struct player_struct* p);
	void (*processinput)(int snum);
	void (*displayweapon)(int snum, double smoothratio);
	void (*displaymasks)(int snum, double smoothratio);

	void (*animatesprites)(int x, int y, int a, int smoothratio);


};

extern Dispatcher fi;

END_DUKE_NS

#endif
