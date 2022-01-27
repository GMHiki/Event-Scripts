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
 - Developer(s): hiki komori
 - Complete: N/A
 - ScriptName: 'boss_eristrasza' 
 - Comment: N/A
<--------------------------------------------------------------------------->
*/

#include "ScriptPCH.h"
#include "MapManager.h"

enum Spells
{
	//Eristrasza
	SPELL_SHADOW_BREATH = 59126,
	SPELL_TWILIGHT_BLOODBOLT = 71480,
	SPELL_TAIL_SWEEP_SILENCE = 59283,
	SPELL_TAIL_SWEEP_KNOCKBACK = 34267,
	SPELL_SHADOWS_OF_THE_PAST = 67678,
	SPELL_TWILIGHT_TORMENT = 58835,
	SPELL_BERSERK = 26662,
	//Twilight Portal
	SPELL_PORTAL_VISUAL = 51807,
	SPELL_TWILIGHT_PULSE = 46087,
	//Twilight Whelp
	SPELL_ENRAGE = 61369,
	SPELL_VAMPIRIC_CURSE = 70423,
	SPELL_FEAR = 29321,
	//Alexstrasza
	SPELL_SUNBEAM = 62872,
};

enum Events
{
	//Eristrasza
	EVENT_SHADOW_BREATH = 1,
	EVENT_TWILIGHT_BLOODBOLT = 2,
	EVENT_TAIL_SWEEP = 3,
	EVENT_WING_SWEEP = 4,
	EVENT_BERSERK = 5,
	EVENT_WHELP_LIST_CHECK = 6,
	EVENT_ESCAPE = 7,
	EVENT_ASSISTANCE = 8,
	EVENT_TWILIGHT_TORMENT = 9,
	EVENT_SHADOWS_OF_THE_PAST = 10,
	//Twilight Portal
	EVENT_TWILIGHT_PULSE = 11,
	//Twilight Whelp
	EVENT_ENRAGE = 12,
	EVENT_RANDOM_SPELL = 13,
	EVENT_SUMMON_WHELPS = 14,
	EVENT_CANCEL_SUMMON_WHELPS = 15,
	//Alexstrasza
	EVENT_SUNBEAM = 16,
};

enum Creatures
{
	BOSS_ERISTRASZA = 600110,
	NPC_TWILIGHT_WHELP = 600111,
	NPC_TWILIGHT_PORTAL = 600112,
	NPC_ALEXSTRASZA = 600113,
	NPC_SHADOW_TRAP = 39137,
};

enum Actions
{
	ACTION_NO_WHELPS = 1,
	ACTION_AIRBORNE = 2,
};

enum Data
{
	DATA_ERISTRASZA = 1,
};

Position const StartFlightPoint = { 3365.0f, 259.0f, 58.0f };
Position const EndFlightPoint = { 3400.0f, 354.0f, 150.0f };
Position const AlexstraszaSpawn = { 3291.0f, 258.0f, 70.0f };

#define SUMMON_WHELP_COUNT 2

#define EMOTE_STAY "<Eristrasza prefers to stay and finish you off!>"  
#define EMOTE_ESCAPE "<Eristrasza is trying to escape!>"  
#define EMOTE_ESCAPED "<Eristrasza escaped, better luck next time.>"

class EristraszaDespawner : public BasicEvent
{
public:
	explicit EristraszaDespawner(Creature* creature) : creature(creature){ }

	bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
	{
		Trinity::CreatureWorker<EristraszaDespawner> worker(creature, *this);
		creature->VisitNearbyGridObject(600.0f, worker);
		return true;
	}

	void operator()(Creature* creature) const
	{
		switch (creature->GetEntry())
		{
		case BOSS_ERISTRASZA:
			break;
		case NPC_TWILIGHT_PORTAL:
		case NPC_TWILIGHT_WHELP:
		case NPC_SHADOW_TRAP:
		case NPC_ALEXSTRASZA:
			creature->DespawnOrUnsummon(0);
			return;
			creature->Respawn(true);
			break;
		default:
			return;
		}

		uint32 corpseDelay = creature->GetCorpseDelay();
		uint32 respawnDelay = creature->GetRespawnDelay();
		creature->SetCorpseDelay(1);
		creature->SetRespawnDelay(60);

		if (CreatureData const* data = creature->GetCreatureData())
			creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);
		creature->DespawnOrUnsummon();

