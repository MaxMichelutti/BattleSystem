#ifndef __MONSTER_H__
#define __MONSTER_H__
#include "common.h"
#include "species.h"
#include "attack.h"
#include "expcurve.h"
#include "abilities.h"
#include "enums.h"
#include "common.h"
#include "events.h"
#include "player.h"
#include <vector>
#include <string>
#include <iostream>

class EventHandler;
class Player;

class AttackData{
    public:
    unsigned int attack_id;
    unsigned int current_pp;
    AttackData();
    AttackData(unsigned int attack_id);
    AttackData(unsigned int attack_id, unsigned int current_pp);
};

class Monster{
    private:
    unsigned int species_id;
    unsigned int level;
    unsigned int damage;
    unsigned long experience;
    unsigned int friendship;
    Stats stats;
    EVs effort;
    IVs individual;
    AttackData attack_ids[4];
    // AttackData* mimicked_attack;
    std::string nickname;
    Nature nature;
    Gender gender;
    Ability ability;
    bool is_mega;
    Ability mega_ability;
    PermanentStatusCondition permanent_status;
    bool has_hidden_ability;
    Monster* transformation;
    ItemType held_item;
    ItemType consumed_item;
    unsigned int form_id;
    Type hidden_power_type;
    std::set<Monster*> seen_opponents;
    ItemType ball_containing_monster;

    void updateStats();
    void packAttacks();
    void levelUp(EventHandler*handler);
    void init(unsigned int species_id, unsigned int level,unsigned int form_id);
    void init_stats();
    void init_gender(GenderRule);
    void completeEvolution(unsigned int target_species_id,unsigned int target_form_id);
    void evolveIntoShedinja();
    public:
    Monster(unsigned int species_id);
    Monster(unsigned int species_id, unsigned int level);
    Monster(unsigned int species_id, unsigned int level,unsigned int form_id);
    Monster(Monster* other);
    ~Monster();
    unsigned int getSpeciesId()const;
    Stats getStats()const;
    EVs getEffort()const;
    IVs getIndividual()const;
    std::vector<unsigned int> getAttackIds()const;
    std::map<unsigned int,unsigned int> getAttacks()const;
    std::string getNickname()const;
    unsigned int getLevel()const;
    Nature getNature()const;
    Gender getGender()const;
    Type getType1()const;
    Type getType2()const;
    Type getHiddenPowerType()const;
    unsigned long getExperience()const;
    unsigned int getFriendship()const;
    unsigned int getMaxHP()const;
    unsigned int getCurrentHP()const;
    unsigned int getDamage()const;
    Ability getAbility()const;
    PermanentStatusCondition getPermanentStatus()const;
    void setPermanentStatus(PermanentStatusCondition);
    void setNickname(std::string);
    void changeEffortAtk(int amount);
    void changeEffortDef(int amount);
    void changeEffortSpatk(int amount);
    void changeEffortSpdef(int amount);
    void changeEffortSpd(int amount);
    void changeEffortHp(int amount);
    void changeFriendship(int amount);
    bool hasAttack(unsigned int attack_id)const;
    bool learnAttack(unsigned int attack_id);
    bool forgetAttack(unsigned int attack_id);
    bool replaceAttack(unsigned int old_attack_id, unsigned int new_attack_id);
    void gainExperience(unsigned long exp,EventHandler* handler);
    static Monster* generateRandomMonster(unsigned int species_id);
    static Monster* generateRandomMonster(unsigned int species_id, unsigned int level);
    static Monster* generateRandomMonster(unsigned int species_id, unsigned int level,unsigned int form_id);
    void printSummary()const;
    bool canEvolve()const;
    void evolve();
    void heal();
    bool isFainted()const;
    bool hasPP(unsigned int attack_id)const;
    void usePP(unsigned int attack_id, unsigned int amount);
    void recoverPP(unsigned int attack_id, unsigned int amount);
    unsigned int addDamage(unsigned int damage);// returns the actual damage that was dealt
    unsigned int removeDamage(unsigned int damage);// returns the actual damage that was healed
    bool isAtFullHP()const;
    bool hasType(Type type)const;
    // bool setMimickedAttack(unsigned int attack_id);
    // bool hasMimickedAttack()const;
    void clearBattleData();
    unsigned int getMaxPPForAttack(unsigned int attack_id)const;
    void transformInto(Monster* other);
    bool isTransformed()const;
    unsigned int getTransformedSpeciesId()const;
    unsigned int getHeight()const;
    unsigned int getWeight()const;
    bool useItem(ItemType item, EventHandler*, unsigned int data);
    bool itemWouldHaveEffect(ItemType item)const;
    bool hasHeldItem()const;
    ItemType getHeldItem()const;
    ItemType setHeldItem(ItemType item);
    ItemType removeHeldItem();
    bool dislikesBerry(ItemType item)const;
    bool likesBerry(ItemType item)const;
    // bool hasMaxedEVs()const;
    unsigned int getFormId()const;
    void setConsumedItem(ItemType item);
    ItemType getConsumedItem()const;
    bool megaEvolve();
    bool isMegaEvolved()const;
    bool canMegaEvolve()const;
    void cancelMega();
    void addSeenOpponent(Monster* opponent);
    bool hasSeenOpponent(Monster* opponent)const;
    bool isPastEvoLevel()const;
    void setBall(ItemType ball);
    bool hasEvolutions()const;
    bool changeWeatherForm(EventHandler* handler,Weather weather);
};

#endif