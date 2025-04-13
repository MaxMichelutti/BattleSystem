#ifndef __EVENTS__H__
#define __EVENTS__H__

#include "common.h"
#include "battle.h"
#include "bag.h"
#include "team.h"

class BattleAction;
class Battler;
class MonsterTeam;
class Field;
class Bag;
class MonsterTeam;
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
    virtual BattleAction chooseAction(Battler*,MonsterTeam*,Battler*,Field*,Bag*) = 0;
    virtual unsigned int chooseAttack(Battler*) = 0;
    virtual unsigned int chooseSwitch(MonsterTeam*) = 0;
    virtual unsigned int chooseSwitchForced(MonsterTeam*) = 0;
    virtual std::pair<ItemType,unsigned int> chooseItem(Bag*,MonsterTeam*,Battler*) = 0;//returns pair <ITEM TO USE, MONSTER INDEX IN TEAM TO USE ITEM ON>
    virtual void displayMsg(const std::string& msg) = 0;
    virtual void displayBattleSituation(Battler*,MonsterTeam*,Battler*,MonsterTeam*) = 0;
};

#endif