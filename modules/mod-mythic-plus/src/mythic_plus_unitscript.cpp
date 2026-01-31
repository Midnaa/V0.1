/*
 * Credits: silviu20092
 */

#include "ScriptMgr.h"
#include "GameTime.h"
#include "mythic_plus.h"
#include "mythic_affix.h"

class mythic_plus_unitscript : public UnitScript
{
public:
    mythic_plus_unitscript() : UnitScript("mythic_plus_unitscript", true,
        {
            UNITHOOK_ON_DAMAGE,
            UNITHOOK_ON_UNIT_DEATH,
            UNITHOOK_ON_UNIT_ENTER_EVADE_MODE
        })
    {
    }

    void OnDamage(Unit* attacker, Unit* victim, uint32& /*damage*/) override
    {
        if (!attacker || !victim)
            return;

        if (attacker->GetMap() != victim->GetMap())
            return;

        Creature* creature = victim->ToCreature();
        if (!creature)
            return;

        Map* map = creature->GetMap();
        if (!map || !sMythicPlus->IsMapInMythicPlus(map))
            return;

        // DB-driven boss recognition (no IsDungeonBoss / no IsFinalBoss)
        if (!sMythicPlus->IsRequiredBossForMap(map, creature->GetEntry()))
            return;

        // Only start engagement if a player (or player-controlled unit) initiated it
        if (!attacker->ToPlayer() && !attacker->IsControlledByPlayer())
            return;

        MythicPlus::CreatureData* creatureData = sMythicPlus->GetCreatureData(creature);
        if (!creatureData || creatureData->engageTimer > 0)
            return;

        creatureData->engageTimer = MythicPlus::Utils::GameTimeCount();

        const Map::PlayerList& playerList = map->GetPlayers();
        for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                const std::string& cname = MythicPlus::Utils::GetCreatureName(player, creature);
                std::ostringstream oss;
                oss << "Engaged " << cname << ". Good luck!";
                MythicPlus::AnnounceToPlayer(player, oss.str());
                MythicPlus::BroadcastToPlayer(player, oss.str());
            }
        }
    }

    void OnUnitDeath(Unit* unit, Unit* killer) override
    {
        if (!unit)
            return;

        Creature* creature = unit->ToCreature();
        if (!creature)
            return;

        Map* map = creature->GetMap();
        if (!map || !sMythicPlus->IsMapInMythicPlus(map))
            return;

        // DB-driven boss recognition (no IsDungeonBoss / no IsFinalBoss)
        if (!sMythicPlus->IsRequiredBossForMap(map, creature->GetEntry()))
            return;

        MythicPlus::MapData* mapData = sMythicPlus->GetMapData(map, false);
        ASSERT(mapData);

        MythicPlus::MythicPlusDungeonInfo* savedDungeon = sMythicPlus->GetSavedDungeonInfo(map->GetInstanceId());
        if (!savedDungeon)
            return;

        // ensure creature data exists; do NOT require engageTimer to be set (script kills / no-damage pulls etc.)
        MythicPlus::CreatureData* creatureData = sMythicPlus->GetCreatureData(creature);
        if (!creatureData)
            return;

        uint64 gameTime = GameTime::GetGameTime().count();

        // If we never recorded engagement, at least provide a stable value for diff/snapshot.
        // Use dungeon start time so it's never "0 -> huge" or blocked.
        if (creatureData->engageTimer == 0)
            creatureData->engageTimer = savedDungeon->startTime;

        uint64 diff = gameTime - creatureData->engageTimer;
        std::string downAfterStr = secsToTimeString(diff);

        // reset engage timer for next spawn
        creatureData->engageTimer = 0;

        // Mark this boss as killed for this instance (prevents skipping any configured boss)
        mapData->killedBossEntries.insert(creature->GetEntry());

        // Check completion: ALL configured bosses for this map+difficulty must be killed
        bool allBossesKilled = sMythicPlus->HaveAllRequiredBossesBeenKilled(map);

        bool rewarded = false;

        // Announce and snapshot per player
        const Map::PlayerList& playerList = map->GetPlayers();
        for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
        {
            if (Player* player = itr->GetSource())
            {
                const std::string& cname = MythicPlus::Utils::GetCreatureName(player, creature);

                {
                    std::ostringstream oss;
                    oss << cname << " was bested in " << downAfterStr << ".";
                    MythicPlus::AnnounceToPlayer(player, oss.str());
                    MythicPlus::BroadcastToPlayer(player, oss.str());
                }

                // Progress feedback: X / N bosses down
                {
                    uint32 killed = (uint32)mapData->killedBossEntries.size();
                    uint32 required = sMythicPlus->GetRequiredBossCount(map);

                    std::ostringstream oss;
                    oss << "Bosses defeated: " << killed << "/" << required;
                    MythicPlus::AnnounceToPlayer(player, oss.str());
                }

                // If this kill completed the run, finish & reward exactly once (guarded by mapData->done)
                if (allBossesKilled && !mapData->done)
                {
                    std::ostringstream oss2;
                    oss2 << "All required bosses defeated! Mythic Plus dungeon ended after ";
                    oss2 << secsToTimeString(gameTime - savedDungeon->startTime);
                    MythicPlus::AnnounceToPlayer(player, oss2.str());
                    MythicPlus::BroadcastToPlayer(player, oss2.str());

                    if (mapData->receiveLoot)
                    {
                        rewarded = true;
                        sMythicPlus->Reward(player, mapData->mythicLevel->reward);
                    }

                    mapData->done = true;

                    sMythicPlus->SaveDungeonInfo(
                        map->GetInstanceId(),
                        map->GetId(),
                        mapData->timeLimit,
                        mapData->mythicPlusStartTimer,
                        mapData->mythicLevel->level,
                        mapData->penaltyOnDeath,
                        mapData->deaths,
                        true
                    );
                }

                const MythicLevel* mythicLevel = sMythicPlus->GetMythicLevel(savedDungeon->mythicLevel);
                ASSERT(mythicLevel);

                // "finalBoss" in snapshot now means "run completed on this kill"
                sMythicPlus->AddDungeonSnapshot(
                    map->GetInstanceId(),
                    map->GetId(),
                    map->GetDifficulty(),
                    savedDungeon->startTime,
                    gameTime,
                    (uint32)diff,
                    savedDungeon->timeLimit,
                    player->GetGUID().GetCounter(),
                    player->GetName(),
                    savedDungeon->mythicLevel,
                    creature->GetEntry(),
                    allBossesKilled,
                    rewarded,
                    mapData->penaltyOnDeath,
                    mapData->deaths,
                    mythicLevel->randomAffixCount
                );
            }
        }
    }


    void OnUnitEnterEvadeMode(Unit* unit, uint8 /*evadeReason*/) override
    {
        if (unit && unit->ToCreature())
        {
            Creature* creature = unit->ToCreature();
            MythicPlus::CreatureData* creatureData = sMythicPlus->GetCreatureData(creature, false);
            if (creatureData == nullptr)
                return;

            creatureData->engageTimer = 0;
        }
    }
};

