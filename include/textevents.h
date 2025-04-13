#ifndef __TEXT_EVENT_HANDLER_H__
#define __TEXT_EVENT_HANDLER_H__

#include "events.h"
#include <iostream>
#include "battle.h"
#include "common.h"
#include "util.h"
#include "attack.h"
#include "monster.h"
#include "team.h"
#include "bag.h"

class TextEventHandler : public EventHandler {
    private:
    unsigned int getNumberFromCin();
    public:
    TextEventHandler();
    ~TextEventHandler();
    BattleAction chooseAction(Battler*,MonsterTeam*,Battler*,Field*,Bag*);
    unsigned int chooseAttack(Battler*);
    unsigned int chooseSwitch(MonsterTeam*);
    unsigned int chooseSwitchForced(MonsterTeam*);
    unsigned int chooseItemTarget(ItemType,MonsterTeam*);
    std::pair<ItemType,unsigned int> chooseItem(Bag*,MonsterTeam*);
    std::pair<ItemType,unsigned int> chooseItemFromPocket(Pocket*,MonsterTeam*);
    void displayMsg(const std::string& msg);
    void displayMsgNoEndl(const std::string& msg);
    void displayBattleSituation(Battler*,MonsterTeam*,Battler*,MonsterTeam*);
};


#endif