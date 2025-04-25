#ifndef __BATTLER_H__
#define __BATTLER_H__

#include "common.h"
#include "species.h"
#include "attack.h"
#include "abilities.h"
#include "monster.h"
#include <map>
#include "field.h"
#include "events.h"

class Field;
class AttackData;
class EventHandler;
class Monster;

std::string cannotFallMsg(std::string nickname, std::string stat_name);
std::string cannotRaiseMsg(std::string nickname, std::string stat_name); 
std::string statFallMsg(std::string nickname, std::string stat_name, unsigned int severity);
std::string statRaiseMsg(std::string nickname, std::string stat_name, unsigned int severity); 


bool isVolatileConditionClearedByRapidSpin(VolatileStatusCondition status);

class Modifiers{
    private:
    int attack;
    int defense;
    int special_attack;
    int special_defense;
    int speed;
    int accuracy;
    int evasion;

    public:
    Modifiers();
    ~Modifiers();
    Modifiers(unsigned int attack, unsigned int defense, unsigned int special_attack, unsigned int special_defense, unsigned int speed, unsigned int accuracy, unsigned int evasion);
    bool changeAtk(int amount);
    bool changeDef(int amount);
    bool changeSpatk(int amount);
    bool changeSpdef(int amount);
    bool changeSpd(int amount);
    bool changeAccuracy(int amount);
    bool changeEvasion(int amount);
    bool setAtk(int amount);
    bool setDef(int amount);
    bool setSpatk(int amount);
    bool setSpdef(int amount);
    bool setSpd(int amount);
    bool setAccuracy(int amount);
    bool setEvasion(int amount);
    unsigned int getAttack()const;
    unsigned int getDefense()const;
    unsigned int getSpecialAttack()const;
    unsigned int getSpecialDefense()const;
    unsigned int getSpeed()const;
    unsigned int getAccuracy()const;
    unsigned int getEvasion()const;
};

class Battler{
    private:
    Monster* monster;
    Battler* opponent;
    std::map<VolatileStatusCondition, int> volatile_conditions;
    Modifiers stat_modifiers;
    Ability battler_ability;
    unsigned int bad_poison_counter;
    unsigned int consecutive_protects_counter;
    Field* field;
    BattleActionActor actor;
    std::set<Type> types;
    EventHandler* handler;
    unsigned int last_attack_used;
    unsigned int same_attack_counter;
    unsigned int stockpiles;
    bool has_ability_suppressed;
    bool has_ability_neutralized;
    unsigned int disabled_attack_id;
    unsigned int disabled_turns_left;
    std::set<unsigned int> attacks_used;
    AttackData* mimicked_attack;
    unsigned int turns_in_battle;
    bool last_attack_failed;
    unsigned int hits_taken;
    unsigned int physical_dmg_taken_this_turn;
    unsigned int special_dmg_taken_thi_turn;
    bool is_mold_breaker_active;
    // ItemType consumed_held_item;
    bool had_held_item;
    std::set<Monster*> lock_ons;// never dereference the pointers contained in this set!!!
    unsigned int weight;
    unsigned int height;
    bool had_flying_type;
    Stats battle_stats;//HP FROM THESE STATS MUST NEVER BE USED!!!
    unsigned int substituteHP;

