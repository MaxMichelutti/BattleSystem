#ifndef __COMMON_H__
#define __COMMON_H__

#include "constants.h"
#include "rng.h"
#include "util.h"
#include <iostream>
#include <set>
#include <unordered_map>
#include "enums.h"


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
    {"NAURAL_CURE",NATURAL_CURE},
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
bool isAbilityIgnorable(Ability ability);
bool isAbilityTraceable(Ability ability);

std::string permanentStatusConditionToString(PermanentStatusCondition status);
PermanentStatusCondition stringToPermanentStatusCondition(std::string status);
PermanentStatusCondition decrementSleep(PermanentStatusCondition status);



Gender stringToGender(std::string);
std::string genderToString(Gender);
bool areMaleAndFemale(Gender,Gender);


GenderRule stringToGenderRule(std::string);
std::string genderRuleToString(GenderRule);



Nature stringToNature(std::string nature);
std::string natureToString(Nature nature);
Nature randomNature();



Type stringToType(std::string type);
std::string typeToString(Type type);
float getTypeEffectiveness(Type attacker, Type defender_1, Type defender_2, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
float getTypeEffectiveness(Type attacker, Type defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
float getTypeEffectiveness(Type attacker, std::set<Type> defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
std::set<Type> getAllTypesSet();

class Stats{
    protected:
    unsigned int hp;
    unsigned int atk;
    unsigned int spatk;
    unsigned int def;
    unsigned int spdef;
    unsigned int spd;
    public:
    Stats();
    Stats(std::string);
    ~Stats();
    unsigned int getHp()const;
    unsigned int getAtk()const;
    unsigned int getSpatk()const;
    unsigned int getDef()const;
    unsigned int getSpdef()const;
    unsigned int getSpd()const;
    void setHp(unsigned int);
    void setAtk(unsigned int);
    void setSpatk(unsigned int);
    void setDef(unsigned int);
    void setSpdef(unsigned int);
    void setSpd(unsigned int);
    void applyNatureBoosts(Nature nature);
};

class EVs: public Stats{
    private:
    unsigned int get_total();
    public:
    EVs();
    ~EVs();
    bool changeAtk(int amount);
    bool changeDef(int amount);
    bool changeSpatk(int amount);
    bool changeSpdef(int amount);
    bool changeSpd(int amount);
    bool changeHp(int amount);
};

class IVs: public Stats{
    public:
    IVs();
    ~IVs();
    static IVs generateRandomIVs();
};

#endif