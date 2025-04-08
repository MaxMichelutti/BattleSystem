#include "team.h"


MonsterTeam::MonsterTeam() {
    team_size = 0;
    for (int i = 0; i < 6; i++) {
        monsters[i] = nullptr;
    }
}

MonsterTeam::~MonsterTeam() {
    for (int i = 0; i < team_size; i++) {
        delete monsters[i];
    }
}

MonsterTeam::MonsterTeam(const MonsterTeam& other) {
    team_size = other.team_size;
    for (int i = 0; i < team_size; i++) {
        monsters[i] = other.monsters[i];
    }
}

void MonsterTeam::addMonster(Monster* monster) {
    if (team_size >= 6) {
        std::cerr << "Error: Team is full!" << std::endl;
        return;
    }
    monsters[team_size++] = monster;
}

Monster* MonsterTeam::removeMonster(unsigned int index) {
    if (index >= team_size) {
        std::cerr << "Error: Invalid index!" << std::endl;
        return nullptr;
    }
    Monster* temp = monsters[index];
    for (unsigned int i = index; i < team_size - 1; i++) {
        monsters[i] = monsters[i + 1];
    }
    monsters[team_size - 1] = nullptr;
    team_size--;
    return temp;
}

void MonsterTeam::swapMonsters(unsigned int index1, unsigned int index2) {
    if (index1 >= team_size || index2 >= team_size) {
        std::cerr << "Error: Invalid index!" << std::endl;
        return;
    }
    std::swap(monsters[index1], monsters[index2]);
}

void MonsterTeam::swapActiveMonster(unsigned int index) {
    if (index >= team_size) {
        std::cerr << "Error: Invalid index!" << std::endl;
        return;
    }
    // std::cout << "Swapping active monster with monster at index " << index << std::endl;
    std::swap(monsters[0], monsters[index]);
}

Monster* MonsterTeam::getActiveMonster()const {
    for(int i=0;i<team_size;i++){
        if(!monsters[i]->isFainted()){
            return monsters[i];
        }
    }
    return nullptr;
}

std::vector<Monster*> MonsterTeam::getMonsters()const {
    std::vector<Monster*> monster_vector;
    for (unsigned int i = 0; i < team_size; i++) {
        monster_vector.push_back(monsters[i]);
    }
    return monster_vector;
}

Monster* MonsterTeam::getMonster(unsigned int index)const {
    if (index >= team_size) {
        std::cerr << "Error: Invalid index!" << std::endl;
        return nullptr;
    }
    return monsters[index];
}

bool MonsterTeam::isEmpty()const {
    return team_size == 0;
}

bool MonsterTeam::isFull()const {
    return team_size == 6;
}

bool MonsterTeam::isDead()const {
    for (unsigned int i = 0; i < team_size; i++) {
        if (!monsters[i]->isFainted()) {
            return false;
        }
    }
    return true;
}

bool MonsterTeam::hasAliveBackup()const {
    for (unsigned int i = 1; i < team_size; i++) {
        if (!monsters[i]->isFainted()) {
            return true;
        }
    }
    return false;
}

void MonsterTeam::swapRandomMonster() {
    if (! hasAliveBackup()) {
        std::cerr << "Error: No alive monsters to swap!" << std::endl;
        return;
    }
    bool found = false;
    unsigned index = RNG::getRandomInteger(1,team_size-1);
    for(unsigned int i=index; i<team_size;i++){
        if(!monsters[i]->isFainted()){
            index = i;
            found=true;
            break;
        }
    }
    if(!found){
        for(int i=1;i<index;i++){
            if(!monsters[i]->isFainted()){
                index = i;
                found=true;
                break;
            }
        }
    }
    swapActiveMonster(index);
}

void MonsterTeam::clearBattleEffectsAll() {
    for (unsigned int i = 0; i < team_size; i++) {
        if(monsters[i] == nullptr)
            continue;
        monsters[i]->clearBattleData();
    }
}