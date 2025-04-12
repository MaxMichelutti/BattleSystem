
#ifndef __ABILITIES_H__
#define __ABILITIES_H__
#include "enums.h"
#include "constants.h"
#include "util.h"
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>

Ability stringToAbility(std::string);
static std::unordered_map<std::string,Ability> strToAbility = {
    {"OVERGROW",OVERGROW},
    {"CHLOROPHYL",CHLOROPHYL},
    {"CHLOROPHYLL",CHLOROPHYL},
    {"BLAZE",BLAZE},
    {"SOLAR_POWER",SOLAR_POWER},
    {"TORRENT",TORRENT},
    {"SWIFT_SWIM",SWIFT_SWIM},
    {"INSOMNIA",INSOMNIA},
    {"RAIN_DISH",RAIN_DISH},
    {"SHED_SKIN",SHED_SKIN},
    {"SHIELD_DUST",SHIELD_DUST},
    {"RUN_AWAY",RUN_AWAY},
    {"COMPOUND_EYES",COMPOUND_EYES},
    {"TINTED_LENS",TINTED_LENS},
    {"SNIPER",SNIPER},
    {"SWARM",SWARM},
    {"KEEN_EYE",KEEN_EYE},
    {"TANGLED_FEET",TANGLED_FEET},
    {"BIG_PECKS",BIG_PECKS},
    {"GUTS",GUTS},
    {"HUSTLE",HUSTLE},
    {"INTIMIDATE",INTIMIDATE},
    {"UNNERVE",UNNERVE},
    {"STATIC",STATIC},
    {"LIGHTNING_ROD",LIGHTNING_ROD},
    {"SAND_VEIL",SAND_VEIL},
    {"SAND_RUSH",SAND_RUSH},
    {"SHEER_FORCE",SHEER_FORCE},
    {"POISON_POINT",POISON_POINT},
    {"RIVALRY",RIVALRY},
    {"CUTE_CHARM",CUTE_CHARM},
    {"MAGIC_GUARD",MAGIC_GUARD},
    {"FRIEND_GUARD",FRIEND_GUARD},
    {"UNAWARE",UNAWARE},
    {"COMPETITIVE",COMPETITIVE},
    {"FLASH_FIRE",FLASH_FIRE},
    {"DROUGHT",DROUGHT},
    {"FRISK",FRISK},//shows item of enemy
    {"INNER_FOCUS",INNER_FOCUS},
    {"INFILTRATOR",INFILTRATOR},
    {"STENCH",STENCH},//no battle effect
    {"EFFECT_SPORE",EFFECT_SPORE},
    {"DAMP",DAMP},
    {"DRY_SKIN",DRY_SKIN},
    {"WONDER_SKIN",WONDER_SKIN},
    {"ARENA_TRAP",ARENA_TRAP},
    {"IMPOSTOR",IMPOSTOR},
    {"IMPOSTER",IMPOSTOR},
    {"LIMBER",LIMBER},
    {"SAND_FORCE",SAND_FORCE},
    {"PICKUP",PICKUP},//does nothing in battle
    {"TECHNICIAN",TECHNICIAN},
    {"CLOUD_NINE",CLOUD_NINE},
    {"VITAL_SPIRIT",VITAL_SPIRIT},
    {"ANGER_POINT",ANGER_POINT},
    {"DEFIANT",DEFIANT},
    {"JUSTIFIED",JUSTIFIED},
    {"WATER_ABSORB",WATER_ABSORB},
    {"SYNCHRONIZE",SYNCHRONIZE},
    {"NO_GUARD",NO_GUARD},
    {"STEADFAST",STEADFAST},
    {"GLUTTONY",GLUTTONY},
    {"LIQUID_OOZE",LIQUID_OOZE},
    {"CLEAR_BODY",CLEAR_BODY},
    {"STURDY",STURDY},
    {"ROCK_HEAD",ROCK_HEAD},
    {"FLAME_BODY",FLAME_BODY},
    {"OWN_TEMPO",OWN_TEMPO},
    {"OBLIVIOUS",OBLIVIOUS},
    {"REGENERATOR",REGENERATOR},
    {"MAGNET_PULL",MAGNET_PULL},
    {"ANALYTIC",ANALYTIC},
    {"EARLY_BIRD",EARLY_BIRD},
    {"THICK_FAT",THICK_FAT},
    {"HYDRATION",HYDRATION},
    {"ICE_BODY",ICE_BODY},
    {"STICKY_HOLD",STICKY_HOLD},
    {"POISON_TOUCH",POISON_TOUCH},
    {"SHELL_ARMOR",SHELL_ARMOR},
    {"SKILL_LINK",SKILL_LINK},
    {"OVERCOAT",OVERCOAT},
    {"LEVITATE",LEVITATE},
    {"CURSED_BODY",CURSED_BODY},
    {"WEAK_ARMOR",WEAK_ARMOR},
    {"FOREWARN",FOREWARN},
    {"HYPER_CUTTER",HYPER_CUTTER},
    {"SOUNDPROOF",SOUNDPROOF},
    {"AFTERMATH",AFTERMATH},
    {"PLUS",PLUS},
    {"MINUS",MINUS},
    {"HARVEST",HARVEST},//only works with items
    {"BATTLE_ARMOR",BATTLE_ARMOR},
    {"RECKLESS",RECKLESS},
    {"UNBURDEN",UNBURDEN},
    {"IRON_FIST",IRON_FIST},
    {"NEUTRALIZING_GAS",NEUTRALIZING_GAS},
    {"NATURAL_CURE",NATURAL_CURE},
    {"SERENE_GRACE",SERENE_GRACE},
    {"HEALER",HEALER},//only works in doubles
    {"LEAF_GUARD",LEAF_GUARD},
    {"SCRAPPY",SCRAPPY},
    {"WATER_VEIL",WATER_VEIL},
    {"ILLUMINATE",ILLUMINATE},
    {"FILTER",FILTER},
    {"MOLD_BREAKER",MOLD_BREAKER},
    {"MOXIE",MOXIE},
    {"RATTLED",RATTLED},
    {"ADAPTABILITY",ADAPTABILITY},
    {"ANTICIPATION",ANTICIPATION},
    {"VOLT_ABSORB",VOLT_ABSORB},
    {"QUICK_FEET",QUICK_FEET},
    {"TRACE",TRACE},
    {"DOWNLOAD",DOWNLOAD},
    {"PRESSURE",PRESSURE},
    {"IMMUNITY",IMMUNITY},
    {"SNOW_CLOAK",SNOW_CLOAK},
    {"MULTISCALE",MULTISCALE},
    {"MARVEL_SCALE",MARVEL_SCALE}
};

std::string abilityToString(Ability);
std::string abilityToSerialString(Ability ability);
bool isAbilityIgnorable(Ability ability);
bool isAbilityTraceable(Ability ability);
bool doesAbilityIgnoreIntimidate(Ability ability);

class AbilityItem{
    private:
    std::string igname;
    std::string serial_name;
    std::string description;
    public:
    AbilityItem();
    AbilityItem(std::string igname, std::string serial_name, std::string description);
    ~AbilityItem();
    std::string getIGName()const;
    std::string getSerialName()const;
    std::string getDescription()const;
    static void initAbilities();
    static AbilityItem* getAbilityItem(Ability ability);
};

static std::unordered_map<Ability,AbilityItem*> abilityMap;

#endif