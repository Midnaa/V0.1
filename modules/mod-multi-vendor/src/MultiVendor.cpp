#include "ScriptMgr.h"
#include "ScriptedGossip.h"
#include "Player.h"
#include "Chat.h"
#include "ReputationMgr.h"
#include "Creature.h"
#include "Item.h"
#include "Log.h"

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
    VENDOR_TB_EXALTED = 7100004,

    // Glyphs
    VENDOR_GLYPH_DRUID = 500027,
    VENDOR_GLYPH_HUNTER = 500046,
    VENDOR_GLYPH_MAGE = 500057,
    VENDOR_GLYPH_PALADIN = 500068,
    VENDOR_GLYPH_ROGUE = 500095,
    VENDOR_GLYPH_SHAMAN = 500097,
    VENDOR_GLYPH_WARLOCK = 500110,
    VENDOR_GLYPH_WARRIOR = 500111,
    VENDOR_GLYPH_PRIEST = 500113,

    // Enchants
    VENDOR_ENCH_ARMOR_I = 500137,
    VENDOR_ENCH_ARMOR_II = 500138,
    VENDOR_ENCH_WEAPON = 500136,

    // Consumables
    VENDOR_CONS_BANDAGES = 500012,
    VENDOR_CONS_ELIXIRS = 500029,
    VENDOR_CONS_FLASKS = 500038,
    VENDOR_CONS_SCROLLS = 500096,
    VENDOR_CONS_POTIONS = 500135


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
#define GOSSIP_TB_FRIENDLY "|TInterface\\Icons\\achievement_reputation_01:30:30|t Friendly Rewards"
#define GOSSIP_TB_HONORED  "|TInterface\\Icons\\achievement_reputation_02:30:30|t Honored Rewards"
#define GOSSIP_TB_REVERED  "|TInterface\\Icons\\achievement_reputation_03:30:30|t Revered Rewards"
#define GOSSIP_TB_EXALTED  "|TInterface\\Icons\\achievement_reputation_08:30:30|t Exalted Rewards"


// Gray requirement text
#define REQ_FRIENDLY "|cff808080Requires Friendly|r"
#define REQ_HONORED  "|cff808080Requires Honored|r"
#define REQ_REVERED  "|cff808080Requires Revered|r"
#define REQ_EXALTED  "|cff808080Requires Exalted|r"

#include <vector>
#include <string>

// Class colors
static constexpr char const* C_END = "|r";
static constexpr char const* C_WARRIOR = "|cffC79C6E";
static constexpr char const* C_PALADIN = "|cffF58CBA";
static constexpr char const* C_HUNTER = "|cffABD473";
static constexpr char const* C_ROGUE = "|cffFFF569";
static constexpr char const* C_PRIEST = "|cffFFFFFF";
static constexpr char const* C_SHAMAN = "|cff0070DE";
static constexpr char const* C_MAGE = "|cff69CCF0";
static constexpr char const* C_WARLOCK = "|cff9482C9";
static constexpr char const* C_DRUID = "|cffFF7D0A";

using BisList = std::vector<uint32>; // itemId only


// TODO: fill later
static BisList BIS_WARRIOR_ARMS = { 13404, 15411, 12927, 13340, 11726, 12936, 15063, 13959, 15062, 12555, 17713, 13098, 11815, 13965, 12651, 12939, 12940 };
                                    
static BisList BIS_WARRIOR_FURY = { 13404, 15411, 12927, 13340, 11726, 12936, 15063, 13959, 15062, 12555, 17713, 13098, 11815, 13965, 12651, 12939, 12940 };
static BisList BIS_WARRIOR_PROT = {12952, 13091, 14552, 18413, 14624, 12550, 13072, 14620, 11927, 14621, 11669, 22331, 11810, 10779, 15806, 12602, 12651 };

static BisList BIS_PALADIN_HOLY = {12633, 13141, 18720, 13386, 13346, 13969, 11923, 11928, 12554, 18702, 13170, 13954, 16058, 18103, 11819, 12930 };
static BisList BIS_PALADIN_PROT = {12952, 13091, 14552, 18413, 14624, 12550, 15806, 12602, 13072, 14620, 11927, 14621, 11669, 22331, 11810, 10779 };
static BisList BIS_PALADIN_RET = {13404, 15411, 12927, 13340, 11726, 12936, 12784, 13957, 13959, 14554, 14616, 13098, 12548, 13965, 11815 };

