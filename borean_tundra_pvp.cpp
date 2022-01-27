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
 - ScriptName: 'borean_tudra_pvp' 
 - Comment: N/A
<--------------------------------------------------------------------------->
*/
#include "ScriptMgr.h"
#include "OutdoorPvPBT.h"
#include "Chat.h"
#include "Language.h"
#include "Player.h"

OutdoorPvPBT::OutdoorPvPBT()
{
    m_TypeId = OUTDOOR_PVP_BT;
    m_ally_gathered = BT_RESOURCES_A;
    m_horde_gathered = BT_RESOURCES_H;
    IS_ABLE_TO_SHOW_MESSAGE = false;
    IS_RESOURCE_MESSAGE_A = false;
    IS_RESOURCE_MESSAGE_H = false;
    m_FirstLoad = false;
    limit_A = 0;
    limit_H = 0;
    m_LastWin = 0;
    limit_resources_message_A = 0;
    limit_resources_message_H = 0;
}

void OutdoorPvPBT::HandleReset()
{
    m_ally_gathered = BT_RESOURCES_A;
    m_horde_gathered = BT_RESOURCES_H;
    IS_ABLE_TO_SHOW_MESSAGE = false;
    IS_RESOURCE_MESSAGE_A = false;
    IS_RESOURCE_MESSAGE_H = false;
    m_FirstLoad = false;
    limit_A = 0;
    limit_H = 0;
    limit_resources_message_A = 0;
    limit_resources_message_H = 0;

    for (int i = 0; i < 2; ++i)
    {
        for (GuidSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
        {
            if (Player* const player = ObjectAccessor::FindPlayer(*itr))
            {
                switch (player->GetTeam())
                {
                case ALLIANCE:
                    player->TeleportTo(OutdoorPvPBTZone, 2269.542236f, 5415.090820f, 1.200347f, 0.633591f);
                    break;
                case HORDE:
                    player->TeleportTo(OutdoorPvPBTZone, 2743.239990f, 6099.940918f, 77.227180f, 0.483234f);
                    break;
                }
            }
        }
    }

    TC_LOG_ERROR("outdoorpvp", MSG_BT_HANDLE_RESET);
}

bool OutdoorPvPBT::SetupOutdoorPvP()
{
    RegisterZone(OutdoorPvPBTZone);
    return true;
}

bool OutdoorPvPBT::Update(uint32 diff)
{
    if (m_ally_gathered == 0 || m_horde_gathered == 0)
    {
        if (ResetEvent_Timer <= diff)
            OutdoorPvPBT::HandleReset();
        else ResetEvent_Timer -= diff;
    }

    if (DISABLED_MOUNTS == true && BT_RESOURCES_A != 0 && BT_RESOURCES_H != 0 && m_players)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (GuidSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
            {
                if (Player* const player = ObjectAccessor::FindPlayer(*itr))
                {
                    if (player->IsMounted())
                    {
                        player->Dismount();
                        player->RemoveAurasByType(SPELL_AURA_MOUNTED);
                        player->GetSession()->GetPlayer()->SendChatMessage(MSG_DISABLED_MOUNTS);
                    }
                    else if (player->GetShapeshiftForm() == FORM_FLIGHT || player->GetShapeshiftForm() == FORM_FLIGHT_EPIC)
                    {
                        player->RemoveAura(33943);
                        player->RemoveAura(40120);
                        player->GetSession()->GetPlayer()->SendChatMessage(MSG_DISABLED_MOUNTS);
                    }
                }
            }
        }
    }

    if (m_FirstLoad == false)
    {
        if (m_LastWin == ALLIANCE)
            TC_LOG_ERROR("outdoorpvp", MSG_BT_START_A_WON, ALLIANCE);
        else if (m_LastWin == HORDE)
            TC_LOG_ERROR("outdoorpvp", MSG_BT_START_H_WON, HORDE);
        else if (m_LastWin == 0)
            TC_LOG_ERROR("outdoorpvp", MSG_BT_START_NEUTRAL);
        m_FirstLoad = true;
    }

    if (m_ally_gathered <= 10 && limit_A == 0)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        IS_RESOURCE_MESSAGE_A = true;
        limit_A = 1;
        PlaySounds(false);
    }
    else if (m_horde_gathered <= 10 && limit_H == 0)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        IS_RESOURCE_MESSAGE_H = true;
        limit_H = 1;
        PlaySounds(true);
    }
    else if (m_ally_gathered <= 0 && limit_A == 1)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        IS_RESOURCE_MESSAGE_A = true;
        limit_A = 2;
        PlaySounds(false);
    }
    else if (m_horde_gathered <= 0 && limit_H == 1)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        IS_RESOURCE_MESSAGE_H = true;
        limit_H = 2;
        PlaySounds(true);
    }
    else if (m_ally_gathered <= 75 && limit_resources_message_A == 0)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_A = 1;
        PlaySounds(false);
    }
    else if (m_horde_gathered <= 75 && limit_resources_message_H == 0)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_H = 1;
        PlaySounds(true);
    }
    else if (m_ally_gathered <= 50 && limit_resources_message_A == 1)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_A = 2;
        PlaySounds(false);
    }
    else if (m_horde_gathered <= 50 && limit_resources_message_H == 1)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_H = 2;
        PlaySounds(true);
    }
    else if (m_ally_gathered <= 25 && limit_resources_message_A == 2)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_A = 3;
        PlaySounds(false);
    }
    else if (m_horde_gathered <= 25 && limit_resources_message_H == 2)
    {
        IS_ABLE_TO_SHOW_MESSAGE = true;
        limit_resources_message_H = 3;
        PlaySounds(true);
    }

    if (IS_ABLE_TO_SHOW_MESSAGE == true)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (GuidSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
            {
                if (Player* const player = ObjectAccessor::FindPlayer(*itr))
                {
                    if (limit_resources_message_A == 1 || limit_resources_message_A == 2 || limit_resources_message_A == 3 && !IS_RESOURCE_MESSAGE_A)
                        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_A_RESOURCES, m_ally_gathered);

                    if (limit_resources_message_H == 1 || limit_resources_message_H == 2 || limit_resources_message_H == 3 && !IS_RESOURCE_MESSAGE_H)
                        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_H_RESOURCES, m_horde_gathered);

                    if (IS_RESOURCE_MESSAGE_A == true)
                    {
                        if (limit_A == 1)
                        {
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_A_RESOURCES, m_ally_gathered);
                            IS_RESOURCE_MESSAGE_A = false;
                        }
                        else if (limit_A == 2)
                        {
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_H_WINS);
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_H_WINS);
                            HandleRewards(player, 250, true, false, false);
                            switch (player->GetTeam())
                            {
                            case ALLIANCE:
                                HandleBuffs(player, true);
                                break;
                            case HORDE:
                                HandleBuffs(player, false);
                                break;
                            }
                            m_LastWin = HORDE;
                            m_ally_gathered = 0;
                            ResetEvent_Timer = 3600000;
                            sWorld->SendGlobalText("[Borean Tundra PvP]: A new round will start in 1 hour!", NULL);
                            IS_RESOURCE_MESSAGE_A = false;
                        }
                    }
                    if (IS_RESOURCE_MESSAGE_H == true)
                    {
                        if (limit_H == 1)
                        {
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_H_RESOURCES, m_horde_gathered);
                            IS_RESOURCE_MESSAGE_H = false;
                        }
                        else if (limit_H == 2)
                        {
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_A_WINS);
                            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_A_WINS);
                            HandleRewards(player, 250, true, false, false);
                            switch (player->GetTeam())
                            {
                            case ALLIANCE:
                                HandleBuffs(player, false);
                                break;
                            case HORDE:
                                HandleBuffs(player, true);
                                break;
                            }
                            m_LastWin = ALLIANCE;
                            m_horde_gathered = 0;
                            ResetEvent_Timer = 3600000;
                            sWorld->SendGlobalText("[Borean Tundra PvP]: A new round will start in 1 hour!", NULL);
                            IS_RESOURCE_MESSAGE_H = false;
                        }
                    }
                }
            }
        }
    }
    IS_ABLE_TO_SHOW_MESSAGE = false;
    return false;
}

