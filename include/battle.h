#ifndef __BATTLE_H___
#define __BATTLE_H___

#include "common.h"
#include "team.h"
#include "field.h"
#include "battler.h"
#include "events.h"
#include "cpu_ai.h"
#include "util.h"

class CPUAI;
class EventHandler;
class Battler;
class MonsterTeam;
class Attack;
class Field;

struct ScheduledFutureSight{
    unsigned int turns_left;
    unsigned int user_special_attack;
    unsigned int user_level;
    bool stab;
    BattleActionActor target;   
    ScheduledFutureSight(unsigned int turns_left, unsigned int user_special_attack, unsigned int level, BattleActionActor target, bool stab);
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
    int speed;
    unsigned int order;
    public:
    BattleAction();
    BattleAction(BattleActionActor actor,BattleActionType type,unsigned int attack_id, int priority, unsigned int speed, unsigned int switch_id);
    ~BattleAction();

    BattleActionActor getActor()const;
    BattleActionType getActionType()const;
    unsigned int getSwitchId()const;
    unsigned int getAttackId()const;
    int getPriority()const;

    bool operator<=(const BattleAction& other) const;
    bool operator>=(const BattleAction& other) const;
    bool operator<(const BattleAction& other) const;
    bool operator>(const BattleAction& other) const;
    bool operator==(const BattleAction& other) const;

    void setOrder(unsigned int order);
    unsigned int getOrder()const;

    // void applySpeedModifiers(Battler* active_user, Field* field);
};

class Battle{
    private:
    Battler* player_active;
    Battler* opponent_active;
    MonsterTeam* player_team;
    MonsterTeam* opponent_team;
    Field* field;
    CPUAI* cpu_ai;
    EventHandler* event_handler;
    unsigned int last_attack_used_id;
    unsigned int turn;
    unsigned int turn_of_player_last_kill;
    unsigned int turn_of_opponent_last_kill;
    unsigned int rounds_used_this_turn;
    std::set<Monster*> uproaring_monsters;//never dereference this pointers!!!
    std::vector<ScheduledFutureSight> scheduled_futuresights;
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
    void applySwitchInAbilitiesEffects(BattleActionActor actor);
    void applyImpostorSwitchIn(BattleActionActor actor);
    void applyAttackEffect(Attack* attack,BattleActionActor actor);
    void applyContactEffects(Attack* attack,BattleActionActor actor);
    bool checkIfAttackFails(Attack* attack, BattleAction action, BattleAction other_action);
    bool thereIsNeutralizingGas();
    std::string getActorBattlerName(BattleActionActor actor);
    Battler* getActorBattler(BattleActionActor actor);
    MonsterTeam* getActorTeam(BattleActionActor actor);
    void applyRecoil(Attack*,unsigned int,BattleActionActor);
    unsigned int applyDamage(Attack*,BattleActionActor, bool moves_after_target);//returns the actual damage dealt
    bool checkIfMoveMisses(Attack* attack, BattleActionActor actor);//true if move results in miss
    double computePower(Attack* attack, BattleActionActor actor, bool moves_after_target);
    void performAction(BattleAction action, std::vector<BattleAction>& all_actions);
    void performAttack(BattleAction action, std::vector<BattleAction>& all_actions);
    void performSwitch(BattleAction action);
    void performRechargeTurn(BattleAction action);
    unsigned int computeDamage(unsigned int attack_id, BattleActionActor user, bool critical_hit, bool moves_after_target);
    void performEntryHazardCheck(BattleActionActor actor);
    bool thereIsaCloudNine();
    void removeVolatilesFromOpponentOfMonsterLeavingField(BattleActionActor actor_switching_out);
    void forceSwitch(BattleActionActor actor);
    void applyScheduledFutureSights();
    public:
    Battle();
    Battle(unsigned int cpu_skill, EventHandler* handler,MonsterTeam* player_team, MonsterTeam* opponent_team);
    Battle(unsigned int cpu_skill, EventHandler* handler,MonsterTeam* player_team, MonsterTeam* opponent_team, Weather weather, Terrain terrain);
    ~Battle();
    // void addEventHandler(EventHandler* event_handler);
    void startBattle();
    bool isOver()const;
    bool playerWon()const;
    bool opponentWon()const;
    
};

#endif