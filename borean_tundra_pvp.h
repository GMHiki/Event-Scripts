/*
        _ _    _                                     _ 
  /\  /(_) | _(_)     /\ /\___  _ __ ___   ___  _ __(_)
 / /_/ / | |/ / |    / //_/ _ \| '_ ` _ \ / _ \| '__| |
/ __  /| |   <| |   / __ \ (_) | | | | | | (_) | |  | |
\/ /_/ |_|_|\_\_|   \/  \/\___/|_| |_| |_|\___/|_|  |_|

	 Even a fool, 
	 	when he holdeth his peace,
		  			is counted wise.

	Game Freedom 2021-2022
<--------------------------------------------------------------------------->
 - Developer(s): frostI / hiki komori
 - Complete: N/A
 - ScriptName: 'borean_tudra_pvp.h' 
 - Comment: N/A
<--------------------------------------------------------------------------->
*/

#ifndef OUTDOOR_PVP_BT_
#define OUTDOOR_PVP_BT_

#include "OutdoorPvP.h"

using namespace std;

const uint32 OutdoorPvPBTZone = 3537;

#define MAX_A_NPCS                         9
#define MAX_H_NPCS                         9
#define WIN_BUFFS                          4
#define LOSE_BUFFS                         2

const uint32 AllianceNpcs[MAX_A_NPCS] = { 25306, 29617, 27393, 25313, 25253, 25759, 25311, 25271, 35131 };
const uint32 HordeNpcs[MAX_H_NPCS] = { 51890, 25242, 25446, 25338, 25243, 25244, 25273, 25439, 25286 };
const uint32 WinBuffs[WIN_BUFFS] = { 39233, 23948, 53899, 62213 };
const uint32 LoseBuffs[LOSE_BUFFS] = { 23948, 40079 };
const uint32 BT_RESOURCES_A = 100;
const uint32 BT_RESOURCES_H = 100;
const uint32 LostResources = 1;

const bool DISABLED_MOUNTS = true;

#define MSG_BT_START_A_WON                 "[Borean Tundra PvP]: Borean Tundra has started! Last winner: Alliance(%u)"
#define MSG_BT_START_H_WON                 "[Borean Tundra PvP]: Borean Tundra has started! Last winner: Horde(%u)"
#define MSG_BT_START_NEUTRAL               "[Borean Tundra PvP]: Borean Tundra has started! Last winner: Neutral(0)"
#define MSG_BT_HANDLE_RESET                "[Borean Tundra PvP]: Borean Tundra : HandleReset()"

#define MSG_DISABLED_MOUNTS                "[Borean Tundra PvP]: Mounting while the zone is active is not allowed!"
#define MSG_BT_PLAYER_LEFT                 "[Borean Tundra PvP]: You're leaving while the zone is active!"

#define MSG_BT_DEFENSE_SHOW_RESOURCES_A    "[Borean Tundra Defense]: Alliance has %u resources!"
#define MSG_BT_DEFENSE_SHOW_RESOURCES_H    "[Borean Tundra Defense]: Horde has %u resources!"
#define MSG_BT_DEFENSE_A_RESOURCES         "[Borean Tundra Defense]: Alliance has %u resources remaining!"
#define MSG_BT_DEFENSE_H_RESOURCES         "[Borean Tundra Defense]: Horde has %u resources remaining!"
#define MSG_BT_DEFENSE_H_WINS              "[Borean Tundra Defense]: Alliance has reached 0 resources!"
#define MSG_BT_DEFENSE_A_WINS              "[Borean Tundra Defense]: Horde has reached 0 resources!"

#define MSG_BT_H_WINS                      "Horde wins!"
#define MSG_BT_A_WINS                      "Alliance wins!"

#define MSG_BT_EXTRA_HONOR_POINTS          "You received %u extra honor points!"
#define MSG_BT_EXTRA_ARENA_POINTS          "You received %u extra arena points!"
#define MSG_BT_EXTRA_BOTH_POINTS           "You received %u extra honor and arena points!"

enum Auras
{
	AURA_FLAG_A = 14268,
	AURA_FLAG_H = 14267
};

enum Sounds
{
	BT_SOUND_ALLIANCE_GOOD = 8173,
	BT_SOUND_HORDE_GOOD = 8213
};

class OutdoorPvPBT : public OutdoorPvP
{
public:
	OutdoorPvPBT();

	bool SetupOutdoorPvP();
	void HandleReset();

	bool Update(uint32 diff);

	void HandlePlayerEnterZone(Player* player, uint32 zone);
	void SetFlagPlayer(Player* player);
	void HandlePlayerLeaveZone(Player* player, uint32 zone);
	void RemoveFlagPlayer(Player* player);

	void HandleKill(Player* player, Unit * killed);
	void Randomizer(Player* player);

	void PlaySounds(bool side);
	void HandleRewards(Player* player, uint32 honororarenapoints, bool honor, bool arena, bool both);
	void HandleBuffs(Player* player, bool loser);

private:
	uint32 m_ally_gathered;
	uint32 m_horde_gathered;
	uint32 m_LastWin;
	uint32 ResetEvent_Timer;
	bool IS_ABLE_TO_SHOW_MESSAGE;
	bool IS_RESOURCE_MESSAGE_A;
	bool IS_RESOURCE_MESSAGE_H;
	bool m_FirstLoad;
	int limit_A;
	int limit_H;
	int limit_resources_message_A;
	int limit_resources_message_H;
};
#endif