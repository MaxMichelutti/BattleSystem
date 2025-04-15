#ifndef __FIELD_H___
#define __FIELD_H___

#include "common.h"
#include "events.h"
#include <map>
#include <set>

class EventHandler;

std::string terrainToString(Terrain terrain);

bool isFieldEffectClearedByRapidSpin(FieldEffect field_effect);
bool isFieldEffectRemovedAtSwitch(FieldEffect field_effect);
bool isFieldEffectTrapping(FieldEffect field_effect);

class Field{
    private:
    Weather weather;
    Terrain terrain;
    EventHandler* event_handler;
    std::map<FieldEffect,unsigned int> field_effect_player;
    std::map<FieldEffect,unsigned int> field_effect_opponent;
    std::map<FieldEffect,unsigned int> full_field_effects;
    unsigned int turns_to_clear_weather;
    unsigned int turns_to_clear_terrain;
    public:
    Field();
    Field(EventHandler* event_handler);
    ~Field();
    Field(Weather weather, Terrain terrain);
    Weather getWeather()const;
    Terrain getTerrain()const;
    std::set<FieldEffect> getFieldEffects(BattleActionActor)const;
    std::set<FieldEffect> getFieldEffectsPlayer()const;
    std::set<FieldEffect> getFieldEffectsOpponent()const;
    void setWeather(Weather weather);
    void setWeather(Weather weather,unsigned int);
    void setTerrain(Terrain terrain);
    void setTerrain(Terrain terrain,unsigned int);
    void clearWeather();
    void clearTerrain();
    void clearFieldSide(BattleActionActor actor);
    bool setFullFieldEffect(FieldEffect field_effect, unsigned int turns);
    void clearFullFieldEffect(FieldEffect field_effect);
    bool hasFullFieldEffect(FieldEffect field_effect)const;
    void setFieldEffect(FieldEffect field_effect, unsigned int turns, BattleActionActor actor);
    void setFieldEffectPlayer(FieldEffect field_effect, unsigned int turns);
    void setFieldEffectOpponent(FieldEffect field_effect, unsigned int turns);
    bool hasFieldEffect(FieldEffect field_effect, BattleActionActor actor)const;
    void clearFieldEffect(FieldEffect field_effect, BattleActionActor actor);
    void clearFieldEffectPlayer();
    void clearFieldEffectOpponent();
    void nextTurnWeather();
    void nextTurnTerrain();
    void nextTurnFieldEffect();
    bool clearFieldEffectsSuchThat(bool (*func)(FieldEffect), BattleActionActor);
    bool hasFieldEffectsSuchThat(bool (*func)(FieldEffect), BattleActionActor)const;
};

#endif