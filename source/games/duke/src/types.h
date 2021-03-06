#pragma once
#include "names.h"
#include "packet.h"
#include "d_net.h"

BEGIN_DUKE_NS

// all the struct types from JFDuke's duke3d.h

struct STATUSBARTYPE 
{
	short frag[MAXPLAYERS], got_access, last_extra, shield_amount, curr_weapon;
	short ammo_amount[MAX_WEAPONS], holoduke_on;
	unsigned char inven_icon, jetpack_on, heat_on;
	short firstaid_amount, steroids_amount, holoduke_amount, jetpack_amount;
	short heat_amount, scuba_amount, boot_amount;
	short last_weapon, weapon_pos, kickback_pic;
	FixedBitArray<MAX_WEAPONS> gotweapon; // must be the same type as in player_struct
};

struct weaponhit
{
	uint8_t cgg;
	uint8_t spriteextra;	// moved here for easier maintenance. This was originally a hacked in field in the sprite structure called 'filler'.
	short picnum, ang, extra, owner, movflag;
	short tempang, actorstayput, dispicnum;
	short timetosleep;
	int floorz, ceilingz, lastvx, lastvy, bposx, bposy, bposz, aflags;
	int temp_data[6];
};

struct animwalltype
{
	short wallnum, tag;
};

// Todo - put more state in here
struct ActorInfo
{
	uint32_t scriptaddress;
	uint32_t flags;
	int aimoffset;
};

// for now just flags not related to actors, may get more info later.
struct TileInfo
{
	int flags;
	int loadeventscriptptr;
};

struct user_defs
{
	int levelclock;
	unsigned char god, cashman, eog;
	unsigned char clipping;
	char user_name[MAXPLAYERS][32];
	unsigned char showweapons;
	unsigned char user_pals[MAXPLAYERS];

	short from_bonus;
	short camerasprite, last_camsprite;
	short last_level, secretlevel;

	int const_visibility;

	int runkey_mode;

	int shadows;
	int coords, levelstats, m_coop, coop;
	int wchoice[MAXPLAYERS][MAX_WEAPONS];

	int respawn_monsters, respawn_items, respawn_inventory, recstat, monsters_off, brightness;
	int m_respawn_items, m_respawn_monsters, m_respawn_inventory, m_recstat, m_monsters_off;
	int m_ffire, ffire, m_player_skill, multimode;
	int player_skill, marker;
	//MapRecord* nextLevel;

};

struct player_orig
{
	int ox, oy, oz;
	short oa, os;
};

struct player_struct 
{
	// This is basically the version from JFDuke but this first block contains a few changes to make it work with other parts of Raze.
	
	// The sound code wants to read a vector out of this so we need to define one for the main coordinate.
	union
	{
		vec3_t pos;
		struct { int32_t posx, posy, posz; };
	};

	// input handles angle and horizon as fixed16 numbers. We need to account for that as well.
	fixed_t q16ang, q16horiz, q16horizoff, q16rotscrnang, q16look_ang;
	fixed_t oq16ang, oq16horiz, oq16horizoff, oq16rotscrnang, oq16look_ang; // These are only needed with synchronous mouse input.
	fixed_t one_eighty_count;

	// using a bit field for this to save a bit of space.
	FixedBitArray<MAX_WEAPONS> gotweapon;

	// Palette management uses indices into the engine's palette table now.
	unsigned int palette;
	PalEntry pals;

	// this was a global variable originally.
	vec2_t fric;

	// weapon drawer variables and their interpolation counterparts.
	int weapon_sway;
	int oweapon_sway;
	short weapon_pos, kickback_pic, random_club_frame;
	short oweapon_pos, okickback_pic, orandom_club_frame;
	unsigned char hard_landing;
	unsigned char ohard_landing;

	// Store current psectlotag as determined in processinput() for use with scaling angle aiming.
	short psectlotag;

	// From here on it is unaltered from JFDuke with the exception of a few fields that are no longer needed and were removed.
	int exitx, exity, loogiex[64], loogiey[64], numloogs, loogcnt;
	int invdisptime;
	int bobposx, bobposy, oposx, oposy, oposz, pyoff, opyoff;
	int posxv, posyv, poszv, last_pissed_time, truefz, truecz;
	int player_par, visibility;
	int bobcounter;
	int randomflamex, crack_time;

	int aim_mode, auto_aim, ftt;

	short cursectnum, last_extra, subweapon;
	short ammo_amount[MAX_WEAPONS], wackedbyactor, frag, fraggedself;

