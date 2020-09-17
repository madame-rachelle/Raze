#include "names.h"

BEGIN_DUKE_NS
enum
{
	LABEL_HASPARM2=	1,
	LABEL_ISSTRING=	2
};

struct LABELS
{
	const char *name;
	int lId;
	int	flags;
	int maxParm2;
};

LABELS sectorlabels[]=
 {
  {	"wallptr", SECTOR_WALLPTR, 0, 0 },
  { "wallnum", SECTOR_WALLNUM, 0, 0 },
  { "ceilingz", SECTOR_CEILINGZ, 0, 0 },
  { "floorz", SECTOR_FLOORZ, 0, 0 },
  { "ceilingstat", SECTOR_CEILINGSTAT, 0, 0 },
  { "floorstat", SECTOR_FLOORSTAT, 0, 0 },
  { "ceilingpicnum", SECTOR_CEILINGPICNUM, 0, 0 },
  { "ceilingslope", SECTOR_CEILINGSLOPE, 0, 0 },
  { "ceilingshade", SECTOR_CEILINGSHADE, 0, 0 },
  { "ceilingpal", SECTOR_CEILINGPAL, 0, 0 },
  { "ceilingxpanning", SECTOR_CEILINGXPANNING, 0, 0 },
  { "ceilingypanning", SECTOR_CEILINGYPANNING, 0, 0 },
  { "floorpicnum", SECTOR_FLOORPICNUM, 0, 0 },
  { "floorslope", SECTOR_FLOORSLOPE, 0, 0 },
  { "floorshade", SECTOR_FLOORSHADE, 0, 0 },
  { "floorpal", SECTOR_FLOORPAL, 0, 0 },
  { "floorxpanning", SECTOR_FLOORXPANNING, 0, 0 },
  { "floorypanning", SECTOR_FLOORYPANNING, 0, 0 },
  { "visibility", SECTOR_VISIBILITY, 0, 0 },
  { "lotag", SECTOR_LOTAG, 0, 0 },
  { "hitag", SECTOR_HITAG, 0, 0 },
  { "extra", SECTOR_EXTRA, 0, 0 },

  { "", -1, 0, 0  }	// END OF LIST
 };

LABELS walllabels[]=
{
	{ "x", WALL_X, 0, 0 },
	{ "y", WALL_Y, 0, 0 },
	{ "point2", WALL_POINT2, 0, 0 },
	{ "nextwall", WALL_NEXTWALL, 0, 0 },
	{ "nextsector", WALL_NEXTSECTOR, 0, 0 },
	{ "cstat", WALL_CSTAT, 0, 0 },
	{ "picnum", WALL_PICNUM, 0, 0 },
	{ "overpicnum", WALL_OVERPICNUM, 0, 0 },
	{ "shade", WALL_SHADE, 0, 0 },
	{ "pal", WALL_PAL, 0, 0 },
	{ "xrepeat", WALL_XREPEAT, 0, 0 },
	{ "yrepeat", WALL_YREPEAT, 0, 0 },
	{ "xpanning", WALL_XPANNING, 0, 0 },
	{ "ypanning", WALL_YPANNING, 0, 0 },
	{ "lotag", WALL_LOTAG, 0, 0 },
	{ "hitag", WALL_HITAG, 0, 0 },
	{ "extra", WALL_EXTRA, 0, 0 },
	
  { "", -1, 0, 0  }	// END OF LIST

};

