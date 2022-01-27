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
 - Developer(s): ??? / hiki komori
 - Complete: N/A
 - ScriptName: 'event_master_system.cpp' 
 - Comment: N/A
<--------------------------------------------------------------------------->
*/
#include "ScriptPCH.h"
#include "Player.h"

class reward_npc : public CreatureScript
{
    public:

        reward_npc()
            : CreatureScript("reward_npc")
        {
        }

        enum GOSSIPMENU
        {
            REQ_REBIRTHPOINTS = 1,
            EVENT_CATEGORIES  = 4,
            REQ_NEXT_EVENTS   = 2,
            EVENT_TELEPORT    = 3,
            MAIN_MENU         = 5
        };

        enum REWARDTYPES
        {
            REWARD_TYPE_ITEM      = 0,
            REWARD_TYPE_HONOR     = 1,
            REWARD_TYPE_TITLE     = 2,
            REWARD_TYPE_XP        = 3,
            REWARD_TYPE_SPELL     = 4,
            REWARD_TYPE_BUFF      = 5,
            REWARD_TYPE_ARENA     = 6
        };

        void RewardScript(Player* player, int scriptid)
        {
            QueryResult result = WorldDatabase.PQuery("SELECT type, param1, param2, param3 FROM rebirth_reward_scripts WHERE id = %d",scriptid);
            if (result)
            {
                Field* field = result->Fetch();

                do
                {
                    int type = field[0].GetInt32();
                    int param1 = field[1].GetInt32();
                    int param2 = field[2].GetInt32();
                    int param3 = field[3].GetInt32();

                    switch (type)
                    {
                        case 0:
                            player->RemoveAura(param1);
                            break;

                        case 1:
                            player->DestroyItemCount(param1, param2, true, false);
                            break;

                        case 2:
                            player->removeSpell(param1,false,false);
                            break;
                    }
                } while (result->NextRow());
            }
        }

        void SendMessageToPlayer(Player* player, Creature* creature, std::string message)
        {
            char str_info[200];
            sprintf(str_info,"%s",message.c_str());
            OnGossipHello(player, creature);
            player->MonsterWhisper(str_info,player->GetGUID(),true);
        }

        bool CheckCondition(int condition, int cParam1, int cParam2, int cParam3, int negation, Player* player)
        {
            int TEAM = 0;
            switch (condition)
            {

                case 0:   //Keine Condition
                    return true;
                    break;

                case 1:    //Spell Condition
                    if (player->HasSpell(cParam1) && negation == 0)  //If he has the spell and negation = 0 then true
                        return true;
                    else if (!player->HasSpell(cParam1) && negation == 0)
                        return false;

                    if (!player->HasSpell(cParam1) && negation == 1)  //If he has the spell and negation = 1 then false
                        return true;
                    else if (player->HasSpell(cParam1) && negation == 1)
                        return false;
                    break;

                case 2:   //Title Condition
                    if (player->HasTitle(sCharTitlesStore.LookupEntry(cParam1)) && negation == 0)
                        return true;
                    else if (!player->HasTitle(sCharTitlesStore.LookupEntry(cParam1)) && negation == 0)
                        return false;

                    if (player->HasTitle(sCharTitlesStore.LookupEntry(cParam1)) && negation == 1)
                        return false;
                    else if (!player->HasTitle(sCharTitlesStore.LookupEntry(cParam1)) && negation == 1)
                        return true;
                    break;

                case 3:   //Item Condition
                    if (player->HasItemCount(cParam1,cParam2,true) && negation == 0)
                        return true;
                    else if (!player->HasItemCount(cParam1,cParam2,true) && negation == 0)
                        return false;

                    if (player->HasItemCount(cParam1,cParam2,true) && negation == 1)
                        return false;
                    else if (!player->HasItemCount(cParam1,cParam2,true) && negation == 1)
                        return true;
                    break;

                case 4:  //Level Condition
                    if (player->getLevel() >= cParam1 && negation == 0)
                        return true;
                    else if (player->getLevel() <= cParam1 && negation == 0)
                        return false;
                    if (player->getLevel() >= cParam1 && negation == 1)
                        return false;
                    else if (player->getLevel() <= cParam1 && negation == 1)
                        return true;
                    break;

                case 5: //Faction Condition
                    TEAM = cParam1 == 1 ? ALLIANCE : HORDE;
                    if (player->GetTeam() == TEAM && negation == 0)
                        return true;
                    else if (player->GetTeam() != TEAM && negation == 0)
                        return false;

                    if (player->GetTeam() != TEAM && negation == 1)
                        return true;
                    else if (player->GetTeam() == TEAM && negation == 1)
                        return false;
                    break;

                case 6: //Race Condition
                    if (player->getRace() == cParam1 && negation == 0)
                        return true;
                    else if (player->getRace() != cParam1 && negation == 0) 
                        return false;

                    if (player->getRace() != cParam1 && negation == 1)
                        return true;
                    else if (player->getRace() == cParam1 && negation == 1)
                        return false;
                    break;

                case 7: //Class Condition
                    if (player->getClass() == cParam1 && negation == 0)
                        return true;
                    else if (player->getClass() != cParam1 && negation == 0)
                        return false;

                    if (player->getClass() != cParam1 && negation == 1)
                        return true;
                    else if (player->getClass() == cParam1 && negation == 1)
                        return false;
                    break;

                case 8:    //Aura Condition
                    if (player->HasAura(cParam1) && negation == 0) 
                        return true;
                    else if (!player->HasAura(cParam1) && negation == 0)
                        return false;

                    if (!player->HasAura(cParam1) && negation == 1)  
                        return true;
                    else if (player->HasAura(cParam1) && negation == 1)
                        return false;
                    break;
            }
            return true;
        }