		creature->SetCorpseDelay(corpseDelay);
		creature->SetRespawnDelay(respawnDelay);
	}

private:
	Creature* creature;
};

class boss_eristrasza : public CreatureScript
{
public:
	boss_eristrasza() : CreatureScript("boss_eristrasza"){ }

	struct boss_eristraszaAI : public ScriptedAI
	{
		boss_eristraszaAI(Creature* creature) : ScriptedAI(creature), summons(me){ }

		void Reset()
		{
			events.Reset();
			isInAir = false;
			DespawnCreatures(NPC_TWILIGHT_WHELP, 2000.0f);
			DespawnCreatures(NPC_ALEXSTRASZA, 2000.0f);
			DespawnCreatures(NPC_SHADOW_TRAP, 2000.0f);
			DespawnCreatures(NPC_TWILIGHT_PORTAL, 2000.0f);
			me->ApplySpellImmune(0, IMMUNITY_ID, 31789, false);
			me->ApplySpellImmune(0, IMMUNITY_ID, 355, false);
			me->ApplySpellImmune(0, IMMUNITY_ID, 1161, false);
			me->SetHealth(me->GetMaxHealth());
		}

		void EnterCombat(Unit* /*victim*/)
		{
			me->Yell("What MERE MORTALS dare disrupt me?", LANG_UNIVERSAL);
			events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(7500, 12500));
			events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(5000, 10000));
			events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(10000, 17500));
			events.ScheduleEvent(EVENT_WING_SWEEP, 45000);
			events.ScheduleEvent(EVENT_ENRAGE, 360000);
		}

		void KilledUnit(Unit* /*who*/)
		{
			me->Yell("You are weak...", LANG_UNIVERSAL);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->Yell("This is... not over yet...", LANG_UNIVERSAL);
		}

		void TwilightWhelpList(Creature* me)
		{
			std::list<Creature*> TwilightWhelpList;
			Trinity::AllCreaturesOfEntryInRange checker(me, NPC_TWILIGHT_WHELP, 2000.0f);
			Trinity::CreatureListSearcher<Trinity::AllCreaturesOfEntryInRange> searcher(me, TwilightWhelpList, checker);
			me->VisitNearbyObject(2000.0f, searcher);

			if (TwilightWhelpList.empty())
				DoAction(ACTION_NO_WHELPS);
		}

		void DespawnCreatures(uint32 entry, float distance, bool discs = false)
		{
			std::list<Creature*> m_pCreatures;
			GetCreatureListWithEntryInGrid(m_pCreatures, me, entry, distance);

			if (m_pCreatures.empty())
				return;

			for (std::list<Creature*>::iterator iter = m_pCreatures.begin(); iter != m_pCreatures.end(); ++iter)
				(*iter)->DespawnOrUnsummon();
		}

		void JustSummoned(Creature* summoned)
		{
			summons.Summon(summoned);
			switch (summoned->GetEntry())
			{
			case NPC_ALEXSTRASZA:
				summoned->Yell("Not any longer.", LANG_UNIVERSAL);
				summoned->AI()->AttackStart(me);
				summoned->GetMotionMaster()->MoveChase(me, 2000.0f);
				summoned->AddThreat(me, 9999999.0f);
				me->AddThreat(summoned, 9999999.0f);
				break;
			}
		}

		void DoAction(int32 action)
		{
			switch (action)
			{
			case ACTION_NO_WHELPS:
				events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(17500, 22500));
				events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(15000, 20000));
				events.CancelEvent(EVENT_ESCAPE);
				events.CancelEvent(EVENT_WHELP_LIST_CHECK);
				me->SetSpeed(MOVE_FLIGHT, 3.0f, true);
				me->TextEmote(EMOTE_STAY, 0, true);
				me->GetMotionMaster()->MovePoint(0, StartFlightPoint);
				isInAir = false;
				break;
			case ACTION_AIRBORNE:
				me->SetSpeed(MOVE_FLIGHT, 0.75f, true);
				me->GetMotionMaster()->MovePoint(0, EndFlightPoint);
				break;
			}
		}

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (me->HealthBelowPctDamaged(25, damage) && !isInAir)
			{
				events.ScheduleEvent(EVENT_ASSISTANCE, 0);
				events.ScheduleEvent(EVENT_TWILIGHT_TORMENT, urand(7500, 15000));
				events.ScheduleEvent(EVENT_SHADOWS_OF_THE_PAST, urand(5000, 7500));
			}
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_SHADOW_BREATH:
					DoCast(me, SPELL_SHADOW_BREATH);
					events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(7500, 12500));
					break;
				case EVENT_TAIL_SWEEP:
					switch (urand(0, 1))
					{
					case 0:
					{
						DoCast(me, SPELL_TAIL_SWEEP_KNOCKBACK);
						break;
					}
					case 1:
					{
						DoCast(me, SPELL_TAIL_SWEEP_SILENCE);
						break;
					}
					}
					events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(5000, 10000));
					break;
				case EVENT_TWILIGHT_BLOODBOLT:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
					{
						DoCast(target, SPELL_TWILIGHT_BLOODBOLT);
					}
					events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(10000, 17500));
					break;
				case EVENT_WING_SWEEP:
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() + 10.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() + 20.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() + 30.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() + 40.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() - 10.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() - 20.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() - 30.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_SHADOW_TRAP, me->GetPositionX() - 40.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_TWILIGHT_PORTAL, me->GetPositionX() + 16.0f, me->GetPositionY() + 16.0f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_TWILIGHT_PORTAL, me->GetPositionX() - 16.0f, me->GetPositionY() - 16.0f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_TWILIGHT_PORTAL, me->GetPositionX() + 16.0f, me->GetPositionY() - 16.0f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					me->SummonCreature(NPC_TWILIGHT_PORTAL, me->GetPositionX() - 16.0f, me->GetPositionY() + 16.0f, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 27500);
					events.ScheduleEvent(EVENT_ESCAPE, 31000);
					events.CancelEvent(EVENT_SHADOW_BREATH);
					events.CancelEvent(EVENT_TAIL_SWEEP);
					events.CancelEvent(EVENT_TWILIGHT_BLOODBOLT);
					isInAir = true;
					DoAction(ACTION_AIRBORNE);
					events.ScheduleEvent(EVENT_WING_SWEEP, 60000);
					break;
				case EVENT_BERSERK:
					DoCast(me, SPELL_BERSERK);
					events.CancelEvent(EVENT_BERSERK);
					break;
				case EVENT_WHELP_LIST_CHECK:
					TwilightWhelpList(me);
					events.ScheduleEvent(EVENT_WHELP_LIST_CHECK, 500);
					break;
				case EVENT_ESCAPE:
					me->TextEmote(EMOTE_ESCAPED, 0, true);
					me->m_Events.AddEvent(new EristraszaDespawner(me), me->m_Events.CalculateTime(0));
					events.CancelEvent(EVENT_ESCAPE);
					break;
				case EVENT_ASSISTANCE:
					events.ScheduleEvent(EVENT_SHADOW_BREATH, urand(7500, 12500));
					events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(5000, 10000));
					events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(10000, 17500));
					events.CancelEvent(EVENT_WING_SWEEP);
					me->ApplySpellImmune(0, IMMUNITY_ID, 31789, true);
					me->ApplySpellImmune(0, IMMUNITY_ID, 1161, true);
					me->ApplySpellImmune(0, IMMUNITY_ID, 355, true);
					me->Yell("Alexstrasza... My powers are far superior to yours... You cannot defeat me!", LANG_UNIVERSAL);
					me->SummonCreature(NPC_ALEXSTRASZA, me->GetPositionX() - 30.0f, me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
					events.CancelEvent(EVENT_ASSISTANCE);
					break;
				case EVENT_TWILIGHT_TORMENT:
					DoCast(me, SPELL_TWILIGHT_TORMENT);
					events.ScheduleEvent(EVENT_TWILIGHT_TORMENT, 20000);
					break;
				case EVENT_SHADOWS_OF_THE_PAST:
					if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
					{
						DoCast(target, SPELL_SHADOWS_OF_THE_PAST);
					}
					events.ScheduleEvent(EVENT_SHADOWS_OF_THE_PAST, 5000);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
		SummonList summons;
		bool isInAir;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_eristraszaAI(creature);
	}
};

