#ifndef __BATTLE_H___
#define __BATTLE_H___

#include "common.h"
#include "abilities.h"
#include "team.h"
#include "field.h"
#include "battler.h"
#include "events.h"
#include "cpu_ai.h"
#include "util.h"
#include "items.h"
#include "bag.h"
#include "monster.h"

class CPUAI;
class EventHandler;
class Battler;
class MonsterTeam;
class Attack;
class Field;
class ItemData;
class Bag;
class Monster;

using StatCV = std::vector<std::pair<unsigned int,int>>;

struct ScheduledFutureSight{
    unsigned int attack_id;
    unsigned int turns_left;
    unsigned int user_special_attack;
    unsigned int user_level;
    bool stab;
    BattleActionActor target;   
    ScheduledFutureSight(unsigned int attack_id,unsigned int turns_left, unsigned int user_special_attack, unsigned int level, BattleActionActor target, bool stab);
    ~ScheduledFutureSight();
};

BattleActionActor otherBattleActionActor(BattleActionActor actor);

bool isAttackingActionType(BattleActionType action_type);

class BattleAction{
    private:
    BattleActionActor actor;
    BattleActionType action_type;
    unsigned int switch_id;
    unsigned int attack_id;
    int priority;
    unsigned int speed;
    unsigned int order;
    ItemType item_to_use;
    bool mega_evolution;
    Monster* monster_to_act;
    public:
    BattleAction();
    BattleAction(BattleActionActor actor,
        BattleActionType type,
        unsigned int attack_id, 
        int priority, 
        unsigned int speed, 
        unsigned int switch_id,
        ItemType item_to_use,
        bool mega_evolution,
        Monster* monster_to_act);
    ~BattleAction();

    BattleActionActor getActor()const;
    BattleActionType getActionType()const;
    unsigned int getSwitchId()const;
    unsigned int getAttackId()const;
    int getPriority()const;
    ItemType getItemToUse()const;
    void setSpeed(unsigned int speed);
    void setPriority(int priority);
    bool isMega()const;
    Monster* getMonsterToAct()const;

    bool operator<=(const BattleAction& other) const;
    bool operator>=(const BattleAction& other) const;
    bool operator<(const BattleAction& other) const;
    bool operator>(const BattleAction& other) const;
    bool operator==(const BattleAction& other) const;

    void setOrder(unsigned int order);
    unsigned int getOrder()const;
    unsigned int getSpeed()const;

    // void applySpeedModifiers(Battler* active_user, Field* field);
};

class Battle{
    private:
    Battler* player_active;
    Battler* opponent_active;
    MonsterTeam* player_team;
    MonsterTeam* opponent_team;
    Bag* player_bag;
    Bag* opponent_bag;
    Field* field;
    CPUAI* cpu_ai;
    EventHandler* event_handler;
    unsigned int last_attack_used_id;
    unsigned int turn;
    unsigned int turn_of_player_last_kill;
    unsigned int turn_of_opponent_last_kill;
    unsigned int rounds_used_this_turn;
    ItemType item_on_the_ground_player;
    ItemType item_on_the_ground_opponent;
    std::set<Monster*> uproaring_monsters;//never dereference this pointers!!!
    std::vector<ScheduledFutureSight> scheduled_futuresights;
    bool is_wild_battle;
    bool battle_gives_exp;
    bool caught_wild_monster;
    bool is_wild_battle_over;
    std::set<Monster*> monsters_defeated_by_player;
    std::set<Monster*> monsters_defeated_by_opponent;
    unsigned int runaway_attempts;
    unsigned int money;
    std::map<Battler*,BattleAction> actions;

