#include "field.h"

// Terrain --------------------------------------------------------------------------------------------------------------------

std::string terrainToString(Terrain terrain) {
    switch (terrain) {
        case NO_TERRAIN: return "No Terrain";
        case GRASSY_FIELD: return "Grassy Terrain";
        case MISTY_FIELD: return "Misty Terrain";
        case ELECTRIC_FIELD: return "Electric Terrain";
        case PSYCHIC_FIELD: return "Psychic Terrain";
        default: return "Unknown Terrain";
    }
}

// Field Effect ------------------------------------------------------------------------------------------------------------

bool isFieldEffectClearedByRapidSpin(FieldEffect field_effect) {
    switch (field_effect) {
        case TOXIC_SPIKES:
        case BAD_TOXIC_SPIKES:
        case SPIKES:
        case SPIKES_2:
        case SPIKES_3:
        case STEALTH_ROCKS:
            return true;
        default:
            return false;
    }
}

bool isFieldEffectRemovedAtSwitch(FieldEffect field_effect){
    switch (field_effect) {
        default:
            return false;
    }
}

bool isFieldEffectTrapping(FieldEffect field_effect) {
    switch (field_effect) {
        default:return false;
    }
}

// Field ------------------------------------------------------------------------------------------------------------

Field::Field() {
    weather = CLEAR;
    terrain = NO_TERRAIN;
    turns_to_clear_weather = 0;
    turns_to_clear_terrain = 0;
}

Field::Field(EventHandler* event_handler) {
    weather = CLEAR;
    terrain = NO_TERRAIN;
    turns_to_clear_weather = 0;
    turns_to_clear_terrain = 0;
    this->event_handler = event_handler;
}
Field::~Field() {
    
}

Field::Field(Weather weather, Terrain terrain) {
    this->weather = weather;
    this->terrain = terrain;
    turns_to_clear_weather = 0;
    turns_to_clear_terrain = 0;
}

Weather Field::getWeather()const {
    return weather;
}

Terrain Field::getTerrain()const {
    return terrain;
}

std::set<FieldEffect> Field::getFieldEffectsPlayer()const {
    std::set<FieldEffect> field_effects;
    for(auto it = field_effect_player.begin(); it != field_effect_player.end(); it++){
        field_effects.insert(it->first);
    }
    return field_effects;
}

std::set<FieldEffect> Field::getFieldEffectsOpponent()const {
    std::set<FieldEffect> field_effects;
    for(auto it = field_effect_opponent.begin(); it != field_effect_opponent.end(); it++){
        field_effects.insert(it->first);
    }
    return field_effects;
}

std::set<FieldEffect> Field::getFieldEffects(BattleActionActor actor)const{
    if(actor==PLAYER)
        return getFieldEffectsPlayer();
    else
        return getFieldEffectsOpponent();
}

void Field::setWeather(Weather new_weather){
    setWeather(new_weather,5);
}

void Field::setWeather(Weather new_weather, unsigned int length) {
    this->weather = new_weather;
    switch(new_weather){
        case RAIN:
            event_handler->displayMsg("It started to rain!");
            break;
        case SUN:
            event_handler->displayMsg("The sun is shining harshly!");
            break;
        case HAIL:
            event_handler->displayMsg("Hail started to fall!");
            break;
        case SANDSTORM:
            event_handler->displayMsg("A sandstorm started!");
            break;
        case SNOWSTORM:
            event_handler->displayMsg("A snowstorm started!");
            break;
        default:break;
    }
    turns_to_clear_weather = length;
}

void Field::setTerrain(Terrain terrain) {
    setTerrain(terrain,5);
}

void Field::setTerrain(Terrain terrain, unsigned int length) {
    if(terrain != this->terrain){
        this->terrain = terrain;
        turns_to_clear_terrain = length;
        switch(terrain){
            case ELECTRIC_FIELD:
                event_handler->displayMsg("The terrain is full of electricity!");
                break;
            case GRASSY_FIELD:
                event_handler->displayMsg("The Grass grows lush on the terrain!");
                break;
            case MISTY_FIELD:
                event_handler->displayMsg("A Misty clound covers the terrain!");
                break;
            case PSYCHIC_FIELD:
                event_handler->displayMsg("A Psychic field covers the terrain!");
                break;
            default:break;
        }
    }
}