class mythic_plus_unitscript_start_timer : public UnitScript
{
public:
    mythic_plus_unitscript_start_timer() : UnitScript("mythic_plus_unitscript_start_timer", true,
        {
            UNITHOOK_ON_UNIT_DEATH,
        })
    {
    }

    void OnUnitDeath(Unit* unit, Unit* killer) override
    {
        if (unit && killer && unit->GetMap() == killer->GetMap())
        {
            Map* map = unit->GetMap();
            if (!sMythicPlus->CanMapBeMythicPlus(map))
                return;

            Creature* creature = unit->ToCreature();
            if (!creature || !sMythicPlus->CanProcessCreature(creature))
                return;

            // prevent cases like Drak'Tharon Keep where dungeon mobs kill each other at the start
            if (killer->ToCreature() != nullptr && !killer->IsControlledByPlayer())
                return;
        
            MythicPlus::MapData* mapData = sMythicPlus->GetMapData(map);
            // check if a creature was killed by a player and we can be in a M+ dungeon but it was not yet started,
            // in which case we mark this dungeon as non M+
            if (mapData->mythicPlusStartTimer == 0)
            {
                const MythicPlus::MythicPlusDungeonInfo* dsave = sMythicPlus->GetSavedDungeonInfo(map->GetInstanceId());
                if (dsave != nullptr && !dsave->isMythic)
                    return;

                sMythicPlus->SaveDungeonInfo(map->GetInstanceId(), map->GetId(), 0, 0L, 0, 0, 0, false, false);
                MythicPlus::AnnounceToMap(map, "This dungeon is now saved as non Mythic Plus!");
            }
        }
    }
};

