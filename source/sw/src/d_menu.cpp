//-------------------------------------------------------------------------
/*
Copyright (C) 2016 EDuke32 developers and contributors
Copyright (C) 2019 Christoph Oelckers

This is free software; you can redistribute it and/or
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

#include "ns.h"	// Must come before everything else!
#include "build.h"
#include "g_input.h"

#include "names2.h"
#include "panel.h"
#include "game.h"
#include "tags.h"
#include "sector.h"
#include "sprite.h"
#include "weapon.h"
#include "player.h"
#include "jsector.h"
#include "menus.h"
#include "pal.h"
#include "keydef.h"
#include "d_net.h"

#include "gamecontrol.h"
#include "misc.h"
#include "version.h"
#include "network.h"

#include "misc.h"
#include "menu.h"
#include "raze_sound.h"
#include "sounds.h"
#include "gamestate.h"
#include "raze_music.h"

#include "../../glbackend/glbackend.h"


BEGIN_SW_NS

//----------------------------------------------------------------------------
//
// Implements the native looking menu used for the main menu
// and the episode/skill selection screens, i.e. the parts
// that need to look authentic
//
//----------------------------------------------------------------------------

class SWMainMenu : public DListMenu
{
	void Ticker() override
	{
		// Dynamically enable and disable the save option
		for (unsigned e = 0; e < mDesc->mItems.Size(); ++e)
		{
			auto entry = mDesc->mItems[e];
			if (entry->GetAction(nullptr) == NAME_Savegamemenu)
			{
				entry->mEnabled = gi->CanSave();
			}
		}
	}

	void PreDraw() override
	{
		DrawTexture(twod, tileGetTexture(pic_shadow_warrior), 160, 15, DTA_FullscreenScale, FSMode_Fit320x200,
			DTA_CenterOffsetRel, true, DTA_Color, 0xfff0f0f0, TAG_DONE);
	}
};

static bool DidOrderSound;
static int zero = 0;
class SWOrderMenu : public DImageScrollerMenu
{
public:
	SWOrderMenu()
	{
		if (SW_SHAREWARE && !DidOrderSound)
		{
			DidOrderSound = true;
			int choose_snd = STD_RANDOM_RANGE(1000);
			if (choose_snd > 500)
				PlaySound(DIGI_WANGORDER1, v3df_dontpan, CHAN_BODY, CHANF_UI);
			else 
				PlaySound(DIGI_WANGORDER2, v3df_dontpan, CHAN_BODY, CHANF_UI);
		}
	}
};

//----------------------------------------------------------------------------
//
// Menu related game interface functions
//
//----------------------------------------------------------------------------

void GameInterface::DrawNativeMenuText(int fontnum, int state, double xpos, double ypos, float fontscale, const char* text, int flags)
{
	switch (fontnum)
	{
		case NIT_BigFont:
			if (flags & LMF_Centered) xpos -= BigFont->StringWidth(text) * 0.5;
			DrawText(twod, BigFont, CR_UNDEFINED, xpos, ypos, text, DTA_FullscreenScale, FSMode_Fit320x200, DTA_Color, state == NIT_InactiveState? 0xff505050 : 0xffffffff, TAG_DONE);
			break;
		
		case NIT_SmallFont:
		default:
			MNU_DrawString(short(xpos), short(ypos), text, state == NIT_InactiveState? 20 : 0, 16, (flags & LMF_Centered) ? 0 : -1);
			break;
	}
	if (state == NIT_SelectedState)
	{
		int x = int(xpos), y = int(ypos);
		int scale = 65536;
		short w,h;

		if (text)
		{
			scale /= 2;
			x -= mulscale17(tilesiz[pic_yinyang].x,scale) + 2;
			y += 4;
		}
		else
		{
			scale -= (1<<13);
			x -= ((tilesiz[pic_yinyang].x) / 2) - 3;
			y += 8;
		}
		DrawTexture(twod, tileGetTexture(pic_yinyang, true), x, y, DTA_FullscreenScale, FSMode_Fit320x200,
			DTA_CenterOffset, true, DTA_Color, 0xfff0f0f0, DTA_ScaleX, scale / 65536., DTA_ScaleY, scale / 65536., TAG_DONE);
	}
}

void GameInterface::QuitToTitle()
{
	Mus_Stop();
	gameaction = ga_mainmenu;
}


void GameInterface::MenuOpened()
{
}

void GameInterface::MenuSound(EMenuSounds snd)
{
	switch (snd)
	{
		case CursorSound:
            PlaySound(DIGI_STAR, v3df_dontpan, CHAN_BODY, CHANF_UI);
			break;

		case AdvanceSound:
		case ChooseSound:
			PlaySound(DIGI_SWORDSWOOSH, v3df_dontpan, CHAN_BODY, CHANF_UI);
			break;
			
		case CloseSound:
		case BackSound:
			PlaySound(DIGI_STARCLINK, v3df_dontpan, CHAN_BODY, CHANF_UI);
			break;

		default:
			return;
	}
}

void GameInterface::MenuClosed()
{
}

bool GameInterface::CanSave()
{
    return (gamestate == GS_LEVEL && !CommEnabled && numplayers ==1 && /*!DemoMode &&*/ !TEST(Player[myconnectindex].Flags, PF_DEAD));
}