static BisList BIS_HUNTER_BM = { 13404, 15411, 12927, 13340, 11726, 13211, 12940, 12939, 2099, 15063, 14502, 15062, 13967, 13098, 17713, 13965, 11815};
static BisList BIS_HUNTER_MM = { 13404, 15411, 12927, 13340, 11726, 13211, 12940, 12939, 2099, 15063, 14502, 15062, 13967, 13098, 17713, 13965, 11815 };
static BisList BIS_HUNTER_SV = { 13404, 15411, 12927, 13340, 11726, 13211, 12940, 12939, 2099, 15063, 14502, 15062, 13967, 13098, 17713, 13965, 11815 };

static BisList BIS_ROGUE_ASSA = { 13404, 15411, 12927, 13340, 14637, 13120, 12783, 14555, 12651, 15063, 13252, 15062, 12553, 13098, 17713, 13965, 11815};
static BisList BIS_ROGUE_COMBAT = { 13404, 15411, 12927, 13340, 14637, 13120, 12940, 12939, 12651, 15063, 13252, 15062, 12553, 13098, 17713, 13965, 11815};
static BisList BIS_ROGUE_SUB = { 13404, 15411, 12927, 13340, 14637, 13120, 12783, 14555, 12651, 15063, 13252, 15062, 12553, 13098, 17713, 13965, 11815 };

static BisList BIS_PRIEST_DISC = {13102, 13141, 13013, 13386, 14154, 13107, 11923, 11928, 16997, 12554, 14143, 11841, 11822, 16058, 13178, 11819, 12930 };
static BisList BIS_PRIEST_HOLY = { 13102, 13141, 13013, 13386, 14154, 13107, 11923, 11928, 16997, 12554, 14143, 11841, 11822, 16058, 13178, 11819, 12930 };
static BisList BIS_PRIEST_SHADOW = {10504, 18691, 14112, 13386, 14136, 13107, 13349, 10796, 13396, 13253, 13956, 13170, 18735, 12545, 13001, 12930, 11819 };

static BisList BIS_SHAMAN_ELE = {10504, 13141, 13013, 15421, 11924, 13107, 13253, 13956, 13170, 13954, 12545, 13001, 13968, 12930, 13964, 11904 };
static BisList BIS_SHAMAN_ENH = {12587, 15411, 12927, 13340, 11726, 13211, 12784, 13957, 22232, 15062, 14616, 13098, 17713, 13965, 11815, 871, 811 };
static BisList BIS_SHAMAN_RESTO = {13216, 13141, 15061, 13386, 13346, 13969, 11923, 11928, 12554, 14553, 13170, 13954, 16058, 13178, 12930, 11819 };

static BisList BIS_MAGE_ARCANE = { 10504, 12103, 11782, 13386, 14152, 13107, 13964, 10796, 13938, 13253, 11662, 13170, 11822, 942, 942, 12930, 13968};
static BisList BIS_MAGE_FIRE = { 10504, 12103, 11782, 13386, 14152, 13107, 13964, 10796, 13938, 13253, 11662, 13170, 11822, 942, 942, 12930, 13968 };
static BisList BIS_MAGE_FROST = { 10504, 12103, 11782, 13386, 14152, 13107, 13964, 10796, 13938, 13253, 11662, 13170, 11822, 942, 942, 12930, 13968 };

static BisList BIS_WARLOCK_AFF = { 10504, 18691, 14112, 13386, 14153, 13107, 13253, 13956, 13170, 18735, 12545, 13001, 12930, 13968 };
static BisList BIS_WARLOCK_DEMO = { 10504, 18691, 14112, 13386, 14153, 13107, 13253, 13956, 13170, 18735, 12545, 13001, 12930, 13968 };
static BisList BIS_WARLOCK_DESTRO = { 10504, 18691, 14112, 13386, 14153, 13107, 13253, 13956, 13170, 18735, 12545, 13001, 12930, 13968 };

static BisList BIS_DRUID_BALANCE = { 10504, 12103, 13013, 13386, 22409, 11766, 13964, 10796, 13258, 11662, 13170, 11822, 12545, 13001, 12930, 13968};
static BisList BIS_DRUID_FERAL = {14539, 13091, 15058, 18413, 15064, 18700, 943, 13258, 14502, 11821, 14641, 15855, 11669, 13966, 11811, 8345, 15411, 12927, 13340, 14637, 16710, 13167, 15063, 13252, 15062, 12553, 17713, 2264, 13965, 11815 };
static BisList BIS_DRUID_RESTO = {13102, 13141, 15061, 13386, 13346, 13208, 11923, 11928, 12554, 14553, 15060, 13954, 13178, 16058, 12930, 11819 };