void Field::setFieldEffectPlayer(FieldEffect field_effect,unsigned int turns) {
    field_effect_player[field_effect] = turns;
}

void Field::setFieldEffectOpponent(FieldEffect field_effect,unsigned int turns) {
    field_effect_opponent[field_effect] = turns;
}

void Field::setFieldEffect(FieldEffect field_effect, unsigned int turns, BattleActionActor actor) {
    if(actor == PLAYER){
        setFieldEffectPlayer(field_effect, turns);
    }else{
        setFieldEffectOpponent(field_effect, turns);
    }
}

bool Field::hasFieldEffect(FieldEffect field_effect, BattleActionActor actor)const {
    if(actor == PLAYER){
        return field_effect_player.find(field_effect) != field_effect_player.end();
    }else{
        return field_effect_opponent.find(field_effect) != field_effect_opponent.end();
    }
}

void Field::clearFieldEffect(FieldEffect field_effect, BattleActionActor actor) {
    std::string field_side_owner;
    if(!hasFieldEffect(field_effect,actor))
        return;
    if(actor == PLAYER){
        field_effect_player.erase(field_effect);
        field_side_owner = "Player";
    }else{
        field_effect_opponent.erase(field_effect);
        field_side_owner = "Opponent";
    }
    switch(field_effect){
        case SAFEGUARD:
            event_handler->displayMsg(field_side_owner+"'s side is no longer protected by safeguard!");
            break;
        case TAILWIND:
            event_handler->displayMsg("The strong wind stopped blowing on "+field_side_owner+"'s side!");
            break;
        case LIGHT_SCREEN:
            event_handler->displayMsg(field_side_owner+"'s side is no longer protected by light screen!");
            break;
        case REFLECT:
            event_handler->displayMsg(field_side_owner+"'s side is no longer protected by reflect!");
            break;
        case MIST:
            event_handler->displayMsg("Mist dissolved from "+field_side_owner+"'s side!");
            break;
        default:break;
    }
}

bool Field::hasFieldEffectsSuchThat(bool (*func)(FieldEffect), BattleActionActor actor)const{
    if(actor==PLAYER){
        for(auto it=field_effect_player.begin();it!=field_effect_player.end();it++){
            if(func(it->first))
                return true;
        }
    }else{
        for(auto it=field_effect_opponent.begin();it!=field_effect_opponent.end();it++){
            if(func(it->first))
                return true;
        }
    }   
    return false;
}

void Field::nextTurnWeather() {
    if(turns_to_clear_weather > 0){
        turns_to_clear_weather--;
        if(turns_to_clear_weather == 0)
            clearWeather();
        else{
            switch(weather){
                case RAIN:
                    event_handler->displayMsg("Rain continues to fall!");
                    break;
                case SUN:
                    event_handler->displayMsg("The sun continues to shine harshly!");
                    break;
                case HAIL:
                    event_handler->displayMsg("Hail continues to fall!");
                    break;
                case SANDSTORM:
                    event_handler->displayMsg("The sandstorm rages!");
                    break;
                case SNOWSTORM:
                    event_handler->displayMsg("Snow continues to fall!");
                    break;
                default:break;
            }
        }
    }
}
void Field::nextTurnTerrain() {
    if(turns_to_clear_terrain > 0){
        turns_to_clear_terrain--;
        if(turns_to_clear_terrain == 0)
            clearTerrain();
    }
}
void Field::nextTurnFieldEffect() {
    std::set<FieldEffect> field_effects_to_clear;
    // full field
    for(auto it = full_field_effects.begin(); it != full_field_effects.end(); it++){
        if(it->second < 0)
            continue;
        it->second--;
        if(it->second == 0)
            field_effects_to_clear.insert(it->first);
    }
    for(auto it = field_effects_to_clear.begin(); it != field_effects_to_clear.end(); it++){
        clearFullFieldEffect(*it);
    }
    // player side
    for(auto it = field_effect_player.begin(); it != field_effect_player.end(); it++){
        if(it->second < 0)
            continue;
        it->second--;
        if(it->second == 0)
            field_effects_to_clear.insert(it->first);
    }
    for(auto it = field_effects_to_clear.begin(); it != field_effects_to_clear.end(); it++){
        clearFieldEffect(*it, PLAYER);
    }
    // opponent side
    field_effects_to_clear.clear();
    for(auto it = field_effect_opponent.begin(); it != field_effect_opponent.end(); it++){
        if(it->second < 0)
            continue;
        it->second--;
        if(it->second == 0)
            field_effects_to_clear.insert(it->first);
    }
    for(auto it = field_effects_to_clear.begin(); it != field_effects_to_clear.end(); it++){
        clearFieldEffect(*it, OPPONENT);
    }
}

