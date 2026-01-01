#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "Chat.h"
#include "ReputationMgr.h"

/* ========================================================= */
/* ===================== VENDOR LISTS ====================== */
/* ========================================================= */

enum VendorLists
{
    // Pre-Raid BiS
    VENDOR_PRERAID_WARRIOR = 7000001,
    VENDOR_PRERAID_PALADIN = 7000002,
    VENDOR_PRERAID_HUNTER = 7000003,
    VENDOR_PRERAID_ROGUE = 7000004,
    VENDOR_PRERAID_PRIEST = 7000005,
    VENDOR_PRERAID_SHAMAN = 7000006,
    VENDOR_PRERAID_MAGE = 7000007,
    VENDOR_PRERAID_WARLOCK = 7000008,
    VENDOR_PRERAID_DRUID = 7000009,

    // Thorium Brotherhood
    VENDOR_TB_FRIENDLY = 7100001,
    VENDOR_TB_HONORED = 7100002,
    VENDOR_TB_REVERED = 7100003,
    VENDOR_TB_EXALTED = 7100004
};

/* ========================================================= */
/* ===================== GOSSIP TEXT ======================= */
/* ========================================================= */

// Pre-Raid
#define GOSSIP_PRERAID_WARRIOR  "Warrior Pre-Raid BiS"
#define GOSSIP_PRERAID_PALADIN  "Paladin Pre-Raid BiS"
#define GOSSIP_PRERAID_HUNTER   "Hunter Pre-Raid BiS"
#define GOSSIP_PRERAID_ROGUE    "Rogue Pre-Raid BiS"
#define GOSSIP_PRERAID_PRIEST   "Priest Pre-Raid BiS"
#define GOSSIP_PRERAID_SHAMAN   "Shaman Pre-Raid BiS"
#define GOSSIP_PRERAID_MAGE     "Mage Pre-Raid BiS"
#define GOSSIP_PRERAID_WARLOCK  "Warlock Pre-Raid BiS"
#define GOSSIP_PRERAID_DRUID    "Druid Pre-Raid BiS"

// Thorium Brotherhood
#define GOSSIP_TB_FRIENDLY "|TInterface\Icons\achievement_reputation_01:30:30|t Friendly Rewards"
#define GOSSIP_TB_HONORED  "|TInterface\Icons\achievement_reputation_02:30:30|t Honored Rewards"
#define GOSSIP_TB_REVERED  "|TInterface\Icons\achievement_reputation_03:30:30|t Revered Rewards"
#define GOSSIP_TB_EXALTED  "|TInterface\Icons\achievement_reputation_08:30:30|t Exalted Rewards"

// Gray requirement text
#define REQ_FRIENDLY "|cff808080Requires Friendly|r"
#define REQ_HONORED  "|cff808080Requires Honored|r"
#define REQ_REVERED  "|cff808080Requires Revered|r"
#define REQ_EXALTED  "|cff808080Requires Exalted|r"

/* ========================================================= */
/* ================= PRE-RAID BIS HUB ====================== */
/* ========================================================= */

class npc_preraid_bis_hub : public CreatureScript
{
public:
    npc_preraid_bis_hub() : CreatureScript("npc_preraid_bis_hub") {}

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_WARRIOR, 0, 1);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_PALADIN, 0, 2);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_HUNTER, 0, 3);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_ROGUE, 0, 4);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_PRIEST, 0, 5);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_SHAMAN, 0, 6);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_MAGE, 0, 7);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_WARLOCK, 0, 8);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_PRERAID_DRUID, 0, 9);

        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action) override
    {
        CloseGossipMenuFor(player);

        switch (action)
        {
        case 1: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_WARRIOR); break;
        case 2: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_PALADIN); break;
        case 3: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_HUNTER); break;
        case 4: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_ROGUE); break;
        case 5: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_PRIEST); break;
        case 6: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_SHAMAN); break;
        case 7: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_MAGE); break;
        case 8: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_WARLOCK); break;
        case 9: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_PRERAID_DRUID); break;
        }
        return true;
    }
};

/* ========================================================= */
/* ============== THORIUM BROTHERHOOD HUB ================== */
/* ========================================================= */

class npc_thorium_brotherhood : public CreatureScript
{
public:
    npc_thorium_brotherhood() : CreatureScript("npc_thorium_brotherhood") {}

    static constexpr uint32 FACTION_TB = 59;

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ReputationRank rank = player->GetReputationRank(FACTION_TB);

        if (rank >= REP_FRIENDLY)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TB_FRIENDLY, 0, 1);
        else
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, REQ_FRIENDLY, 0, 0);

        if (rank >= REP_HONORED)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TB_HONORED, 0, 2);
        else
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, REQ_HONORED, 0, 0);

        if (rank >= REP_REVERED)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TB_REVERED, 0, 3);
        else
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, REQ_REVERED, 0, 0);

        if (rank == REP_EXALTED)
            AddGossipItemFor(player, GOSSIP_ICON_VENDOR, GOSSIP_TB_EXALTED, 0, 4);
        else
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, REQ_EXALTED, 0, 0);

        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action) override
    {
        CloseGossipMenuFor(player);

        switch (action)
        {
        case 1: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_TB_FRIENDLY); break;
        case 2: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_TB_HONORED);  break;
        case 3: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_TB_REVERED);  break;
        case 4: player->GetSession()->SendListInventory(creature->GetGUID(), VENDOR_TB_EXALTED);  break;
        }
        return true;
    }
};

/* ========================================================= */
/* ====================== LOADER =========================== */
/* ========================================================= */

void AddMultiVendor_scripts()
{
    new npc_preraid_bis_hub();
    new npc_thorium_brotherhood();
}