        void RequestRebirthPoints(Player* player, Creature* creature)
        {

            QueryResult result = LoginDatabase.PQuery("SELECT rebirth_points FROM account WHERE id = %u", player->GetSession()->GetAccountId());

            if (result)
            {
                Field* field = result->Fetch();
                uint32 eventpoints = field[0].GetUInt32();
                char str_info[200];
                sprintf(str_info,"You have %u Rebirth points!", eventpoints);
                player->PlayerTalkClass->ClearMenus();
                player->MonsterWhisper(str_info,player->GetGUID(),true);
                OnGossipHello(player, creature);
            }

            else if (!result)
                SendMessageToPlayer(player, creature,"There has been an error. Please contact an administrator and report ErrorID 100!");

        }

        void RequestNextEvents(Player* player, Creature* creature)
        {

            QueryResult result = LoginDatabase.PQuery("SELECT FROM_UNIXTIME(date), type, reqLevel FROM rebirth_next_event WHERE date > UNIX_TIMESTAMP()");

            if (result)
            {
                do
                {
                     Field* field = result->Fetch();
                     std::string eventType = "";
                     std::string date = field[0].GetCString();
                     uint32 type = field[1].GetUInt32();
                     uint32 reqLevel = field[2].GetUInt32();

                     if (type == 0)
                         eventType = "PvP";
                     else if (type == 1)
                         eventType = "PvE";
                     else if (type == 2)
                         eventType = "Fun";
                     else
                         eventType = "Other";

                     char str_info[200];
                     sprintf(str_info,"Event Info: Date: %s || Type: %s || Recommended Level: %u",date.c_str(),eventType.c_str(),reqLevel);
                     player->PlayerTalkClass->ClearMenus();
                     player->MonsterWhisper(str_info,player->GetGUID(),true);
                     OnGossipHello(player, creature);

                } while (result->NextRow());
            }
            else
                SendMessageToPlayer(player, creature,"There is currently no event planned!");

        }

        void TeleportToEvent(Player* player, Creature* creature)
        {

            QueryResult result = LoginDatabase.PQuery("SELECT x, y, z, map, reqLevel FROM rebirth_next_event WHERE active = 1");

            if (result && isActive())
            {
                Field* field = result->Fetch();
                float x = field[0].GetFloat();
                float y = field[1].GetFloat();
                float z = field[2].GetFloat();
                int map = field[3].GetInt32();
                int reqLevel = field[4].GetInt32();

                if (player->getLevel() >= reqLevel)
                    player->TeleportTo(map, x, y, z, 0.0f, 0);
                else
                    SendMessageToPlayer(player, creature,"Your level is too low to participate in this event!");
            }

            else if (!isActive())
                SendMessageToPlayer(player, creature,"The event teleport has not yet been activated by the event manager!");
        }