enum BisActions : uint32
{
    // Main menu
    ACT_CLASS_WARRIOR = 1001,
    ACT_CLASS_PALADIN = 1002,
    ACT_CLASS_HUNTER = 1003,
    ACT_CLASS_ROGUE = 1004,
    ACT_CLASS_PRIEST = 1005,
    ACT_CLASS_SHAMAN = 1006,
    ACT_CLASS_MAGE = 1007,
    ACT_CLASS_WARLOCK = 1008,
    ACT_CLASS_DRUID = 1009,

    // Warrior specs
    ACT_WARRIOR_ARMS = 1101,
    ACT_WARRIOR_FURY = 1102,
    ACT_WARRIOR_PROT = 1103,

    // Paladin specs
    ACT_PALADIN_HOLY = 1201,
    ACT_PALADIN_PROT = 1202,
    ACT_PALADIN_RET = 1203,

    // Hunter specs
    ACT_HUNTER_BM = 1301,
    ACT_HUNTER_MM = 1302,
    ACT_HUNTER_SV = 1303,

    // Rogue specs
    ACT_ROGUE_ASSA = 1401,
    ACT_ROGUE_COMBAT = 1402,
    ACT_ROGUE_SUB = 1403,

    // Priest specs
    ACT_PRIEST_DISC = 1501,
    ACT_PRIEST_HOLY = 1502,
    ACT_PRIEST_SHADOW = 1503,

    // Shaman specs
    ACT_SHAMAN_ELE = 1601,
    ACT_SHAMAN_ENH = 1602,
    ACT_SHAMAN_RESTO = 1603,

    // Mage specs
    ACT_MAGE_ARCANE = 1701,
    ACT_MAGE_FIRE = 1702,
    ACT_MAGE_FROST = 1703,

    // Warlock specs
    ACT_WARLOCK_AFF = 1801,
    ACT_WARLOCK_DEMO = 1802,
    ACT_WARLOCK_DESTRO = 1803,

    // Druid specs
    ACT_DRUID_BAL = 1901,
    ACT_DRUID_FERAL = 1902,
    ACT_DRUID_RESTO = 1903,

    ACT_BACK = 99999
};

static bool EnsureClass(Player* player, uint8 requiredClass)
{
    if (!player)
        return false;

    if (player->getClass() != requiredClass)
    {
        ChatHandler(player->GetSession()).SendSysMessage("This menu is not for your class.");
        return false;
    }
    return true;
}

static void GiveItemSafe(Player* player, uint32 itemId, uint32 count)
{
    if (!player || !itemId || !count)
        return;

    ItemPosCountVec dest;
    uint32 noSpaceForCount = 0;

    if (player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount) == EQUIP_ERR_OK)
    {
        if (Item* item = player->StoreNewItem(dest, itemId, true))
            player->SendNewItem(item, count, true, false);
    }
    else
        ChatHandler(player->GetSession()).PSendSysMessage("Not enough bag space for item %u.", itemId);
}

static void GiveBisList(Player* player, BisList const& items)
{
    for (uint32 itemId : items)
        GiveItemSafe(player, itemId, 1);
}



/* ========================================================= */
/* ================= PRE-RAID BIS HUB ====================== */
/* ========================================================= */

class npc_preraid_bis_hub : public CreatureScript
{
public:
    npc_preraid_bis_hub() : CreatureScript("npc_preraid_bis_hub") {}

