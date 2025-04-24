#ifndef __COMMON_H__
#define __COMMON_H__

#include "constants.h"
#include "rng.h"
#include "util.h"
#include <iostream>
#include <set>
#include <unordered_map>
#include "enums.h"
#include <math.h>

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

std::string formKindToString(FormKind form_kind);
FormKind stringToFormKind(std::string form_kind);

Type stringToType(std::string type);
std::string typeToString(Type type);
float getTypeEffectiveness(Type attacker, Type defender_1, Type defender_2, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
float getTypeEffectiveness(Type attacker, Type defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
float getTypeEffectiveness(Type attacker, std::set<Type> defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry);
std::set<Type> getAllTypesSet();

unsigned int computeExperience(unsigned int base,unsigned int level,bool active_participation, unsigned int level_active, bool is_traded,bool uses_lucky_egg, bool is_past_evo_level);

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