//-------------------------------------------------------------------------
/*
Copyright (C) 2010-2019 EDuke32 developers and contributors
Copyright (C) 2019 sirlemonhead, Nuke.YKT
This file is part of PCExhumed.
PCExhumed is free software; you can redistribute it and/or
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
#include "ns.h"
#include "ps_input.h"
#include "exhumed.h"
#include "player.h"
#include "status.h"
#include "view.h"
#include "menu.h"

BEGIN_PS_NS

static int turn;
static int counter;

short nInputStack = 0;

short bStackNode[kMaxPlayers];

short nTypeStack[kMaxPlayers];
PlayerInput sPlayerInput[kMaxPlayers];

int *pStackPtr;

// (nInputStack * 32) - 11;

void PushInput(PlayerInput *pInput, int edx)
{
    if (!bStackNode[edx])
    {
//		memcpy(sInputStack[nInputStack], pInput,
    }
}

int PopInput()
{
    if (!nInputStack)
        return -1;

    nInputStack--;

    // TEMP
    return 0;
}

void InitInput()
{
    memset(nTypeStack, 0, sizeof(nTypeStack));
    nInputStack = 0;
    memset(bStackNode, 0, sizeof(bStackNode));

//	pStackPtr = &sInputStack;
}

void ClearSpaceBar(short nPlayer)
{
    sPlayerInput[nPlayer].actions &= SB_OPEN;
    buttonMap.ClearButton(gamefunc_Open);
}


void BackupInput()
{

}

void SendInput()
{

}


void CheckKeys2()
{
    if (PlayerList[nLocalPlayer].nHealth <= 0)
    {
        SetAirFrame();
    }
}


void GameInterface::GetInput(InputPacket* packet, ControlInfo* const hidInput)
{
    if (paused || M_Active())
    {
        localInput = {};
        return;
    }

    if (packet != nullptr)
    {
        localInput = {};
        ApplyGlobalInput(localInput, hidInput);
        if (PlayerList[nLocalPlayer].nHealth == 0) localInput.actions &= SB_OPEN;
    }

    double const scaleAdjust = InputScale();
    InputPacket input {};

    if (PlayerList[nLocalPlayer].nHealth == 0)
    {
        lPlayerYVel = 0;
        lPlayerXVel = 0;
    }
    else
    {
        processMovement(&input, &localInput, hidInput, true, scaleAdjust);
    }

    if (!cl_syncinput)
    {
        Player* pPlayer = &PlayerList[nLocalPlayer];

        if (!nFreeze)
        {
            applylook(&pPlayer->q16angle, &pPlayer->q16look_ang, &pPlayer->q16rotscrnang, &pPlayer->spin, input.q16avel, &sPlayerInput[nLocalPlayer].actions, scaleAdjust, eyelevel[nLocalPlayer] > -14080);
            sethorizon(&pPlayer->q16horiz, input.q16horz, &sPlayerInput[nLocalPlayer].actions, scaleAdjust);
        }

        playerProcessHelpers(&pPlayer->q16angle, &pPlayer->angAdjust, &pPlayer->angTarget, &pPlayer->q16horiz, &pPlayer->horizAdjust, &pPlayer->horizTarget, scaleAdjust);
        UpdatePlayerSpriteAngle(pPlayer);
    }

    if (packet)
    {
        *packet = localInput;
    }
}

//---------------------------------------------------------------------------
//
// This is called from InputState::ClearAllInput and resets all static state being used here.
//
//---------------------------------------------------------------------------

void GameInterface::clearlocalinputstate()
{
    localInput = {};
    turn = 0;
    counter = 0;
}

END_PS_NS
