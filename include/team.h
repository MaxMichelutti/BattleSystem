#ifndef __TEAM_H__
#define __TEAM_H__

#include "common.h"
#include "monster.h"

class MonsterTeam{
    private:
    Monster* monsters[6];
    unsigned int team_size;
    public:
    MonsterTeam();
    ~MonsterTeam();
    MonsterTeam(const MonsterTeam& other);
    void addMonster(Monster* monster);
    Monster* removeMonster(unsigned int index);
    void swapMonsters(unsigned int index1, unsigned int index2);
    void swapActiveMonster(unsigned int index);
    Monster* getActiveMonster()const;
    Monster* getMonster(unsigned int index)const;
    bool isEmpty()const;
    bool isFull()const;
    bool isDead()const;
    std::vector<Monster*> getMonsters()const;
    bool hasAliveBackup()const;
    void swapRandomMonster();
    void clearBattleEffectsAll();
};

#endif