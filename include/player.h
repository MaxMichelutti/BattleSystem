#ifndef __MONSTER_PLAYER_H__
#define __MONSTER_PLAYER_H__
#include "common.h"
#include "events.h"
#include "bag.h"
#include "team.h"
#include <string>
#include <iostream>

class Player;
class MonsterTeam;
class Bag;
class EventHandler;

static Player* global_player;

class Player{
    std::string name;
    MonsterTeam* team;
    Bag* bag;
    unsigned int money;
    EventHandler* event_handler;
    Player();

    public:
    static void createPlayer();
    static Player* getPlayer();
    ~Player();
    void setName(std::string name);
    std::string getName()const;
    MonsterTeam* getTeam()const;
    Bag* getBag()const;
    void addMoney(unsigned int money);
    void removeMoney(unsigned int money);
    bool hasMoney(unsigned int money)const;
    EventHandler* getEventHandler()const;
    void setEventHandler(EventHandler* event_handler);
};

#endif