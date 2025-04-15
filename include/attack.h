#ifndef __ATACK_H__
#define __ATACK_H__

#include "common.h"
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include "enums.h"
#include "battler.h"

class Battler;
class Attack;

// always contains a nullptr at index 0
static std::unordered_map<unsigned int,Attack*> static_attacks;

AttackType stringToAttackType(std::string type);
std::string attackTypeToString(AttackType type);

AttackTarget stringToAttackTarget(std::string type);
std::string attackTargetToString(AttackTarget type);

class Attack{
    private:
    unsigned int id;
    Type type;
    std::string name;
    std::string description;
    unsigned int power;
    unsigned int accuracy;// as a percentage
    AttackType attack_type;
    unsigned int effect_id;
    unsigned int effect_chance;// as a percentage
    int priority_level;
    unsigned int max_pp;
    AttackTarget target;
    AttackTarget effect_target;
    bool is_contact;
    bool cannot_be_metronomed;
    bool is_sound_based;
    bool is_punching;
    bool is_powder;
    bool is_pulse;
    public:
    Attack();
    Attack(unsigned int index, std::map<std::string,std::string> data);
    ~Attack();
    unsigned int getId()const;
    Type getType()const;
    Type getType(Battler* user)const;
    unsigned int getPower()const;
    unsigned int getAccuracy()const;
    AttackType getCategory()const;
    unsigned int getEffectId()const;
    unsigned int getEffectChance()const;
    unsigned int getMaxPP()const;
    int getPriorityLevel()const;
    std::string getName()const;
    std::string getDescription()const;
    static void loadAttacks();
    AttackTarget getTarget()const;
    AttackTarget getEffectTarget()const;
    bool makesContact()const;
    bool isSoundBased()const;
    bool isPunching()const;
    bool isPowder()const;
    bool isPulse()const;
    static Attack* getAttack(unsigned int attack_id);
    static Attack* getRandomMetronomeAttack();
    void printSummary()const;
    static void printAllSummaries();
};

#endif