    void displayStatModifyResult(bool result,int amount,std::string stat_name);
    unsigned int getModifiedDefenseInternal()const;
    unsigned int getModifiedSpecialDefenseInternal()const;
    public:
    Battler();
    Battler(Monster* monster,Field*field,BattleActionActor actor, EventHandler*);
    ~Battler();
    Monster* getMonster()const;
    void setMonster(Monster* monster);
    void addVolatileCondition(VolatileStatusCondition condition, int duration);
    void removeVolatileCondition(VolatileStatusCondition condition);
    void decrementVolatileCondition(VolatileStatusCondition condition);
    void removeAllVolatileConditions();
    bool hasVolatileCondition(VolatileStatusCondition condition)const;
    bool clearVolatilesSuchThat(bool (*func)(VolatileStatusCondition));
    bool hasVolatilesSuchThat(bool (*func)(VolatileStatusCondition));
    bool changeAttackModifier(int amount);
    bool changeDefenseModifier(int amount);
    bool changeSpecialAttackModifier(int amount);
    bool changeSpecialDefenseModifier(int amount);
    bool changeSpeedModifier(int amount);
    bool changeAccuracyModifier(int amount);
    bool changeEvasionModifier(int amount);
    bool changeAttackModifierForced(int amount);
    bool changeDefenseModifierForced(int amount);
    bool changeSpecialAttackModifierForced(int amount);
    bool changeSpecialDefenseModifierForced(int amount);
    bool changeSpeedModifierForced(int amount);
    bool changeAccuracyModifierForced(int amount);
    bool changeEvasionModifierForced(int amount);
    bool setAttackModifier(int amount);
    bool setDefenseModifier(int amount);
    bool setSpecialAttackModifier(int amount);
    bool setSpecialDefenseModifier(int amount);
    bool setSpeedModifier(int amount);
    bool setAccuracyModifier(int amount);
    bool setEvasionModifier(int amount);
    int getAttackModifier()const;
    int getDefenseModifier()const;
    int getSpecialAttackModifier()const;
    int getSpecialDefenseModifier()const;
    int getSpeedModifier()const;
    int getAccuracyModifier()const;
    int getEvasionModifier()const;
    unsigned int getConsecutiveProtects()const;
    void incrementConsecutiveProtect();
    void resetConsecutiveProtect();
    unsigned int getModifiedAttack()const;
    unsigned int getModifiedDefense()const;
    unsigned int getModifiedSpecialAttack()const;
    unsigned int getModifiedSpecialDefense()const;
    unsigned int getModifiedSpeed()const;
    unsigned int getModifiedAccuracy()const;
    unsigned int getModifiedEvasion()const;
    void resetAllStatChanges();
    unsigned int getBadPoisonCounter()const;
    void increaseBadPoisonCounter();
    void resetBadPoisonCounter();
    Ability getAbility()const;
    bool hasAbility(Ability ability)const;
    Ability getNonNeutralizedAbility()const;
    void neutralizeAbility();
    void cancelAbilityNeutralization();
    void setAbility(Ability ability);
    bool canSwitchOut(Battler*enemy)const;
    BattleActionActor getActor()const;
    std::set<Type> getTypes()const;
    Type getHiddenPowerType()const;
    bool hasType(Type)const;
    void changeType(Type type);
    void addType(Type type);
    void clearTypes();
    unsigned int getLastAttackUsed()const;
    void setLastAttackUsed(unsigned int attack_id);
    bool hasUsedAttack(unsigned int attack_id)const;
    unsigned int getSameAttackCounter()const;
    bool isTouchingGround()const;
    bool isAsleep()const;
    bool isParalyzed()const;
    bool isFrozen()const;
    bool isBurned()const;
    bool isPoisoned()const;
    bool canFallAsleep()const;
    bool canBeParalyzed()const;
    bool canBeFrozen()const;
    bool canBeBurned()const;
    bool canBePoisoned()const;
    bool canBeBadlyPoisoned()const;
    void landOnGround();
    void leaveGround();
    PermanentStatusCondition getPermanentStatus()const;
    bool hasPermanentStatus()const;
    bool setPermanentStatus(PermanentStatusCondition status);
    bool setPermanentStatusForced(PermanentStatusCondition status);
    bool clearPermanentStatus();
    unsigned int getStockpiles()const;
    bool incrementStockpiles();
    bool resetStockpiles();
    bool isAbilitySuppressed()const;
    void suppressAbility();
    bool isFainted()const;
    bool disableAttack(unsigned int move_id, unsigned int turns);
    bool isAttackDisabled(unsigned int)const;
    unsigned int getDisabledAttack()const;
    bool hasDiabledAttack()const;
    void decrementDisabledTurns();
    bool knowsAttack(unsigned int)const;
    bool hasPreviouslyUsed(unsigned int)const;
    std::map<unsigned int,unsigned int> getAttacks()const;
    void usePP(unsigned int attack_id, unsigned int amount);
    void recoverPP(unsigned int attack_id, unsigned int amount);
    bool hasPP(unsigned int attack_id)const;
    bool setMimickedAttack(unsigned int attack_id);
    bool hasMimickedAttack()const;
    unsigned int getMaxPPForAttack(unsigned int attack_id)const;
    void transformInto(Monster* other);
    std::pair<unsigned int,bool> addDamage(unsigned int amount, AttackType category, float effectiveness, bool is_sound);
    unsigned int addDirectDamage(unsigned int amount);
    unsigned int getCurrentHP()const;
    unsigned int getMaxHP()const;
    unsigned int getTurnsInBattle()const;
    void nextTurn();
    bool isFirstTurn()const;
    unsigned int getHeight()const;
    unsigned int getWeight()const;
    void changeWeight(int amount);
    void changeHeight(int amount);
    bool lastAttackFailed()const;
    void setLastAttackFailed();
    void setLastAttackHit();
    void hitOnceMore(Type attack_type);
    unsigned int numberOfHitsTaken()const;
    std::string getNickname()const;
    unsigned int removeDamage(unsigned int amount);
    unsigned int getLevel()const;
    Stats getStats()const;
    Gender getGender()const;
    bool isAtFullHP()const;
    unsigned int getPhysicalDamageTakenThisTurn()const;
    unsigned int getSpecialDamageTakenThisTurn()const;
    void setPhysicalDamageTakenThisTurn(unsigned int);
    void setSpecialDamageTakenThisTurn(unsigned int);
    void resetDamageTakenThisTurn();
    void addLockOn(Monster* monster);
    void removeLockOn(Monster* monster);
    bool hasLockOn(Monster* monster)const;
    void activateMoldBreaker();
    bool isMoldBreakerActive()const;
    void deactivateMoldBreaker();
    bool useItem(ItemType item,unsigned int data);
    bool consumeHeldItem();
    bool itemWouldHaveEffect(ItemType item)const;
    bool hasHeldItem()const;
    ItemType getHeldItem()const;
    ItemType getConsumedItem()const;
    bool hasHeldItem(ItemType item)const;
    ItemType setHeldItem(ItemType item);
    ItemType removeHeldItem();
    bool hasConsumedBerry()const;
    bool hasConsumedItem()const;
    bool canStealItem()const;
    void consumeItem(ItemType item);
    bool restoreBerry();
    bool restoreItem();
    bool hasLostHeldItem()const;
    void tryEatStartOfTurnBerry();
    void tryEatLowHPBerry();
    void tryEatStatusBerry();
    void tryEatLeppaBerry(unsigned int attack_id);
    void tryEatAfterGettingHitBerry(AttackType category, float effectiveness, Battler* attacker);
    bool tryEatSuperEffectiveBerry(Type attack_type, bool is_supereffective);
    bool ismegaEvolved()const;
    bool megaEvolve();
    bool canMegaEvolve()const;
    Stats getBattleStats()const;
    void setBattleAttack(unsigned int attack);
    void setBattleDefense(unsigned int defense);
    void setBattleSpecialAttack(unsigned int special_attack);
    void setBattleSpecialDefense(unsigned int special_defense);
    void setBattleSpeed(unsigned int speed);
    void addSeenOpponent(Monster* opponent);
    bool canConsumeWhiteHerb()const;
    void removeDisable();
    void setOpponent(Battler* opponent);
    bool hasSubstitute()const;
    void setSubstituteHP(unsigned int amount);
    void removeSubstitute();
};

#endif