    void init(unsigned int cpu_skill, EventHandler* handler, MonsterTeam* player_team, MonsterTeam* opponent_team, Bag * user_bag, Bag* opponent_bag, Weather weather, Terrain terrain);
    void checkUproars();
    void incrementTurn();
    void performTurn();
    void decrementVolatiles(Battler*);
    void forgetMoveVolatiles(Battler*);
    void applyPostDamage();
    bool isUproar();
    void applyPermanentStatusPostDamage(BattleActionActor actor);
    void applyVolatileStatusPostDamage(BattleActionActor actor);
    void applyWeatherPostDamage(BattleActionActor actor);
    void applyFieldEffectsPostDamage(BattleActionActor actor);
    void applyTerrainPostDamage(BattleActionActor actor);
    void applyAbilityPostDamage(BattleActionActor actor);
    void applyItemPostDamage(BattleActionActor actor);
    bool applySwitchInAbilitiesEffects(Battler* user_active);
    bool applySwitchInItemsEffects(BattleActionActor actor);
    void applySwitchInFormChange(BattleActionActor actor);
    void applyAttackEffect(Attack* attack,BattleActionActor actor, BattleActionActor other_actor, bool hits_substitute);
    bool applyContactEffects(Attack* attack,BattleActionActor actor,bool makes_contact);
    bool checkIfAttackFails(Attack* attack, BattleAction action, BattleAction other_action);
    bool thereIsAbility(Ability ability);
    void checkMonsterLeavingAbilities(BattleActionActor actor);
    void resetOpponents();
    std::string getActorBattlerName(BattleActionActor actor);
    Battler* getActorBattler(BattleActionActor actor);
    MonsterTeam* getActorTeam(BattleActionActor actor);
    void applyRecoil(Attack*,unsigned int,BattleActionActor);
    std::pair<unsigned int,bool> applyDamage(Attack*,BattleActionActor, bool moves_after_target, double effectiveness, bool acts_second);//returns the actual damage dealt
    bool checkIfMoveMisses(Attack* attack, BattleActionActor actor, bool acts_second);//true if move results in miss
    double computePower(Attack* attack, BattleActionActor actor, bool moves_after_target,unsigned int beat_up_index);
    void performAction(BattleAction action, std::vector<BattleAction>& all_actions);
    void performAttack(BattleAction action, std::vector<BattleAction>& all_actions);
    void performSwitch(BattleAction action);
    void performUseItem(BattleAction action);
    void performRechargeTurn(BattleAction action);
    void performTruantTurn(BattleAction action);
    unsigned int computeDamage(unsigned int attack_id, BattleActionActor user, bool critical_hit, bool moves_after_target,float effectiveness,AttackType category,unsigned int beat_up_index);
    bool performEntryHazardCheck(BattleActionActor actor);
    bool thereIsaCloudNine();
    void removeVolatilesFromOpponentOfMonsterLeavingField(BattleActionActor actor_switching_out);
    void applyScheduledFutureSights();
    void addMoney(unsigned int money);
    void givePlayerExperience(Monster* defeated_monster);
    void checkForExp();
    void tryToCatchWildMonster(ItemType item);
    // void consumeSeeds();
    void performEscape(BattleAction action);
    bool tryEjectPack(BattleActionActor actor);
    void changeStats(BattleActionActor actor,StatCV& changes,bool forced);//true if eject pack activated
    void checkRoomService();
    bool isCriticalHit(Attack* attack, BattleActionActor user_actor, BattleActionActor target_actor);
    void applyBattleActionModifiers();
    double computeEffectiveness(Attack* attack, BattleActionActor user_actor, BattleActionActor target_actor);
    void performMegaEvolutions();
    void checkZenModes();
    bool onTerrainChange(BattleActionActor actor);
    std::vector<Battler*> getBattlersSortedBySpeed();
    public:
    Battle();
    Battle(unsigned int cpu_skill, EventHandler* handler,MonsterTeam* player_team, MonsterTeam* opponent_team, Bag * user_bag, Bag* opponent_bag);
    Battle(unsigned int cpu_skill, EventHandler* handler,MonsterTeam* player_team, MonsterTeam* opponent_team, Weather weather, Terrain terrain, Bag * user_bag, Bag* opponent_bag);
    ~Battle();
    // void addEventHandler(EventHandler* event_handler);
    void startBattle();
    bool isOver()const;
    bool playerWon()const;
    bool opponentWon()const;
    unsigned int getMoney()const;
    void setWild();
    void setBattleGivesExp();
    void onWeatherChange(Weather weather);
    void onTerrainChange();
    void forceSwitch(BattleActionActor actor);
};

#endif