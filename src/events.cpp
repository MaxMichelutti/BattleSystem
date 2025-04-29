#include "events.h"

EventHandler::EventHandler() {}
EventHandler::~EventHandler() {}

BattleAction forcedAction(BattleActionActor actor, Battler* active, Field* field){
    if(active->hasVolatileCondition(CHARGING_SOLARBEAM))
        return BattleAction(
            actor,
            SOLAR_BEAM,
            15, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(CHARGING_SOLARBLADE))
        return BattleAction(
            actor,
            SOLAR_BLADE,
            484, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(CHARGING_METEORBEAM))
        return BattleAction(
            actor,
            METEOR_BEAM,
            521, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(CHARGING_SKYATTACK))
        return BattleAction(
            actor,
            SKY_ATTACK,
            380, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(PETAL_DANCING))
        return BattleAction(
            actor,
            PETAL_DANCE,
            17, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(RAGING_FURING))
        return BattleAction(
            actor,
            RAGING_FURY,
            393, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(THRASHING))
        return BattleAction(
            actor,
            THRASH,
            209, 
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(OUTRAGING))
        return BattleAction(
            actor,
            OUTRAGE,
            19,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    if(active->hasVolatileCondition(FLYING_HIGH)){
        return BattleAction(
            actor,
            FLY,
            82,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(BOUNCING)){
        return BattleAction(
            actor,
            BOUNCE,
            422,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(ROLLINGOUT)){
        return BattleAction(
            actor,
            ROLLOUT,
            127,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(UNDERGROUND)){
        return BattleAction(
            actor,
            DIG,
            131,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(UNDERWATER)){
        return BattleAction(
            actor,
            DIVE,
            249,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(VANISHED)){
        return BattleAction(
            actor,
            PHANTOM_FORCE,
            489,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(VANISHED_2)){
        return BattleAction(
            actor,
            SHADOW_FORCE,
            552,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(UPROARING)){
        return BattleAction(
            actor,
            UPROAR,
            279,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(RECHARGING)){
        return BattleAction(
            actor,
            RECHARGE_TURN,
            0,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    if(active->hasVolatileCondition(TRUANTING)){
        return BattleAction(
            actor,
            TRUANT_TURN,
            0,
            0, 
            active->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active->getMonster());
    }
    //default BattleAction is ATTACK action
    return BattleAction();
}





