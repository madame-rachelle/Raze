//-------------------------------------------------------------------------
/*
Copyright (C) 1997, 2005 - 3D Realms Entertainment

This file is part of Shadow Warrior version 1.2

Shadow Warrior is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

Original Source: 1997 - Frank Maddin and Jim Norwood
Prepared for public release: 03/28/2005 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#include "ns.h"
#include "game.h"
#include "network.h"
#include "gamecontrol.h"
#include "player.h"
#include "menu.h"


BEGIN_SW_NS

void DoPlayerHorizon(PLAYERp pp, fixed_t const q16horz, double const scaleAdjust);
void DoPlayerTurn(PLAYERp pp, fixed_t const q16avel, double const scaleAdjust);
void DoPlayerTurnVehicle(PLAYERp pp, fixed_t q16avel, int z, int floor_dist);
void DoPlayerTurnTurret(PLAYERp pp, fixed_t q16avel);

static InputPacket loc;
static int32_t turnheldtime;

void
InitNetVars(void)
{
    loc = {};
}

void
InitTimingVars(void)
{
    PlayClock = 0;
    randomseed = 17L;
    MoveSkip8 = 2;
    MoveSkip2 = 0;
    MoveSkip4 = 1;                      // start slightly offset so these
}


enum
{
    TURBOTURNTIME = (120 / 8),
    NORMALTURN = (12 + 6),
    RUNTURN = (28),
    PREAMBLETURN = 3,
    NORMALKEYMOVE = 35,
    MAXFVEL = ((NORMALKEYMOVE * 2) + 10),
    MAXSVEL = ((NORMALKEYMOVE * 2) + 10),
    MAXANGVEL = 100,
    MAXHORIZVEL = 128
};

//---------------------------------------------------------------------------
//
// handles the input bits
//
//---------------------------------------------------------------------------

static void processInputBits(PLAYERp const pp, ControlInfo* const hidInput)
{
    ApplyGlobalInput(loc, hidInput);

    if (!CommEnabled)
    {
        // Go back to the source to set this - the old code here was catastrophically bad.
        // this needs to be fixed properly - as it is this can never be compatible with demo playback.

        if (!(loc.actions & SB_AIMMODE))
            SET(Player[myconnectindex].Flags, PF_MOUSE_AIMING_ON);
        else
            RESET(Player[myconnectindex].Flags, PF_MOUSE_AIMING_ON);

        if (cl_autoaim)
            SET(Player[myconnectindex].Flags, PF_AUTO_AIM);
        else
            RESET(Player[myconnectindex].Flags, PF_AUTO_AIM);
    }

    if (buttonMap.ButtonDown(gamefunc_Toggle_Crouch))
    {
        // this shares a bit with another function so cannot be in the common code.
        loc.actions |= SB_CROUCH_LOCK;
    }
}

//---------------------------------------------------------------------------
//
// handles movement
//
//---------------------------------------------------------------------------

static void processWeapon(PLAYERp const pp)
{
    USERp u = User[pp->PlayerSprite];
    int i;

    if (loc.getNewWeapon() == WeaponSel_Next)
    {
        short next_weapon = u->WeaponNum + 1;
        short start_weapon;

        start_weapon = u->WeaponNum + 1;

        if (u->WeaponNum == WPN_SWORD)
            start_weapon = WPN_STAR;

        if (u->WeaponNum == WPN_FIST)
        {
            next_weapon = 14;
        }
        else
        {
            next_weapon = -1;
            for (i = start_weapon; true; i++)
            {
                if (i >= MAX_WEAPONS_KEYS)
                {
                    next_weapon = 13;
                    break;
                }

                if (TEST(pp->WpnFlags, BIT(i)) && pp->WpnAmmo[i])
                {
                    next_weapon = i;
                    break;
                }
            }
        }

        loc.setNewWeapon(next_weapon + 1);
    }
    else if (loc.getNewWeapon() == WeaponSel_Prev)
    {
        USERp u = User[pp->PlayerSprite];
        short prev_weapon = u->WeaponNum - 1;
        short start_weapon;

        start_weapon = u->WeaponNum - 1;

        if (u->WeaponNum == WPN_SWORD)
        {
            prev_weapon = 13;
        }
        else if (u->WeaponNum == WPN_STAR)
        {
            prev_weapon = 14;
        }
        else
        {
            prev_weapon = -1;
            for (i = start_weapon; true; i--)
            {
                if (i <= -1)
                    i = WPN_HEART;

                if (TEST(pp->WpnFlags, BIT(i)) && pp->WpnAmmo[i])
                {
                    prev_weapon = i;
                    break;
                }
            }
        }
        loc.setNewWeapon(prev_weapon + 1);
    }
    else if (loc.getNewWeapon() == WeaponSel_Alt)
    {
        USERp u = User[pp->PlayerSprite];
        short const which_weapon = u->WeaponNum + 1;
        loc.setNewWeapon(which_weapon);
    }
}

void GameInterface::GetInput(InputPacket *packet, ControlInfo* const hidInput)
{
    if (paused || M_Active())
    {
        loc = {};
        return;
    }

    double const scaleAdjust = InputScale();
    InputPacket input {};
    PLAYERp pp = &Player[myconnectindex];

    processInputBits(pp, hidInput);
    processMovement(&input, &loc, hidInput, true, scaleAdjust, pp->sop_control ? 3 : 1);
    processWeapon(pp);

    if (!cl_syncinput)
    {
        if (TEST(pp->Flags2, PF2_INPUT_CAN_AIM))
        {
            DoPlayerHorizon(pp, input.q16horz, scaleAdjust);
        }

        if (TEST(pp->Flags2, PF2_INPUT_CAN_TURN_GENERAL))
        {
            DoPlayerTurn(pp, input.q16avel, scaleAdjust);
        }

        if (TEST(pp->Flags2, PF2_INPUT_CAN_TURN_VEHICLE))
        {
            DoPlayerTurnVehicle(pp, input.q16avel, pp->posz + Z(10), labs(pp->posz + Z(10) - pp->sop->floor_loz));
        }

        if (TEST(pp->Flags2, PF2_INPUT_CAN_TURN_TURRET))
        {
            DoPlayerTurnTurret(pp, input.q16avel);
        }

        playerProcessHelpers(&pp->q16ang, &pp->angAdjust, &pp->angTarget, &pp->q16horiz, &pp->horizAdjust, &pp->horizTarget, scaleAdjust);
    }

    if (packet)
    {
        auto const ang = FixedToInt(pp->q16ang);

        *packet = loc;

        packet->fvel = mulscale9(loc.fvel, sintable[NORM_ANGLE(ang + 512)]) + mulscale9(loc.svel, sintable[NORM_ANGLE(ang)]);
        packet->svel = mulscale9(loc.fvel, sintable[NORM_ANGLE(ang)]) + mulscale9(loc.svel, sintable[NORM_ANGLE(ang + 1536)]);

        loc = {};
    }
}

//---------------------------------------------------------------------------
//
// This is called from InputState::ClearAllInput and resets all static state being used here.
//
//---------------------------------------------------------------------------

void GameInterface::clearlocalinputstate()
{
    loc = {};
    turnheldtime = 0;
}

END_SW_NS