	short curr_weapon, last_weapon, tipincs, wantweaponfire;
	short holoduke_amount, newowner, hurt_delay, hbomb_hold_delay;
	short jumping_counter, airleft, knee_incs, access_incs;
	short ftq, access_wallnum, access_spritenum;
	short got_access, weapon_ang, firstaid_amount;
	short somethingonplayer, on_crane, i, one_parallax_sectnum;
	short over_shoulder_on, fist_incs;
	short cheat_phase;
	short dummyplayersprite, extra_extra8, quick_kick, last_quick_kick;
	short heat_amount, actorsqu, timebeforeexit, customexitsound;

	short weaprecs[256], weapreccnt;
	unsigned int interface_toggle_flag;

	short dead_flag, show_empty_weapon;	// JBF 20031220: added orotscrnang
	short scuba_amount, jetpack_amount, steroids_amount, shield_amount;
	short holoduke_on, pycount, frag_ps;
	short transporter_hold, last_full_weapon, footprintshade, boot_amount;

	unsigned char on_warping_sector, footprintcount;
	unsigned char hbomb_on, jumping_toggle, rapid_fire_hold, on_ground;
	char name[32];
	unsigned char inven_icon, buttonpalette;

	unsigned char jetpack_on, spritebridge, lastrandomspot;
	unsigned char scuba_on, footprintpal, heat_on;

	unsigned char  holster_weapon;
	unsigned char falling_counter;
	unsigned char refresh_inventory;

	unsigned char toggle_key_flag, knuckle_incs; // ,select_dir;
	unsigned char walking_snd_toggle, palookup;
	bool quick_kick_msg;

	int max_secret_rooms, secret_rooms, max_actors_killed, actors_killed;

	// Redneck Rampage additions. Those which did not have names in the reconstructed source got one from either RedneckGDX or RedNukem.
	// Items were reordered by size.
	int stairs;
	int detonate_count; // at57e
	int noise_x, noise_y, noise_radius; // at286, at28a, at290
	int drink_timer; // at58e
	int eat_timer; // at592
	int SlotWin;
	short recoil;
	short detonate_time; // at57c
	short yehaa_timer;
	short drink_amt, eat, drunkang, eatang;
	uint8_t shotgun_state[2];
	uint8_t donoise; // at28e
	uint8_t keys[5];

	// RRRA. The same as for the RR block applies.
	int drug_aspect;
	int drug_timer;
	int SeaSick;
	short MamaEnd; // raat609
	short MotoSpeed, moto_drink;
	float TiltStatus, oTiltStatus;
	short VBumpNow, VBumpTarget, TurbCount;
	short drug_stat[3]; // raat5f1..5
	uint8_t DrugMode, lotag800kill;
	uint8_t sea_sick_stat; // raat5dd
	uint8_t hurt_delay2, nocheat;
	uint8_t OnMotorcycle, OnBoat, moto_underwater, NotOnWater, MotoOnGround;
	uint8_t moto_do_bump, moto_bump_fast, moto_on_oil, moto_on_mud;
	bool vehicle_turnl, vehicle_turnr, vehicle_backwards;

	int8_t crouch_toggle;

	// input stuff.
	double  horizAdjust, angAdjust;
	fixed_t horizTarget, angTarget;
	InputPacket sync;


	// Access helpers for the widened angle and horizon fields.
	void setlookang(int b) { q16look_ang = IntToFixed(b); }
	void addlookang(int b) { q16look_ang += IntToFixed(b); }
	void addlookang(double b) { q16look_ang += FloatToFixed(b); }
	void setrotscrnang(int b) { q16rotscrnang = IntToFixed(b); }
	void addrotscrnang(int b) { q16rotscrnang += IntToFixed(b); }
	void addrotscrnang(double b) { q16rotscrnang += FloatToFixed(b); }
	int getang() { return FixedToInt(q16ang); }
	int getoang() { return FixedToInt(oq16ang); }
	void setang(int v) { q16ang = IntToFixed(v); }
	void addang(int v) { q16ang = (q16ang + IntToFixed(v)) & 0x7FFFFFF; }
	void setoang(int v) { oq16ang = IntToFixed(v); }
	void addhoriz(int v) { q16horiz += (IntToFixed(v)); }
	void addhorizoff(int v) { q16horiz += (IntToFixed(v)); }
	void addhorizoff(double v) { q16horiz += FloatToFixed(v); }
	void sethoriz(int v) { q16horiz = IntToFixed(v); }
	void sethorizoff(int v) { q16horizoff = IntToFixed(v); }
	int gethoriz() { return FixedToInt(q16horiz); }
	int gethorizof() { return FixedToInt(q16horizoff); }
	int gethorizsum() { return FixedToInt(q16horiz + q16horizoff); }

};


END_DUKE_NS
