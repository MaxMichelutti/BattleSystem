#ifndef __CPU_AI_H___
#define __CPU_AI_H___

#include "common.h"
#include "battle.h"

class BattleAction;
class Battler;
class MonsterTeam;
class Field;

class CPUAI{
    private:
    unsigned int skill;
    int computeUtility(unsigned int attack_id, Battler* cpu_active,Battler* enemy_active, Field*field)const;
    unsigned int chooseRandomAttack(Battler* active_monster)const;
    public:
    CPUAI();
    CPUAI(unsigned int skill);
    ~CPUAI();
    unsigned int getSkill()const;
    BattleAction chooseAction(Battler*,MonsterTeam*,Battler*,Field*)const;
    unsigned int chooseAttack(Battler*,Battler*,Field*)const;
    unsigned int chooseSwitch(Battler*,MonsterTeam*,Battler*,Field*)const;
};

#endif