    static void ShowMain(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_WARRIOR) + "Warrior" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_WARRIOR);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_PALADIN) + "Paladin" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_PALADIN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_HUNTER) + "Hunter" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_HUNTER);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_ROGUE) + "Rogue" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_ROGUE);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_PRIEST) + "Priest" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_PRIEST);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_SHAMAN) + "Shaman" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_SHAMAN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_MAGE) + "Mage" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_MAGE);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_WARLOCK) + "Warlock" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_WARLOCK);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, std::string(C_DRUID) + "Druid" + C_END, GOSSIP_SENDER_MAIN, ACT_CLASS_DRUID);

        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }

    static void ShowSpecs(Player* player, Creature* creature, uint32 classAction)
    {
        ClearGossipMenuFor(player);

        switch (classAction)
        {
        case ACT_CLASS_WARRIOR:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Arms", GOSSIP_SENDER_MAIN, ACT_WARRIOR_ARMS);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Fury", GOSSIP_SENDER_MAIN, ACT_WARRIOR_FURY);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Protection", GOSSIP_SENDER_MAIN, ACT_WARRIOR_PROT);
            break;

        case ACT_CLASS_PALADIN:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Holy", GOSSIP_SENDER_MAIN, ACT_PALADIN_HOLY);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Protection", GOSSIP_SENDER_MAIN, ACT_PALADIN_PROT);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Retribution", GOSSIP_SENDER_MAIN, ACT_PALADIN_RET);
            break;

        case ACT_CLASS_HUNTER:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Beast Mastery", GOSSIP_SENDER_MAIN, ACT_HUNTER_BM);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Marksmanship", GOSSIP_SENDER_MAIN, ACT_HUNTER_MM);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Survival", GOSSIP_SENDER_MAIN, ACT_HUNTER_SV);
            break;

        case ACT_CLASS_ROGUE:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Assassination", GOSSIP_SENDER_MAIN, ACT_ROGUE_ASSA);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Combat", GOSSIP_SENDER_MAIN, ACT_ROGUE_COMBAT);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Subtlety", GOSSIP_SENDER_MAIN, ACT_ROGUE_SUB);
            break;

        case ACT_CLASS_PRIEST:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Discipline", GOSSIP_SENDER_MAIN, ACT_PRIEST_DISC);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Holy", GOSSIP_SENDER_MAIN, ACT_PRIEST_HOLY);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Shadow", GOSSIP_SENDER_MAIN, ACT_PRIEST_SHADOW);
            break;

        case ACT_CLASS_SHAMAN:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Elemental", GOSSIP_SENDER_MAIN, ACT_SHAMAN_ELE);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Enhancement", GOSSIP_SENDER_MAIN, ACT_SHAMAN_ENH);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Restoration", GOSSIP_SENDER_MAIN, ACT_SHAMAN_RESTO);
            break;

        case ACT_CLASS_MAGE:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Arcane", GOSSIP_SENDER_MAIN, ACT_MAGE_ARCANE);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Fire", GOSSIP_SENDER_MAIN, ACT_MAGE_FIRE);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Frost", GOSSIP_SENDER_MAIN, ACT_MAGE_FROST);
            break;

        case ACT_CLASS_WARLOCK:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Affliction", GOSSIP_SENDER_MAIN, ACT_WARLOCK_AFF);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Demonology", GOSSIP_SENDER_MAIN, ACT_WARLOCK_DEMO);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Destruction", GOSSIP_SENDER_MAIN, ACT_WARLOCK_DESTRO);
            break;

        case ACT_CLASS_DRUID:
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Balance", GOSSIP_SENDER_MAIN, ACT_DRUID_BAL);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Feral", GOSSIP_SENDER_MAIN, ACT_DRUID_FERAL);
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Restoration", GOSSIP_SENDER_MAIN, ACT_DRUID_RESTO);
            break;
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACT_BACK);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ShowMain(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32, uint32 action) override
    {
        // Back
        if (action == ACT_BACK)
        {
            ShowMain(player, creature);
            return true;
        }

        // Class menus (only correct class can open)
        switch (action)
        {
        case ACT_CLASS_WARRIOR: if (!EnsureClass(player, CLASS_WARRIOR)) break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_PALADIN: if (!EnsureClass(player, CLASS_PALADIN)) break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_HUNTER:  if (!EnsureClass(player, CLASS_HUNTER))  break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_ROGUE:   if (!EnsureClass(player, CLASS_ROGUE))   break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_PRIEST:  if (!EnsureClass(player, CLASS_PRIEST))  break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_SHAMAN:  if (!EnsureClass(player, CLASS_SHAMAN))  break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_MAGE:    if (!EnsureClass(player, CLASS_MAGE))    break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_WARLOCK: if (!EnsureClass(player, CLASS_WARLOCK)) break; ShowSpecs(player, creature, action); return true;
        case ACT_CLASS_DRUID:   if (!EnsureClass(player, CLASS_DRUID))   break; ShowSpecs(player, creature, action); return true;
        }

        // Spec selections (re-check class, grant items, close)
        switch (action)
        {
            // Warrior
        case ACT_WARRIOR_ARMS: if (!EnsureClass(player, CLASS_WARRIOR)) break; GiveBisList(player, BIS_WARRIOR_ARMS);   CloseGossipMenuFor(player); return true;
        case ACT_WARRIOR_FURY: if (!EnsureClass(player, CLASS_WARRIOR)) break; GiveBisList(player, BIS_WARRIOR_FURY);   CloseGossipMenuFor(player); return true;
        case ACT_WARRIOR_PROT: if (!EnsureClass(player, CLASS_WARRIOR)) break; GiveBisList(player, BIS_WARRIOR_PROT);   CloseGossipMenuFor(player); return true;

            // Paladin
        case ACT_PALADIN_HOLY: if (!EnsureClass(player, CLASS_PALADIN)) break; GiveBisList(player, BIS_PALADIN_HOLY);   CloseGossipMenuFor(player); return true;
        case ACT_PALADIN_PROT: if (!EnsureClass(player, CLASS_PALADIN)) break; GiveBisList(player, BIS_PALADIN_PROT);   CloseGossipMenuFor(player); return true;
        case ACT_PALADIN_RET:  if (!EnsureClass(player, CLASS_PALADIN)) break; GiveBisList(player, BIS_PALADIN_RET);    CloseGossipMenuFor(player); return true;

            // Hunter
        case ACT_HUNTER_BM:    if (!EnsureClass(player, CLASS_HUNTER)) break; GiveBisList(player, BIS_HUNTER_BM);        CloseGossipMenuFor(player); return true;
        case ACT_HUNTER_MM:    if (!EnsureClass(player, CLASS_HUNTER)) break; GiveBisList(player, BIS_HUNTER_MM);        CloseGossipMenuFor(player); return true;
        case ACT_HUNTER_SV:    if (!EnsureClass(player, CLASS_HUNTER)) break; GiveBisList(player, BIS_HUNTER_SV);        CloseGossipMenuFor(player); return true;

            // Rogue
        case ACT_ROGUE_ASSA:   if (!EnsureClass(player, CLASS_ROGUE)) break; GiveBisList(player, BIS_ROGUE_ASSA);        CloseGossipMenuFor(player); return true;
        case ACT_ROGUE_COMBAT: if (!EnsureClass(player, CLASS_ROGUE)) break; GiveBisList(player, BIS_ROGUE_COMBAT);      CloseGossipMenuFor(player); return true;
        case ACT_ROGUE_SUB:    if (!EnsureClass(player, CLASS_ROGUE)) break; GiveBisList(player, BIS_ROGUE_SUB);         CloseGossipMenuFor(player); return true;

            // Priest
        case ACT_PRIEST_DISC:  if (!EnsureClass(player, CLASS_PRIEST)) break; GiveBisList(player, BIS_PRIEST_DISC);      CloseGossipMenuFor(player); return true;
        case ACT_PRIEST_HOLY:  if (!EnsureClass(player, CLASS_PRIEST)) break; GiveBisList(player, BIS_PRIEST_HOLY);      CloseGossipMenuFor(player); return true;
        case ACT_PRIEST_SHADOW:if (!EnsureClass(player, CLASS_PRIEST)) break; GiveBisList(player, BIS_PRIEST_SHADOW);    CloseGossipMenuFor(player); return true;

            // Shaman
        case ACT_SHAMAN_ELE:   if (!EnsureClass(player, CLASS_SHAMAN)) break; GiveBisList(player, BIS_SHAMAN_ELE);       CloseGossipMenuFor(player); return true;
        case ACT_SHAMAN_ENH:   if (!EnsureClass(player, CLASS_SHAMAN)) break; GiveBisList(player, BIS_SHAMAN_ENH);       CloseGossipMenuFor(player); return true;
        case ACT_SHAMAN_RESTO: if (!EnsureClass(player, CLASS_SHAMAN)) break; GiveBisList(player, BIS_SHAMAN_RESTO);     CloseGossipMenuFor(player); return true;

            // Mage
        case ACT_MAGE_ARCANE:  if (!EnsureClass(player, CLASS_MAGE)) break; GiveBisList(player, BIS_MAGE_ARCANE);        CloseGossipMenuFor(player); return true;
        case ACT_MAGE_FIRE:    if (!EnsureClass(player, CLASS_MAGE)) break; GiveBisList(player, BIS_MAGE_FIRE);          CloseGossipMenuFor(player); return true;
        case ACT_MAGE_FROST:   if (!EnsureClass(player, CLASS_MAGE)) break; GiveBisList(player, BIS_MAGE_FROST);         CloseGossipMenuFor(player); return true;

            // Warlock
        case ACT_WARLOCK_AFF:  if (!EnsureClass(player, CLASS_WARLOCK)) break; GiveBisList(player, BIS_WARLOCK_AFF);     CloseGossipMenuFor(player); return true;
        case ACT_WARLOCK_DEMO: if (!EnsureClass(player, CLASS_WARLOCK)) break; GiveBisList(player, BIS_WARLOCK_DEMO);    CloseGossipMenuFor(player); return true;
        case ACT_WARLOCK_DESTRO:if (!EnsureClass(player, CLASS_WARLOCK)) break; GiveBisList(player, BIS_WARLOCK_DESTRO); CloseGossipMenuFor(player); return true;

            // Druid
        case ACT_DRUID_BAL:    if (!EnsureClass(player, CLASS_DRUID)) break; GiveBisList(player, BIS_DRUID_BALANCE);     CloseGossipMenuFor(player); return true;
        case ACT_DRUID_FERAL:  if (!EnsureClass(player, CLASS_DRUID)) break; GiveBisList(player, BIS_DRUID_FERAL);       CloseGossipMenuFor(player); return true;
        case ACT_DRUID_RESTO:  if (!EnsureClass(player, CLASS_DRUID)) break; GiveBisList(player, BIS_DRUID_RESTO);       CloseGossipMenuFor(player); return true;
        }

        // If wrong class / unknown action, bounce to main
        ShowMain(player, creature);
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

class npc_glyphs_enchants_hub : public CreatureScript
{
public:
    npc_glyphs_enchants_hub() : CreatureScript("npc_glyphs_enchants_hub") {}

    enum Actions : uint32
    {
        ACT_MAIN_GLYPHS = 3001,
        ACT_MAIN_ENCHANTS = 3002,

        ACT_BACK_MAIN = 3099,

        // Enchants
        ACT_ENCH_ARMOR_I = 3101,
        ACT_ENCH_ARMOR_II = 3102,
        ACT_ENCH_WEAPON = 3103,

        // Glyphs (class selection)
        ACT_GLYPH_DRUID = 3201,
        ACT_GLYPH_HUNTER = 3202,
        ACT_GLYPH_MAGE = 3203,
        ACT_GLYPH_PALADIN = 3204,
        ACT_GLYPH_ROGUE = 3205,
        ACT_GLYPH_SHAMAN = 3206,
        ACT_GLYPH_WARLOCK = 3207,
        ACT_GLYPH_WARRIOR = 3208,
        ACT_GLYPH_PRIEST = 3209,

        ACT_MAIN_CONSUMABLES = 3003,

        // Consumables
        ACT_CONS_BANDAGES = 3301,
        ACT_CONS_ELIXIRS = 3302,
        ACT_CONS_FLASKS = 3303,
        ACT_CONS_SCROLLS = 3304,
        ACT_CONS_POTIONS = 3305

    };

    static void ShowMain(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Glyphs", GOSSIP_SENDER_MAIN, ACT_MAIN_GLYPHS);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Enchants", GOSSIP_SENDER_MAIN, ACT_MAIN_ENCHANTS);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Consumables", GOSSIP_SENDER_MAIN, ACT_MAIN_CONSUMABLES);


        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }

    static void ShowEnchants(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Armor Enchants I", GOSSIP_SENDER_MAIN, ACT_ENCH_ARMOR_I);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Armor Enchants II", GOSSIP_SENDER_MAIN, ACT_ENCH_ARMOR_II);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Weapon Enchants", GOSSIP_SENDER_MAIN, ACT_ENCH_WEAPON);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACT_BACK_MAIN);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }

    static void ShowGlyphs(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);

        // colored class names (you already defined C_* above)
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_DRUID) + "Druid" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_DRUID);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_HUNTER) + "Hunter" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_HUNTER);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_MAGE) + "Mage" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_MAGE);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_PALADIN) + "Paladin" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_PALADIN);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_ROGUE) + "Rogue" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_ROGUE);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_SHAMAN) + "Shaman" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_SHAMAN);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_WARLOCK) + "Warlock" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_WARLOCK);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_WARRIOR) + "Warrior" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_WARRIOR);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, std::string(C_PRIEST) + "Priest" + C_END, GOSSIP_SENDER_MAIN, ACT_GLYPH_PRIEST);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACT_BACK_MAIN);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }

    static void ShowConsumables(Player* player, Creature* creature)
    {
        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Bandages", GOSSIP_SENDER_MAIN, ACT_CONS_BANDAGES);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Elixirs", GOSSIP_SENDER_MAIN, ACT_CONS_ELIXIRS);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Flasks", GOSSIP_SENDER_MAIN, ACT_CONS_FLASKS);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Scrolls", GOSSIP_SENDER_MAIN, ACT_CONS_SCROLLS);
        AddGossipItemFor(player, GOSSIP_ICON_VENDOR, "Potions", GOSSIP_SENDER_MAIN, ACT_CONS_POTIONS);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Back", GOSSIP_SENDER_MAIN, ACT_BACK_MAIN);
        SendGossipMenuFor(player, player->GetGossipTextId(creature), creature->GetGUID());
    }


    static void OpenVendor(Player* player, Creature* creature, uint32 vendorEntry)
    {
        CloseGossipMenuFor(player);
        player->GetSession()->SendListInventory(creature->GetGUID(), vendorEntry);
    }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ShowMain(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        switch (action)
        {
        case ACT_MAIN_CONSUMABLES:
            ShowConsumables(player, creature);
            return true;

        case ACT_CONS_BANDAGES: OpenVendor(player, creature, VENDOR_CONS_BANDAGES); return true;
        case ACT_CONS_ELIXIRS:  OpenVendor(player, creature, VENDOR_CONS_ELIXIRS);  return true;
        case ACT_CONS_FLASKS:   OpenVendor(player, creature, VENDOR_CONS_FLASKS);   return true;
        case ACT_CONS_SCROLLS:  OpenVendor(player, creature, VENDOR_CONS_SCROLLS);  return true;
        case ACT_CONS_POTIONS:  OpenVendor(player, creature, VENDOR_CONS_POTIONS);  return true;

        case ACT_BACK_MAIN:
            ShowMain(player, creature);
            return true;

        case ACT_MAIN_GLYPHS:
            ShowGlyphs(player, creature);
            return true;

        case ACT_MAIN_ENCHANTS:
            ShowEnchants(player, creature);
            return true;

            // Enchants (no class restriction)
        case ACT_ENCH_ARMOR_I:  OpenVendor(player, creature, VENDOR_ENCH_ARMOR_I);  return true;
        case ACT_ENCH_ARMOR_II: OpenVendor(player, creature, VENDOR_ENCH_ARMOR_II); return true;
        case ACT_ENCH_WEAPON:   OpenVendor(player, creature, VENDOR_ENCH_WEAPON);   return true;

            // Glyphs (restrict: only the correct class may open that class vendor)
        case ACT_GLYPH_DRUID:   if (!EnsureClass(player, CLASS_DRUID)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_DRUID);   return true;
        case ACT_GLYPH_HUNTER:  if (!EnsureClass(player, CLASS_HUNTER)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_HUNTER);  return true;
        case ACT_GLYPH_MAGE:    if (!EnsureClass(player, CLASS_MAGE)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_MAGE);    return true;
        case ACT_GLYPH_PALADIN: if (!EnsureClass(player, CLASS_PALADIN)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_PALADIN); return true;
        case ACT_GLYPH_ROGUE:   if (!EnsureClass(player, CLASS_ROGUE)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_ROGUE);   return true;
        case ACT_GLYPH_SHAMAN:  if (!EnsureClass(player, CLASS_SHAMAN)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_SHAMAN);  return true;
        case ACT_GLYPH_WARLOCK: if (!EnsureClass(player, CLASS_WARLOCK)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_WARLOCK); return true;
        case ACT_GLYPH_WARRIOR: if (!EnsureClass(player, CLASS_WARRIOR)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_WARRIOR); return true;
        case ACT_GLYPH_PRIEST:  if (!EnsureClass(player, CLASS_PRIEST)) { ShowGlyphs(player, creature); return true; } OpenVendor(player, creature, VENDOR_GLYPH_PRIEST);  return true;
        }



        ShowMain(player, creature);
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
    new npc_glyphs_enchants_hub();
}