void GameInterface::StartGame(FNewGameStartup& gs)
{
    PLAYERp pp = Player + screenpeek;
    int handle = 0;
    int zero = 0;
	
	MapRecord* map;
    if (gs.Episode >= 1)
		map = FindMapByLevelNum(5);
    else
		map = FindMapByLevelNum(1);

	if (!map) return;
    CameraTestMode = false;
	StopFX();

    //InitNewGame();

	if (!netgame)
	{
		if (Skill == 0)
			PlaySound(DIGI_TAUNTAI3, v3df_none, CHAN_VOICE, CHANF_UI);
		else if (Skill == 1)
			PlaySound(DIGI_NOFEAR, v3df_none, CHAN_VOICE, CHANF_UI);
		else if (Skill == 2)
			PlaySound(DIGI_WHOWANTSWANG, v3df_none, CHAN_VOICE, CHANF_UI);
		else if (Skill == 3)
			PlaySound(DIGI_NOPAIN, v3df_none, CHAN_VOICE, CHANF_UI);

		while (soundEngine->IsSourcePlayingSomething(SOURCE_None, nullptr, CHAN_VOICE))
		{
			gi->UpdateSounds();
			soundEngine->UpdateSounds(I_GetTime());
			I_GetEvent();
		}
		Net_ClearFifo();
	}
	DeferedStartGame(map, gs.Skill);
}

FSavegameInfo GameInterface::GetSaveSig()
{
	return { SAVESIG_SW, MINSAVEVER_SW, SAVEVER_SW };
}

void GameInterface::DrawMenuCaption(const DVector2& origin, const char* text)
{
	DrawTexture(twod, tileGetTexture(2427), 10, 2, DTA_FullscreenScale, FSMode_Fit320x200Top, DTA_TopLeft, true, DTA_Color, 0xfff0f0f0,  TAG_DONE);
	DrawText(twod, BigFont, CR_UNDEFINED, 160 - BigFont->StringWidth(text) * 0.5, 5, text, DTA_FullscreenScale, FSMode_Fit320x200Top, TAG_DONE);
}



END_SW_NS

//----------------------------------------------------------------------------
//
// Class registration
//
//----------------------------------------------------------------------------


static TMenuClassDescriptor<ShadowWarrior::SWMainMenu> _mm("ShadowWarrior.MainMenu");
static TMenuClassDescriptor<ShadowWarrior::SWOrderMenu> _so("ShadowWarrior.OrderMenu");

void RegisterSWMenus()
{
	menuClasses.Push(&_mm);
	menuClasses.Push(&_so);
}