class npc_twilight_portal : public CreatureScript
{
public:
	npc_twilight_portal() : CreatureScript("npc_twilight_portal") { }

	struct npc_twilight_portalAI : public ScriptedAI
	{
		npc_twilight_portalAI(Creature* creature) : ScriptedAI(creature), summons(me)
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetReactState(REACT_PASSIVE);
		}

		void Reset()
		{
			events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 0);
			events.ScheduleEvent(EVENT_SUMMON_WHELPS, 5000);
			events.ScheduleEvent(EVENT_CANCEL_SUMMON_WHELPS, 15000);
		}

		void UpdateAI(uint32 diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_TWILIGHT_PULSE:
					me->RemoveAura(SPELL_PORTAL_VISUAL);
					DoCast(me, SPELL_PORTAL_VISUAL);
					DoCast(me, SPELL_TWILIGHT_PULSE);
					events.ScheduleEvent(EVENT_TWILIGHT_PULSE, 2000);
					break;
				case EVENT_SUMMON_WHELPS:
					me->SummonCreature(NPC_TWILIGHT_WHELP, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 0);
					events.ScheduleEvent(EVENT_SUMMON_WHELPS, 2000);
					break;
				case EVENT_CANCEL_SUMMON_WHELPS:
					events.CancelEvent(EVENT_SUMMON_WHELPS);
					events.CancelEvent(EVENT_CANCEL_SUMMON_WHELPS);
					break;
				}
			}
		}

	private:
		EventMap events;
		SummonList summons;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_portalAI(creature);
	}
};