LABELS actorlabels[]=
{
	{ "x", ACTOR_X, 0, 0 },
	{ "y", ACTOR_Y, 0, 0 },
	{ "z", ACTOR_Z, 0, 0 },
	{ "cstat", ACTOR_CSTAT, 0, 0 },
	{ "picnum", ACTOR_PICNUM, 0, 0 },
	{ "shade", ACTOR_SHADE, 0, 0 },
	{ "pal", ACTOR_PAL, 0, 0 },
	{ "clipdist", ACTOR_CLIPDIST, 0, 0 },
	{ "detail", ACTOR_DETAIL, 0, 0 },
	{ "xrepeat", ACTOR_XREPEAT, 0, 0 },
	{ "yrepeat", ACTOR_YREPEAT, 0, 0 },
	{ "xoffset", ACTOR_XOFFSET, 0, 0 },
	{ "yoffset", ACTOR_YOFFSET, 0, 0 },
	{ "sectnum", ACTOR_SECTNUM, 0, 0 },
	{ "statnum", ACTOR_STATNUM, 0, 0 },
	{ "ang", ACTOR_ANG, 0, 0 },
	{ "owner", ACTOR_OWNER, 0, 0 },
	{ "xvel", ACTOR_XVEL, 0, 0 },
	{ "yvel", ACTOR_YVEL, 0, 0 },
	{ "zvel", ACTOR_ZVEL, 0, 0 },
	{ "lotag", ACTOR_LOTAG, 0, 0 },
	{ "hitag", ACTOR_HITAG, 0, 0 },
	{ "extra", ACTOR_EXTRA, 0, 0 },

	// hittype labels...
	{ "htcgg", ACTOR_HTCGG, 0, 0 },
	{ "htpicnum", ACTOR_HTPICNUM, 0, 0 },
	{ "htang", ACTOR_HTANG, 0, 0 },
	{ "htextra", ACTOR_HTEXTRA, 0, 0 },
	{ "htowner", ACTOR_HTOWNER, 0, 0 },
	{ "htmovflag", ACTOR_HTMOVFLAG, 0, 0 },
	{ "httempang", ACTOR_HTTEMPANG, 0, 0 },
	{ "htactorstayput", ACTOR_HTACTORSTAYPUT, 0, 0 },
	{ "htdispicnum", ACTOR_HTDISPICNUM, 0, 0 },
	{ "httimetosleep", ACTOR_HTTIMETOSLEEP, 0, 0 },
	{ "htfloorz", ACTOR_HTFLOORZ, 0, 0 },
	{ "htceilingz", ACTOR_HTCEILINGZ, 0, 0 },
	{ "htlastvx", ACTOR_HTLASTVX, 0, 0 },
	{ "htlastvy", ACTOR_HTLASTVY, 0, 0 },
	{ "htbposx", ACTOR_HTBPOSX, 0, 0 },
	{ "htbposy", ACTOR_HTBPOSY, 0, 0 },
	{ "htbposz", ACTOR_HTBPOSZ, 0, 0 },
	{ "htg_t", ACTOR_HTG_T, LABEL_HASPARM2, 5 },
	{ "htg_t[0]", ACTOR_HTG_T0, 0, 0 },
	{ "htg_t[1]", ACTOR_HTG_T1, 0, 0 },
	{ "htg_t[2]", ACTOR_HTG_T2, 0, 0 },
	{ "htg_t[3]", ACTOR_HTG_T3, 0, 0 },
	{ "htg_t[4]", ACTOR_HTG_T4, 0, 0 },
	{ "htg_t[5]", ACTOR_HTG_T5, 0, 0 },

  { "", -1, 0, 0  }	// END OF LIST
	
};


//////////////////////////////////

