#include "DatabaseEnv.h"  // QueryResult, Field, ResultSet
#include "Define.h"       // uint32
#include "World.h"        // getMSTime(), GetMSTimeDiffToNow()
#include "WorldScript.h"  // WorldScript
#include "Log.h"          // sLog
#include <unordered_map>  // std::unordered_map

typedef std::unordered_map<uint32, float> Regulation; // creature_entry, percentage

class SpellRegulator
{
public:
	static SpellRegulator* instance()
	{
		static SpellRegulator instance;
		return &instance;
	}

	void Regulate(uint32& damage, uint32 spellId, uint32 entry = 0)
	{
		if (RegulatorContainer.find(spellId) == RegulatorContainer.end())
			return;

		bool found = false;
		float val = 100.0f;
		
		// Find specific regulation for this creature entry
		for (auto& regulation : RegulatorContainer[spellId])
		{
			if (regulation.first == entry)
			{
				val = regulation.second;
				found = true;
				break;
			}
			
		}
		// If it doesn't exist, find a general regulation for this spell
		if (!found && RegulatorContainer[spellId].find(0) != RegulatorContainer[spellId].end())
		{
			val = RegulatorContainer[spellId][0];
		}

		if (!val || val == 100.0f)
			return;

		damage = (damage / 100.0f) * val;
	}

	void LoadFromDB()
	{
		RegulatorContainer.clear();
		uint32 msTime = getMSTime();
		QueryResult result = WorldDatabase.Query("SELECT `spell_id`, `creature_entry`, `percentage` FROM `spell_regulator`");

		if (!result)
			return;

		uint32 count = 0;
		do
		{
			Field* fields = result->Fetch();

            RegulatorContainer[fields[0].Get<uint32>()][fields[1].Get<uint32>()] = fields[2].Get<float>();
			++count;
		} while (result->NextRow());
		LOG_INFO("server.loading", "Loaded %u regulated spells in %u ms", count, GetMSTimeDiffToNow(msTime));
	}

private:
	std::unordered_map<uint32, Regulation> RegulatorContainer; // spellId, regulations
};

#define sSpellRegulator SpellRegulator::instance()

class RegulatorLoader : public WorldScript
{
public:
    RegulatorLoader() : WorldScript("SpellRegulatorLoader") {}

    void OnStartup()
    {
        sSpellRegulator->LoadFromDB();
    }
};
