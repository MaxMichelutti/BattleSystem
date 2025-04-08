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
            0);
    if(active->hasVolatileCondition(CHARGING_SKYATTACK))
        return BattleAction(
            actor,
            SKY_ATTACK,
            380, 
            0, 
            active->getModifiedSpeed(), 
            0);
    if(active->hasVolatileCondition(PETAL_DANCING))
        return BattleAction(
            actor,
            PETAL_DANCE,
            17, 
            0, 
            active->getModifiedSpeed(), 
            0);
    if(active->hasVolatileCondition(THRASHING))
        return BattleAction(
            actor,
            THRASH,
            209, 
            0, 
            active->getModifiedSpeed(), 
            0);
    if(active->hasVolatileCondition(OUTRAGING))
        return BattleAction(
            actor,
            OUTRAGE,
            19,
            0, 
            active->getModifiedSpeed(), 
            0);
    if(active->hasVolatileCondition(FLYING_HIGH)){
        return BattleAction(
            actor,
            FLY,
            82,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    if(active->hasVolatileCondition(ROLLINGOUT)){
        return BattleAction(
            actor,
            ROLLOUT,
            127,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    if(active->hasVolatileCondition(UNDERGROUND)){
        return BattleAction(
            actor,
            DIG,
            131,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    if(active->hasVolatileCondition(UNDERWATER)){
        return BattleAction(
            actor,
            DIVE,
            249,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    if(active->hasVolatileCondition(UPROARING)){
        return BattleAction(
            actor,
            UPROAR,
            279,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    if(active->hasVolatileCondition(RECHARGING)){
        return BattleAction(
            actor,
            RECHARGE_TURN,
            0,
            0, 
            active->getModifiedSpeed(), 
            0);
    }
    return BattleAction();
}