LABELS playerlabels[]=
{
	{ "zoom", PLAYER_ZOOM, 0, 0 },
	{ "exitx", PLAYER_EXITX, 0, 0 },
	{ "exity", PLAYER_EXITY, 0, 0 },
	{ "loogiex", PLAYER_LOOGIEX, LABEL_HASPARM2, 64 },
	{ "loogiey", PLAYER_LOOGIEY, LABEL_HASPARM2, 64 },
	{ "numloogs", PLAYER_NUMLOOGS, 0, 0 },
	{ "loogcnt", PLAYER_LOOGCNT, 0, 0 },
	{ "posx", PLAYER_POSX, 0, 0 },
	{ "posy", PLAYER_POSY, 0, 0 },
	{ "posz", PLAYER_POSZ, 0, 0 },
	{ "horiz", PLAYER_HORIZ, 0, 0 },
	{ "ohoriz", PLAYER_OHORIZ, 0, 0 },
	{ "ohorizoff", PLAYER_OHORIZOFF, 0, 0 },
	{ "invdisptime", PLAYER_INVDISPTIME, 0, 0 },
	{ "bobposx", PLAYER_BOBPOSX, 0, 0 },
	{ "bobposy", PLAYER_BOBPOSY, 0, 0 },
	{ "oposx", PLAYER_OPOSX, 0, 0 },
	{ "oposy", PLAYER_OPOSY, 0, 0 },
	{ "oposz", PLAYER_OPOSZ, 0, 0 },
	{ "pyoff", PLAYER_PYOFF, 0, 0 },
	{ "opyoff", PLAYER_OPYOFF, 0, 0 },
	{ "posxv", PLAYER_POSXV, 0, 0 },
	{ "posyv", PLAYER_POSYV, 0, 0 },
	{ "poszv", PLAYER_POSZV, 0, 0 },
	{ "last_pissed_time", PLAYER_LAST_PISSED_TIME, 0, 0 },
	{ "truefz", PLAYER_TRUEFZ, 0, 0 },
	{ "truecz", PLAYER_TRUECZ, 0, 0 },
	{ "player_par", PLAYER_PLAYER_PAR, 0, 0 },
	{ "visibility", PLAYER_VISIBILITY, 0, 0 },
	{ "bobcounter", PLAYER_BOBCOUNTER, 0, 0 },
	{ "weapon_sway", PLAYER_WEAPON_SWAY, 0, 0 },
	{ "pals_time", PLAYER_PALS_TIME, 0, 0 },
	{ "randomflamex", PLAYER_RANDOMFLAMEX, 0, 0 },
	{ "crack_time", PLAYER_CRACK_TIME, 0, 0 },
	{ "aim_mode", PLAYER_AIM_MODE, 0, 0 },
	{ "ang", PLAYER_ANG, 0, 0 },
	{ "oang", PLAYER_OANG, 0, 0 },
	{ "angvel", PLAYER_ANGVEL, 0, 0 },
	{ "cursectnum", PLAYER_CURSECTNUM, 0, 0 },
	{ "look_ang", PLAYER_LOOK_ANG, 0, 0 },
	{ "last_extra", PLAYER_LAST_EXTRA, 0, 0 },
	{ "subweapon", PLAYER_SUBWEAPON, 0, 0 },
	{ "ammo_amount", PLAYER_AMMO_AMOUNT, LABEL_HASPARM2, MAX_WEAPONS },
	{ "wackedbyactor", PLAYER_WACKEDBYACTOR, 0, 0 },
	{ "frag", PLAYER_FRAG, 0, 0 },
	{ "fraggedself", PLAYER_FRAGGEDSELF, 0, 0 },
	{ "curr_weapon", PLAYER_CURR_WEAPON, 0, 0 },
	{ "last_weapon", PLAYER_LAST_WEAPON, 0, 0 },
	{ "tipincs", PLAYER_TIPINCS, 0, 0 },
	{ "horizoff", PLAYER_HORIZOFF, 0, 0 },
	{ "wantweaponfire", PLAYER_WANTWEAPONFIRE, 0, 0 },
	{ "holoduke_amount", PLAYER_HOLODUKE_AMOUNT, 0, 0 },
	{ "newowner", PLAYER_NEWOWNER, 0, 0 },
	{ "hurt_delay", PLAYER_HURT_DELAY, 0, 0 },
	{ "hbomb_hold_delay", PLAYER_HBOMB_HOLD_DELAY, 0, 0 },
	{ "jumping_counter", PLAYER_JUMPING_COUNTER, 0, 0 },
	{ "airleft", PLAYER_AIRLEFT, 0, 0 },
	{ "knee_incs", PLAYER_KNEE_INCS, 0, 0 },
	{ "access_incs", PLAYER_ACCESS_INCS, 0, 0 },
	{ "fta", PLAYER_FTA, 0, 0 },
	{ "ftq", PLAYER_FTQ, 0, 0 },
	{ "access_wallnum", PLAYER_ACCESS_WALLNUM, 0, 0 },
	{ "access_spritenum", PLAYER_ACCESS_SPRITENUM, 0, 0 },
	{ "kickback_pic", PLAYER_KICKBACK_PIC, 0, 0 },
	{ "got_access", PLAYER_GOT_ACCESS, 0, 0 },
	{ "weapon_ang", PLAYER_WEAPON_ANG, 0, 0 },
	{ "firstaid_amount", PLAYER_FIRSTAID_AMOUNT, 0, 0 },
	{ "somethingonplayer", PLAYER_SOMETHINGONPLAYER, 0, 0 },
	{ "on_crane", PLAYER_ON_CRANE, 0, 0 },
	{ "i", PLAYER_I, 0, 0 },
	{ "one_parallax_sectnum", PLAYER_ONE_PARALLAX_SECTNUM, 0, 0 },
	{ "over_shoulder_on", PLAYER_OVER_SHOULDER_ON, 0, 0 },
	{ "random_club_frame", PLAYER_RANDOM_CLUB_FRAME, 0, 0 },
	{ "fist_incs", PLAYER_FIST_INCS, 0, 0 },
	{ "one_eighty_count", PLAYER_ONE_EIGHTY_COUNT, 0, 0 },
	{ "cheat_phase", PLAYER_CHEAT_PHASE, 0, 0 },
	{ "dummyplayersprite", PLAYER_DUMMYPLAYERSPRITE, 0, 0 },
	{ "extra_extra8", PLAYER_EXTRA_EXTRA8, 0, 0 },
	{ "quick_kick", PLAYER_QUICK_KICK, 0, 0 },
	{ "heat_amount", PLAYER_HEAT_AMOUNT, 0, 0 },
	{ "actorsqu", PLAYER_ACTORSQU, 0, 0 },
	{ "timebeforeexit", PLAYER_TIMEBEFOREEXIT, 0, 0 },
	{ "customexitsound", PLAYER_CUSTOMEXITSOUND, 0, 0 },
	{ "weaprecs", PLAYER_WEAPRECS, LABEL_HASPARM2, 256 },
	{ "weapreccnt", PLAYER_WEAPRECCNT, 0, 0 },
	{ "interface_toggle_flag", PLAYER_INTERFACE_TOGGLE_FLAG, 0, 0 },
	{ "rotscrnang", PLAYER_ROTSCRNANG, 0, 0 },
	{ "dead_flag", PLAYER_DEAD_FLAG, 0, 0 },
	{ "show_empty_weapon", PLAYER_SHOW_EMPTY_WEAPON, 0, 0 },
	{ "scuba_amount", PLAYER_SCUBA_AMOUNT, 0, 0 },
	{ "jetpack_amount", PLAYER_JETPACK_AMOUNT, 0, 0 },
	{ "steroids_amount", PLAYER_STEROIDS_AMOUNT, 0, 0 },
	{ "shield_amount", PLAYER_SHIELD_AMOUNT, 0, 0 },
	{ "holoduke_on", PLAYER_HOLODUKE_ON, 0, 0 },
	{ "pycount", PLAYER_PYCOUNT, 0, 0 },
	{ "weapon_pos", PLAYER_WEAPON_POS, 0, 0 },
	{ "frag_ps", PLAYER_FRAG_PS, 0, 0 },
	{ "transporter_hold", PLAYER_TRANSPORTER_HOLD, 0, 0 },
	{ "last_full_weapon", PLAYER_LAST_FULL_WEAPON, 0, 0 },
	{ "footprintshade", PLAYER_FOOTPRINTSHADE, 0, 0 },
	{ "boot_amount", PLAYER_BOOT_AMOUNT, 0, 0 },
	{ "scream_voice", PLAYER_SCREAM_VOICE, 0, 0 },
	{ "gm", PLAYER_GM, 0, 0 },
	{ "on_warping_sector", PLAYER_ON_WARPING_SECTOR, 0, 0 },
	{ "footprintcount", PLAYER_FOOTPRINTCOUNT, 0, 0 },
	{ "hbomb_on", PLAYER_HBOMB_ON, 0, 0 },
	{ "jumping_toggle", PLAYER_JUMPING_TOGGLE, 0, 0 },
	{ "rapid_fire_hold", PLAYER_RAPID_FIRE_HOLD, 0, 0 },
	{ "on_ground", PLAYER_ON_GROUND, 0, 0 },
	{ "name", PLAYER_NAME,  LABEL_ISSTRING, 32 },
	{ "inven_icon", PLAYER_INVEN_ICON, 0, 0 },
	{ "buttonpalette", PLAYER_BUTTONPALETTE, 0, 0 },
	{ "jetpack_on", PLAYER_JETPACK_ON, 0, 0 },
	{ "spritebridge", PLAYER_SPRITEBRIDGE, 0, 0 },
	{ "lastrandomspot", PLAYER_LASTRANDOMSPOT, 0, 0 },
	{ "scuba_on", PLAYER_SCUBA_ON, 0, 0 },
	{ "footprintpal", PLAYER_FOOTPRINTPAL, 0, 0 },
	{ "heat_on", PLAYER_HEAT_ON, 0, 0 },
	{ "holster_weapon", PLAYER_HOLSTER_WEAPON, 0, 0 },
	{ "falling_counter", PLAYER_FALLING_COUNTER, 0, 0 },
	{ "gotweapon", PLAYER_GOTWEAPON, LABEL_HASPARM2, MAX_WEAPONS },
	{ "refresh_inventory", PLAYER_REFRESH_INVENTORY, 0, 0 },
	{ "palette", PLAYER_PALETTE, 0, 0 },
	{ "toggle_key_flag", PLAYER_TOGGLE_KEY_FLAG, 0, 0 },
	{ "knuckle_incs", PLAYER_KNUCKLE_INCS, 0, 0 },
	{ "walking_snd_toggle", PLAYER_WALKING_SND_TOGGLE, 0, 0 },
	{ "palookup", PLAYER_PALOOKUP, 0, 0 },
	{ "hard_landing", PLAYER_HARD_LANDING, 0, 0 },
	{ "max_secret_rooms", PLAYER_MAX_SECRET_ROOMS, 0, 0 },
	{ "secret_rooms", PLAYER_SECRET_ROOMS, 0, 0 },
	{ "pals", PLAYER_PALS, LABEL_HASPARM2, 3 },
	{ "max_actors_killed", PLAYER_MAX_ACTORS_KILLED, 0, 0 },
	{ "actors_killed", PLAYER_ACTORS_KILLED, 0, 0 },
	{ "return_to_center", PLAYER_RETURN_TO_CENTER, 0, 0 },
	
  { "", -1, 0, 0  }	// END OF LIST
	
};