        bool isActive()
        {
            QueryResult result = LoginDatabase.PQuery("SELECT * FROM rebirth_next_event WHERE active = 1");
            if (result)
                return true;
            else
                return false;
        }

        bool OnGossipHello(Player* pPlayer, Creature* pCreature)
        {
            if (sWorld->getBoolConfig(CONFIG_REBIRTH_EVENTSYSTEM_ENABLED))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "How many Rebirth Points do I have?", GOSSIP_SENDER_MAIN, 1);

                if (sWorld->getBoolConfig(CONFIG_REBIRTH_EVENTSYSTEM_NEXT_EVENT_INFO_ENABLED))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "When will the next events take place?", GOSSIP_SENDER_MAIN, 2);

                if (sWorld->getBoolConfig(CONFIG_REBIRTH_EVENTSYSTEM_TELEPORT_ENABLED))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport me to the event!", GOSSIP_SENDER_MAIN, 3);

                if (sWorld->getBoolConfig(CONFIG_REBIRTH_EVENTSYSTEM_REWARDS_ENABLED))
                    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I want to buy rewards!", GOSSIP_SENDER_MAIN, 4);

                pPlayer->PlayerTalkClass->SendGossipMenu(120100, pCreature->GetGUID());
            }

            return true;
        }

        bool OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 , uint32 uiAction)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            QueryResult resultEvent;
            QueryResult resultVote;
            

            switch (uiAction)
            {
                case REQ_REBIRTHPOINTS:
                    RequestRebirthPoints(pPlayer, pCreature);
                    break;

                case REQ_NEXT_EVENTS:
                    RequestNextEvents(pPlayer, pCreature);
                    break;

                case EVENT_TELEPORT:
                    TeleportToEvent(pPlayer, pCreature);
                    break;

                case MAIN_MENU:
                    OnGossipHello(pPlayer,pCreature);
                    break;

                case EVENT_CATEGORIES: //EventReward Menu
                    resultEvent = WorldDatabase.PQuery("SELECT id, name FROM rebirth_reward_categorie");

                    if (resultEvent)
                    {

                        do
                        {
                            Field* field = resultEvent->Fetch();
                            int catId = field[0].GetInt32();
                            std::string catName = field[1].GetCString();
                            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, catName.c_str(), GOSSIP_SENDER_MAIN, catId+100);
                        } while (resultEvent->NextRow());

                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ">> Main Menu <<", GOSSIP_SENDER_MAIN, 5);
                        pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());

                    }
                    break;
            }


                /*
                    Hier wird die Liste der Rewards erstellt
                    ToDo: Eine schönere Umsetzung finden
                */
                if (uiAction >= 100 && uiAction < 1000)
                {
                    QueryResult result = WorldDatabase.PQuery("SELECT id, name, type, param1, param2, param3, cost FROM rebirth_rewards WHERE catid = %u", uiAction-100);

                    if (result)
                    {
                        do
                        {
                           Field* field = result->Fetch();
                           int id = field[0].GetInt32();
                           std::string name = field[1].GetCString();
                           int cost = field[6].GetInt32();

                           char str_info[200];
                           sprintf(str_info,"%s (%d RP)",name.c_str(), cost);
                           sLog->outError(str_info);
                           pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, str_info, GOSSIP_SENDER_MAIN, id+1000);
                           
                        } while (result->NextRow());
                        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, ">> Catagory <<", GOSSIP_SENDER_MAIN, 4);
                        pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetGUID());
                    }
                }


                /*
                    From here the rewards are given
                     To Do: Find a nicer implementation for this...
                */
                if (uiAction >= 1000 && uiAction < 10000)
                {
                    QueryResult result = WorldDatabase.PQuery("SELECT type, param1, param2, param3, cost, condition_type, cond_value1, cond_value2, cond_value3, negation, script_id FROM rebirth_rewards WHERE id = %u AND catid != ''", uiAction-1000);
                    QueryResult resulta = LoginDatabase.PQuery("SELECT rebirth_points FROM account WHERE id = %u", pPlayer->GetSession()->GetAccountId());

                    int pRP = 0;
                    if (resulta)
                    {

                        Field* field = resulta->Fetch();
                        int pRP = field[0].GetInt32();
                    
                        if (result)
                        {

                            Field* field = result->Fetch();
                            int type = field[0].GetInt32();
                            int param1 = field[1].GetInt32();
                            int param2 = field[2].GetInt32();
                            int param3 = field[3].GetInt32();
                            int cost = field[4].GetInt32();
                            int condition = field[5].GetInt32();
                            int cond_value1 = field[6].GetInt32();
                            int cond_value2 = field[7].GetInt32();
                            int cond_value3 = field[8].GetInt32();
                            int negation = field[9].GetInt32();
                            int scriptID = field[10].GetInt32();
                            
                            switch (type)
                            {

                               case REWARD_TYPE_ITEM:
                               {
                                   if (cost <= pRP)
                                   {
                                       Item* item = pPlayer->GetItemByEntry(param1);

                                       if (pPlayer->HasItemCount(param1, 1, true))
                                       {
                                           if (item->GetTemplate()->MaxCount <= pPlayer->GetItemCount(param1, true, 0))
                                           {
                                               SendMessageToPlayer(pPlayer, pCreature, "You have already reached the maximum number of this item. You can't recieve any more!");
                                               pPlayer->PlayerTalkClass->ClearMenus();
                                               OnGossipHello(pPlayer, pCreature);
                                               return true;
                                           }
                                       }

                                       if (CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->AddItem(param1, param2);
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                       }
                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");

                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");

                                   break;
                               }

                               case REWARD_TYPE_HONOR:
                               {
                                   if (cost <= pRP)
                                   {
                                       if (pPlayer->GetHonorPoints() + param1 <= 75000 && CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->ModifyHonorPoints(param1);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "Du hast eine Belohnung erhalten!");
                                       }
                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");

                                   break;
                               }

                               case REWARD_TYPE_TITLE:
                               {
                                   if (cost <= pRP)
                                   {
                                       CharTitlesEntry const* title;
                                       title = sCharTitlesStore.LookupEntry(param1);

                                       if (!pPlayer->HasTitle(title) && CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->SetTitle(title);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                       }
                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");

                                   break;
                               }

                               case REWARD_TYPE_XP: 
                               {
                                   if (cost <= pRP)
                                   {
                                       if (pPlayer->getLevel() < 80 && CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->GiveXP(param1,pPlayer,1.0f);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                       }

                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");
                                   break;
                               }

                               case REWARD_TYPE_SPELL:
                               {
                                   if (cost <= pRP)
                                   {
                                       if (!pPlayer->HasSpell(param1) && CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->learnSpell(param1,false);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                       }

                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");
                                   break;
                               }

                               case REWARD_TYPE_BUFF:
                               {
                                   if (cost <= pRP)
                                   {
                                       sLog->outError("CheckCondition(%d, cond_value1, cond_value2, cond_value3, negation, pPlayer)",condition);
                                       if (CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           sLog->outError("true>>");
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->AddAura(param1,pPlayer);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                       }

                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");
                                   break;
                               }

                               case REWARD_TYPE_ARENA:
                               {
                                   if (cost <= pRP)
                                   {
                                       if (pPlayer->GetArenaPoints() + param1 <= 4000 && CheckCondition(condition, cond_value1, cond_value2, cond_value3, negation, pPlayer))
                                       {
                                           RewardScript(pPlayer, scriptID);
                                           pPlayer->ModifyArenaPoints(param1);
                                           LoginDatabase.PExecute("UPDATE account SET rebirth_points = rebirth_points - %d WHERE id = %u", cost, pPlayer->GetSession()->GetAccountId());
                                           SendMessageToPlayer(pPlayer, pCreature, "You got a reward!");
                                       }
                                       else
                                           SendMessageToPlayer(pPlayer, pCreature, "You do not meet the requirements to purchase this reward!");
                                   }

                                   else
                                       SendMessageToPlayer(pPlayer, pCreature, "You don't have enough points to buy this reward!");

                                   break;
                               }
                            }
                        }
                    }
                }

            return true;
       }

};

void AddSC_reward_npc()
{
    new reward_npc();
}