void Field::clearWeather() {
    if(weather != CLEAR){
        weather = CLEAR;
        event_handler->displayMsg("The weather cleared!");
    }
}

void Field::clearTerrain() {
    if(terrain != NO_TERRAIN){
        event_handler->displayMsg(terrainToString(terrain)+" no longer affects the field!");
        terrain = NO_TERRAIN;
    }
}

void Field::clearFieldEffectPlayer() {
    field_effect_player.clear();
}

void Field::clearFieldEffectOpponent() {
    field_effect_opponent.clear();
}

bool Field::clearFieldEffectsSuchThat(bool (*func)(FieldEffect),BattleActionActor actor){
    std::set<FieldEffect> field_effects_to_clear;
    if(actor==PLAYER){
        for(auto it = field_effect_player.begin(); it != field_effect_player.end(); it++){
            if(func(it->first))
                field_effects_to_clear.insert(it->first);
        }
    }else{
        for(auto it = field_effect_opponent.begin(); it != field_effect_opponent.end(); it++){
            if(func(it->first))
                field_effects_to_clear.insert(it->first);
        }
    }
    for(auto it = field_effects_to_clear.begin(); it != field_effects_to_clear.end(); it++){
        clearFieldEffect(*it, actor);
    }
    return true;
}


bool Field::setFullFieldEffect(FieldEffect field_effect, unsigned int turns){
    if(hasFullFieldEffect(field_effect)){
        return false;
    }
    full_field_effects[field_effect] = turns;
    switch(field_effect){
        case GRAVITY:
            event_handler->displayMsg("Gravity intensified!");
            break;
        case WONDER_ROOM:
            event_handler->displayMsg("A dimension where Defense and Special defense are swapped was created!");
            break;
        default:break;
    }
    return true;
}
void Field::clearFullFieldEffect(FieldEffect field_effect){
    full_field_effects.erase(field_effect);
    switch(field_effect){
        case GRAVITY:
            event_handler->displayMsg("The gravity returned back to normal!");
            break;
        case WONDER_ROOM:
            event_handler->displayMsg("The dimension where Defense and Special defense are swapped returned back to normal!");
            break;
        default:break;
    }
}
bool Field::hasFullFieldEffect(FieldEffect field_effect)const{
    return full_field_effects.find(field_effect) != full_field_effects.end();
}

void Field::clearFieldSide(BattleActionActor actor){
    // be careful not clearing tailwind from the side
    bool had_tailwind = hasFieldEffect(TAILWIND, actor);
    unsigned int tailwind_turns;
    if(actor == PLAYER){
        if(had_tailwind)
            tailwind_turns = field_effect_player[TAILWIND];
        field_effect_player.clear();
        if(had_tailwind)
            field_effect_player[TAILWIND] = tailwind_turns;
    }else{
        if(had_tailwind)
            tailwind_turns = field_effect_opponent[TAILWIND];
        field_effect_opponent.clear();
        if(had_tailwind)
            field_effect_opponent[TAILWIND] = tailwind_turns;
    }
}