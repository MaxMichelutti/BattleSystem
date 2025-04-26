#ifndef __PLAYER_H___
#define __PLAYER_H___
#include "common.h"
#include "bag.h"
#include "team.h"
#include <string>
#include <iostream>
#include "textevents.h"

class Player{
    std::string name;
    MonsterTeam* team;
    Bag* bag;
    unsigned int money;
    Player();
    EventHandler* event_handler;
    
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

static Player* global_player = nullptr;

#endif