class mythic_plus_damage_affix_unitscript : public UnitScript
{
private:
    void HandleScaleDamage(Unit* attacker, uint32& damage, SpellInfo const* spellInfo, bool dotDmg)
    {
        if (attacker && attacker->ToCreature())
        {
            MythicPlus::CreatureData* creatureData = sMythicPlus->GetCreatureData(attacker->ToCreature(), false);
            if (creatureData != nullptr)
                damage *= creatureData->extraDamageMultiplier;

            // spell damage override is added on top of the global multiplier
            if (spellInfo != nullptr)
            {
                const MythicPlus::SpellOverride* spellOverride = sMythicPlus->GetSpellOverride(attacker->GetMap(), spellInfo->Id);
                if (spellOverride != nullptr)
                {
                    float pct = dotDmg ? spellOverride->dotModPct : spellOverride->modPct;
                    if (pct >= 0.0f) // lets allow 0 aswell, will make the spell effect deal no damage
                        damage *= pct;
                }
            }
        }
    }

    void HandleDamageEffect(Unit* attacker, Unit* victim, uint32& damage, SpellInfo const* spellInfo = nullptr, bool dotDmg = false)
    {
        if (attacker && victim && attacker->GetMap() == victim->GetMap())
        {
            if (!sMythicPlus->IsMapInMythicPlus(attacker->GetMap()))
                return;

            Map* map = attacker->GetMap();
            MythicPlus::MapData* mapData = sMythicPlus->GetMapData(map, false);
            ASSERT(mapData);

            HandleScaleDamage(attacker, damage, spellInfo, dotDmg);

            for (auto* affix : mapData->mythicLevel->affixes)
                affix->HandleOnDamageEffect(attacker, victim, damage);
        }
    }
public:
    mythic_plus_damage_affix_unitscript() : UnitScript("mythic_plus_damage_affix_unitscript", true,
        {
            UNITHOOK_MODIFY_MELEE_DAMAGE,
            UNITHOOK_MODIFY_SPELL_DAMAGE_TAKEN,
            UNITHOOK_MODIFY_PERIODIC_DAMAGE_AURAS_TICK
        })
    {
    }

    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override
    {
        HandleDamageEffect(attacker, target, damage);
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellInfo const* spellInfo) override
    {
        // don't do anything for healing
        if (spellInfo->IsPositive() || damage <= 0)
            return;

        uint32 adjustedDamage = (uint32)damage;
        HandleDamageEffect(attacker, target, adjustedDamage, spellInfo, false);
        damage = (int32)adjustedDamage;
    }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage, SpellInfo const* spellInfo) override
    {
        // don't do anything for healing
        if (spellInfo->IsPositive())
            return;

        // skip entangling roots from affix
        if (spellInfo->Id == EntanglingRootsAffix::ENTANGLING_ROOTS_SPELL_ID)
            return;

        HandleDamageEffect(attacker, target, damage, spellInfo, true);
    }
};

class mythic_plus_periodic_affix_unitscript : public UnitScript
{
public:
    mythic_plus_periodic_affix_unitscript() : UnitScript("mythic_plus_periodic_affix_unitscript", true,
        {
            UNITHOOK_ON_UNIT_UPDATE
        })
    {
    }

    void OnUnitUpdate(Unit* unit, uint32 diff) override
    {
        if (unit && sMythicPlus->IsInMythicPlus(unit))
        {
            Map* map = unit->GetMap();
            MythicPlus::MapData* mapData = sMythicPlus->GetMapData(map, false);
            ASSERT(mapData);

            for (auto* affix : mapData->mythicLevel->affixes)
                affix->HandlePeriodicEffect(unit, diff);
        }
    }
};

void AddSC_mythic_plus_unitscript()
{
    new mythic_plus_unitscript();
    new mythic_plus_unitscript_start_timer();
    new mythic_plus_damage_affix_unitscript();
    new mythic_plus_periodic_affix_unitscript();
}
