#ifndef __EVENTS__H__
#define __EVENTS__H__

#include "common.h"
#include "battle.h"

class BattleAction;
class Battler;
class MonsterTeam;
class Field;
// BattleAction choosePlayerAction(Battler*,MonsterTeam*);
// unsigned int choosePlayerAttack(Battler*);
// unsigned int choosePlayerSwitch(MonsterTeam*);

// BattleAction chooseOpponentAction(Battler*,MonsterTeam*,Battler*,const Field&);
// unsigned int chooseOpponentAttack(Battler*,Battler*,const Field&);
// unsigned int chooseOpponentSwitch(MonsterTeam*,Battler*,const Field&);


BattleAction forcedAction(BattleActionActor, Battler*, Field*);

class EventHandler{
    public:
    EventHandler();
    ~EventHandler();
    virtual BattleAction chooseAction(Battler*,MonsterTeam*,Battler*,Field*) = 0;
    virtual unsigned int chooseAttack(Battler*) = 0;
    virtual unsigned int chooseSwitch(MonsterTeam*) = 0;
    virtual unsigned int chooseSwitchForced(MonsterTeam*) = 0;
    virtual void displayMsg(const std::string& msg) = 0;
    virtual void displayBattleSituation(Battler*,MonsterTeam*,Battler*,MonsterTeam*) = 0;
};

#endif