///////////////////////////////////////

LABELS userdefslabels[]=
{
	{ "god", USERDEFS_GOD, 0, 0 },
	{ "warp_on", USERDEFS_WARP_ON, 0, 0 },
	{ "cashman", USERDEFS_CASHMAN, 0, 0 },
	{ "eog", USERDEFS_EOG, 0, 0 },
	{ "showallmap", USERDEFS_SHOWALLMAP, 0, 0 },
	{ "show_help", USERDEFS_SHOW_HELP, 0, 0 },
	{ "scrollmode", USERDEFS_SCROLLMODE, 0, 0 },
	{ "clipping", USERDEFS_CLIPPING, 0, 0 },
	{ "user_name", USERDEFS_USER_NAME, LABEL_HASPARM2, MAXPLAYERS },
	{ "ridecule", USERDEFS_RIDECULE, LABEL_HASPARM2 | LABEL_ISSTRING, 10 },
	{ "savegame", USERDEFS_SAVEGAME, LABEL_HASPARM2 | LABEL_ISSTRING, 10 },
	{ "pwlockout", USERDEFS_PWLOCKOUT, LABEL_ISSTRING, 128 },
	{ "rtsname;", USERDEFS_RTSNAME,  LABEL_ISSTRING, 128 },
 { "overhead_on", USERDEFS_OVERHEAD_ON, 0, 0 },
 { "last_overhead", USERDEFS_LAST_OVERHEAD, 0, 0 },
 { "showweapons", USERDEFS_SHOWWEAPONS, 0, 0 },

 { "pause_on", USERDEFS_PAUSE_ON, 0, 0 },
 { "from_bonus", USERDEFS_FROM_BONUS, 0, 0 },
 { "camerasprite", USERDEFS_CAMERASPRITE, 0, 0 },
 { "last_camsprite", USERDEFS_LAST_CAMSPRITE, 0, 0 },
 { "last_level", USERDEFS_LAST_LEVEL, 0, 0 },
 { "secretlevel", USERDEFS_SECRETLEVEL, 0, 0 },

 { "const_visibility", USERDEFS_CONST_VISIBILITY, 0, 0 },
 { "uw_framerate", USERDEFS_UW_FRAMERATE, 0, 0 },
 { "camera_time", USERDEFS_CAMERA_TIME, 0, 0 },
 { "folfvel", USERDEFS_FOLFVEL, 0, 0 },
 { "folavel", USERDEFS_FOLAVEL, 0, 0 },
 { "folx", USERDEFS_FOLX, 0, 0 },
 { "foly", USERDEFS_FOLY, 0, 0 },
 { "fola", USERDEFS_FOLA, 0, 0 },
 { "reccnt", USERDEFS_RECCNT, 0, 0 },

 { "entered_name", USERDEFS_ENTERED_NAME, 0, 0 },
 { "screen_tilting", USERDEFS_SCREEN_TILTING, 0, 0 },
 { "shadows", USERDEFS_SHADOWS, 0, 0 },
 { "fta_on", USERDEFS_FTA_ON, 0, 0 },
 { "executions", USERDEFS_EXECUTIONS, 0, 0 },
 { "auto_run", USERDEFS_AUTO_RUN, 0, 0 },
 { "coords", USERDEFS_COORDS, 0, 0 },
 { "tickrate", USERDEFS_TICKRATE, 0, 0 },
 { "m_coop", USERDEFS_M_COOP, 0, 0 },
 { "coop", USERDEFS_COOP, 0, 0 },
 { "screen_size", USERDEFS_SCREEN_SIZE, 0, 0 },
 { "lockout", USERDEFS_LOCKOUT, 0, 0 },
 { "crosshair", USERDEFS_CROSSHAIR, 0, 0 },
 { "wchoice[MAXPLAYERS][MAX_WEAPONS]", USERDEFS_WCHOICE, 0, 0 },
 { "playerai", USERDEFS_PLAYERAI, 0, 0 },
 { "respawn_monsters", USERDEFS_RESPAWN_MONSTERS, 0, 0 },
 { "respawn_items", USERDEFS_RESPAWN_ITEMS, 0, 0 },
 { "respawn_inventory", USERDEFS_RESPAWN_INVENTORY, 0, 0 },
 { "recstat", USERDEFS_RECSTAT, 0, 0 },
 { "monsters_off", USERDEFS_MONSTERS_OFF, 0, 0 },
 { "brightness", USERDEFS_BRIGHTNESS, 0, 0 },
 { "m_respawn_items", USERDEFS_M_RESPAWN_ITEMS, 0, 0 },
 { "m_respawn_monsters", USERDEFS_M_RESPAWN_MONSTERS, 0, 0 },
 { "m_respawn_inventory", USERDEFS_M_RESPAWN_INVENTORY, 0, 0 },
 { "m_recstat", USERDEFS_M_RECSTAT, 0, 0 },
 { "m_monsters_off", USERDEFS_M_MONSTERS_OFF, 0, 0 },
 { "detail", USERDEFS_DETAIL, 0, 0 },
 { "m_ffire", USERDEFS_M_FFIRE, 0, 0 },
 { "ffire", USERDEFS_FFIRE, 0, 0 },
 { "m_player_skill", USERDEFS_M_PLAYER_SKILL, 0, 0 },
 { "m_level_number", USERDEFS_M_LEVEL_NUMBER, 0, 0 },
 { "m_volume_number", USERDEFS_M_VOLUME_NUMBER, 0, 0 },
 { "multimode", USERDEFS_MULTIMODE, 0, 0 },
 { "player_skill", USERDEFS_PLAYER_SKILL, 0, 0 },
 { "level_number", USERDEFS_LEVEL_NUMBER, 0, 0 },
 { "volume_number", USERDEFS_VOLUME_NUMBER, 0, 0 },
 { "m_marker", USERDEFS_M_MARKER, 0, 0 },
 { "marker", USERDEFS_MARKER, 0, 0 },
 { "mouseflip", USERDEFS_MOUSEFLIP, 0, 0 },

  { "", -1, 0, 0  }	// END OF LIST
	
};
END_DUKE_NS