void OutdoorPvPBT::HandlePlayerEnterZone(Player* player, uint32 zone)
{
     
        switch (player->GetTeam())
        {
        case ALLIANCE:
            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_SHOW_RESOURCES_A, m_ally_gathered);
            break;
        case HORDE:
            player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_DEFENSE_SHOW_RESOURCES_H, m_horde_gathered);
            break;
        }
        SetFlagPlayer(player);
        OutdoorPvP::HandlePlayerEnterZone(player, zone);
}


void OutdoorPvPBT::SetFlagPlayer(Player* player)
{
    player->SetPvP(true);
    player->SetByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
    switch (player->GetTeam())
    {
    case ALLIANCE:
        player->AddAura(AURA_FLAG_A, player);
        break;
    case HORDE:
        player->AddAura(AURA_FLAG_H, player);
        break;
    }
}

void OutdoorPvPBT::HandlePlayerLeaveZone(Player* player, uint32 zone)
{
     
        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_PLAYER_LEFT);
        RemoveFlagPlayer(player);
        OutdoorPvP::HandlePlayerLeaveZone(player, zone);
    }


void OutdoorPvPBT::RemoveFlagPlayer(Player* player)
{
    player->RemoveByteFlag(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_PVP);
    switch (player->GetTeam())
    {
    case ALLIANCE:
        player->RemoveAura(AURA_FLAG_A);
        break;
    case HORDE:
        player->RemoveAura(AURA_FLAG_H);
        break;
    }
}