class npc_twilight_whelp : public CreatureScript
{
public:
	npc_twilight_whelp() : CreatureScript("npc_twilight_whelp") { }

	struct npc_twilight_whelpAI : public ScriptedAI
	{
		npc_twilight_whelpAI(Creature* creature) : ScriptedAI(creature){ }

		void Reset()
		{
			events.Reset();
			me->GetMotionMaster()->MoveRandom(10.0f);
		}

		void EnterCombat(Unit* /*victim*/)
		{
			events.ScheduleEvent(EVENT_RANDOM_SPELL, urand(5000, 25000));
			events.ScheduleEvent(EVENT_ENRAGE, 17500);
		}

		void JustDied(Unit* /*killer*/)
		{
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_RANDOM_SPELL:
					switch (urand(0, 1))
					{
					case 0:
					{
						DoCast(me, SPELL_FEAR);
						break;
					}
					case 1:
					{
						if (Unit * target = SelectTarget(SELECT_TARGET_RANDOM))
						{
							DoCast(target, SPELL_VAMPIRIC_CURSE);
						}
						break;
					}
					}
					events.ScheduleEvent(EVENT_RANDOM_SPELL, urand(5000, 25000));
					break;
				case EVENT_ENRAGE:
					DoCast(me, SPELL_ENRAGE);
					events.CancelEvent(EVENT_ENRAGE);
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_whelpAI(creature);
	}
};

class npc_alexstrasza : public CreatureScript
{
public:
	npc_alexstrasza() : CreatureScript("npc_alexstrasza") { }

	struct npc_alexstraszaAI : public ScriptedAI
	{
		npc_alexstraszaAI(Creature* creature) : ScriptedAI(creature){ }

		void Reset()
		{
			events.Reset();
		}

		void EnterCombat(Unit* /*victim*/)
		{
			events.ScheduleEvent(EVENT_SUNBEAM, urand(7500, 15000));
		}

		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_SUNBEAM:
					DoCastVictim(SPELL_SUNBEAM);
					events.ScheduleEvent(EVENT_SUNBEAM, urand(7500, 15000));
					break;
				}
			}

			DoMeleeAttackIfReady();
		}

	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_alexstraszaAI(creature);
	}
};

void AddSC_boss_eristrasza()
{
	new boss_eristrasza();
	new npc_twilight_portal();
	new npc_twilight_whelp();
	new npc_alexstrasza();
}