﻿/*
* Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Vehicle.h"
#include "ScriptPCH.h"
#include "Common.h"
#include "Chat.h"
#include "Group.h"
#include "Player.h"
#include "DisableMgr.h"
#include "ChannelMgr.h"
#include "Config.h"
#include "Language.h"
#include "World.h"
#include "ScriptedGossip.h"
#include "DatabaseEnv.h"
#include <sstream>

struct Location
{
    uint32 mapId;
    float x;
    float y;
    float z;
    float o;
};

struct TeleData
{
    uint32 action;
    uint32 subAction;
    std::string name;
    Location loc;
    uint32 team;
    bool isEmpty;
    bool isGrub;
};

#define LOCATIONS_COUNT 16

TeleData data[] =
{
    {1203, 0, "Darnassus.",             {1, 9947.52f, 2482.73f, 1316.21f, 0.0f},        ALLIANCE, false, false},
    {1216, 0, "Exodar.",                {530, -3965.69f, -11653.59f, -138.84f, 0.0f},   ALLIANCE, false, false},
    {1206, 0, "Stormwind.",             {0, -8960.14f, 516.266f, 96.3568f, 0.0f},       ALLIANCE, false, false},
    {1224, 0, "Ironforge.",             {0, -4924.07f, -951.95f, 501.55f, 5.40f},       ALLIANCE, false, false},

    {1215, 0, "Orgrimmar.",             {1, 1424.53f, -4411.07f, 73.91f, 0.0f},         HORDE,    false, false},
    {1217, 0, "Silvermoon.",            {530, 9338.74f, -7277.27f, 13.7895f, 0.0f},     HORDE,    false, false},
    {1213, 0, "Undercity.",             {0, 1819.71f, 238.79f, 60.5321f, 0.0f},         HORDE,    false, false},
    {1225, 0, "Thunder Bluff.",         {1, -1273.88f, 81.62f, 128.42f, 0.0f},          HORDE,    false, false},

    {1287, 0, "Shattrath City.",        {530, -1850.2f, 5435.8f, -10.9f, 0.0f},         0,        false, false},
    {1205, 0, "Dalaran.",               {571, 5804.14f, 624.770f, 647.7670f, 1.64f},    0,        false, false},
    {1207, 0, "Nagrand.",               {530, -2504.31f, 6445.08f, 200.43f, 1.64f},     0,        false, false},

    {5550, 0, "[Outdoor PvP] ->",       {0, 0, 0, 0, 0},                                0,        true,  false},
    {1248, 5550, "Ring of Trials.",     {530, -2049.26f, 6662.82f, 13.06f, 0.0f},       0,        false, false},
    {1249, 5550, "Circle of Blood.",    {530, 2839.43f, 5930.16f, 11.20f, 0.0f},        0,        false, false},
    {1250, 5550, "The mauls.",          {1, -3761.24f, 1131.89f, 132.96f, 0.0f},        0,        false, false},
    {1252, 5550, "Gurubashi arena.",    {0, -13312.44f, 61.878f,  22.193f, 0.0f},       0,        false, false},
};

class npc_teleport : public CreatureScript
{
public:
    npc_teleport() : CreatureScript("npc_teleport") { }

    void AddAction(Player *player, uint16 index)
    {
        player->ADD_GOSSIP_ITEM(5, data[index].name.c_str(), GOSSIP_SENDER_MAIN, data[index].action);
    }

    bool OnGossipHello(Player* player, Creature* _Creature) override
    {
        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];

            if (_data.subAction == 0 && (_data.team == player->GetTeam() || !_data.team))
                AddAction(player, i);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE,_Creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* _Creature, uint32 sender, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        int16 actionIndex = -1;

        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];

            if (_data.action == action)
                actionIndex = i;
        }

        if (actionIndex < 0)
            return false;

        if (data[actionIndex].isEmpty)
        {
            SendSubMenu(player, _Creature, action);
            return true;
        }

        player->CLOSE_GOSSIP_MENU();

        TelePlayerByAction(player, actionIndex);

        return true;
    }

    void TelePlayerByAction(Player *player, uint16 actionIndex)
    {
        Location loc = data[actionIndex].loc;
        player->TeleportTo(loc.mapId, loc.x, loc.y, loc.z, loc.z);
    }

    void SendSubMenu(Player *player, Creature *creature, uint16 submenu)
    {
        for (uint16 i = 0; i < LOCATIONS_COUNT; ++i)
        {
            TeleData _data = data[i];

            if (_data.subAction == submenu && (_data.team == player->GetTeam() || !_data.team))
                AddAction(player, i);
        }

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
};

void AddSC_npc_teleport()
{
    new npc_teleport();
}