void OutdoorPvPBT::HandleKill(Player* player, Unit* killed)
{
    if (m_horde_gathered == 0 || m_ally_gathered == 0)
        return;

    else if (killed->GetTypeId() == TYPEID_PLAYER)
    {
        if (player->GetGUID() != killed->GetGUID())
            return;

        switch (killed->ToPlayer()->GetTeam())
        {
        case ALLIANCE:
            m_ally_gathered -= LostResources;
            Randomizer(player);
            break;
        case HORDE:
            m_horde_gathered -= LostResources;
            Randomizer(player);
            break;
        }
    }
    else
    {
        switch (player->GetTeam())
        {
        case ALLIANCE:
            for (int i = 0; i < MAX_H_NPCS; ++i)
            {
                if (killed->GetEntry() == HordeNpcs[i])
                {
                    m_horde_gathered -= LostResources;
                    Randomizer(player);
                }
            }
            break;
        case HORDE:
            for (int i = 0; i < MAX_A_NPCS; ++i)
            {
                if (killed->GetEntry() == AllianceNpcs[i])
                {
                    m_ally_gathered -= LostResources;
                    Randomizer(player);
                }
            }
            break;
        }
    }
}

void OutdoorPvPBT::Randomizer(Player* player)
{
    int RandIndex = irand(10, 20);
    HandleRewards(player, RandIndex, true, false, false);
}

void OutdoorPvPBT::PlaySounds(bool side)
{
    for (int i = 0; i < 2; ++i)
    {
        for (GuidSet::iterator itr = m_players[i].begin(); itr != m_players[i].end(); ++itr)
        {
            if (Player* const player = ObjectAccessor::FindPlayer(*itr))
            {
                switch (player->GetTeam())
                {
                case ALLIANCE:
                    if (side)
                        player->PlayDirectSound(BT_SOUND_ALLIANCE_GOOD, player);
                    break;
                case HORDE:
                    if (!side)
                        player->PlayDirectSound(BT_SOUND_HORDE_GOOD, player);
                    break;
                }
            }
        }
    }
}

void OutdoorPvPBT::HandleRewards(Player* player, uint32 honorpointsorarena, bool honor, bool arena, bool both)
{
    uint32 m_GetHonorPoints = player->GetHonorPoints();
    uint32 m_GetArenaPoints = player->GetArenaPoints();
    if (honor)
    {

        player->SetHonorPoints(m_GetHonorPoints + honorpointsorarena);
        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_EXTRA_HONOR_POINTS, honorpointsorarena);
    }
    else if (arena)
    {

        player->SetArenaPoints(m_GetArenaPoints + honorpointsorarena);
        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_EXTRA_ARENA_POINTS, honorpointsorarena);
    }
    else if (both)
    {

        
        player->SetHonorPoints(m_GetHonorPoints + honorpointsorarena);
        player->SetArenaPoints(m_GetArenaPoints + honorpointsorarena);
        player->GetSession()->GetPlayer()->SendChatMessage(MSG_BT_EXTRA_BOTH_POINTS, honorpointsorarena);
    }
}

void OutdoorPvPBT::HandleBuffs(Player* player, bool loser)
{
    if (loser)
    {
        for (int i = 0; i < LOSE_BUFFS; ++i)

                player->CastSpell(player, LoseBuffs[i], true);
    }
    else
    {
        for (int i = 0; i < WIN_BUFFS; ++i)

                player->CastSpell(player, WinBuffs[i], true);
    }
}

class OutdoorPvP_borean_tundra : public OutdoorPvPScript
{
public:
    OutdoorPvP_borean_tundra() : OutdoorPvPScript("outdoorpvp_bt") { }

    OutdoorPvP* GetOutdoorPvP() const override
    {
        return new OutdoorPvPBT();
    }
};

void AddSC_outdoorpvp_bt()
{
    new OutdoorPvP_borean_tundra();
}