#ifndef __CPU_AI_H___
#define __CPU_AI_H___

#include "common.h"
#include "battle.h"
#include "bag.h"

class BattleAction;
class Battler;
class MonsterTeam;
class Field;

struct Choice{
    unsigned int choice_id;
    int utility;
    BattleActionType action_type;
    ItemType item;
    Choice();
    Choice(unsigned int choice_id, int utility, BattleActionType action_type);
    Choice(ItemType item, int utility);
    ~Choice();
};

class CPUAI{
    private:
    unsigned int skill;
    int computeAttackUtility(unsigned int attack_id, Battler* cpu_active,Battler* enemy_active, Field*field)const;
    int computeItemUtility(ItemType item_id, Battler* cpu_active)const;
    unsigned int chooseRandomAttack(Battler* active_monster)const;
    public:
    CPUAI();
    CPUAI(unsigned int skill);
    ~CPUAI();
    unsigned int getSkill()const;
    BattleAction chooseAction(Battler*,MonsterTeam*,Battler*,Field*,Bag*)const;
    Choice* getBestAttackChoice(Battler*,Battler*,Field*)const;
    Choice* getBestSwitchChoice(Battler*,MonsterTeam*,Battler*,Field*)const;
    Choice* getBestItemChoice(Battler*,Bag*)const;
    unsigned int chooseSwitch(Battler*,MonsterTeam*,Battler*,Field*)const;
};

#endif