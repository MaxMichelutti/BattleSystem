#include "battle.h"

// BattleActionActor ------------------------------------------------------------------------------------------------------------

BattleActionActor otherBattleActionActor(BattleActionActor actor) {
    if(actor == PLAYER)
        return OPPONENT;
    else
        return PLAYER;
}

// BattleActionType ------------------------------------------------------------------------------------------------------------

bool isAttackingActionType(BattleActionType action_type){
    switch(action_type){
        case ATTACK:
        case PETAL_DANCE:
        case RAGING_FURY:
        case OUTRAGE:
        case THRASH:
        case SOLAR_BEAM:
        case SOLAR_BLADE:
        case FLY:
        case ROLLOUT:
        case BOUNCE:
        case PHANTOM_FORCE:
        case SHADOW_FORCE:
        case METEOR_BEAM:
        case DIG:
        case DIVE:
        case UPROAR:
        case SKY_ATTACK:
            return true;
        default:
            return false;
    }
}

// ScheduledFutureSight ------------------------------------------------------------------------------------------------------------
ScheduledFutureSight::ScheduledFutureSight(unsigned int attack_id,
        unsigned int turns_left, 
        unsigned int user_special_attack, 
        unsigned int level,
        BattleActionActor target,
        bool stab) {
    this->attack_id = attack_id;
    this->turns_left = turns_left;
    this->user_level = level;
    this->user_special_attack = user_special_attack;
    this->target = target;
    this->stab = stab;
}
ScheduledFutureSight::~ScheduledFutureSight() {}

// BattleAction ------------------------------------------------------------------------------------------------------------
BattleAction::BattleAction() {
    action_type = ATTACK;
    switch_id = 0;
    attack_id = 0;
    priority = 0;
    speed = 0;
    actor = PLAYER;
    monster_to_act = nullptr;
}

BattleAction::BattleAction(BattleActionActor actor,BattleActionType type, unsigned int attack_id, 
    int priority, unsigned int speed, unsigned int switch_id, ItemType item_to_use,
    bool mega_evolution, Monster* monster_to_act) {
    this->item_to_use = item_to_use;
    this->action_type = type;
    this->actor = actor;
    this->attack_id = attack_id;
    this->priority = priority;
    this->speed = speed;
    this->switch_id = switch_id;
    this->mega_evolution = mega_evolution;
    this->monster_to_act = monster_to_act;
    if(action_type == SWITCH)
        this->priority = 10;
    else if(action_type == USE_ITEM)
        this->priority = 20;
    else if(action_type == ESCAPE)
        this->priority = 100;
}
BattleAction::~BattleAction() {}

bool BattleAction::operator==(const BattleAction& other) const {
    return (speed == other.speed) && (priority == other.priority);
}

unsigned int BattleAction::getSpeed()const{
    return speed;
}

void BattleAction::setSpeed(unsigned int speed){
    this->speed = speed;
}

bool BattleAction::isMega()const{
    return mega_evolution;
}

void BattleAction::setPriority(int priority){
    this->priority = priority;
}

ItemType BattleAction::getItemToUse()const {
    return item_to_use;
}

bool BattleAction::operator<=(const BattleAction& other) const {
    if(*this == other)
        return true;
    else
        return (*this) < other;
}

Monster* BattleAction::getMonsterToAct()const{
    return monster_to_act;
}

int BattleAction::getPriority()const {
    return priority;
}

bool BattleAction::operator>=(const BattleAction& other) const {
    return !(*this < other);
}

bool BattleAction::operator>(const BattleAction& other) const {
    return !(*this <= other);
}

// the "BIGGER" action is the first to be execyted
bool BattleAction::operator<(const BattleAction& other) const {
    if(priority != other.priority)
        return priority < other.priority;
    else if(speed != other.speed)
        return speed < other.speed;
    return false;
}

BattleActionActor BattleAction::getActor()const {
    return actor;
}
BattleActionType BattleAction::getActionType()const {
    return action_type;
}
unsigned int BattleAction::getSwitchId()const {
    return switch_id;
}
unsigned int BattleAction::getAttackId()const {
    return attack_id;
}

unsigned int BattleAction::getOrder()const {
    return order;
}
void BattleAction::setOrder(unsigned int order) {
    this->order = order;
}

// void BattleAction::applySpeedModifiers(Battler* active_user, Field* field) {
//     // apply speed modifiers
//     if(active_user->getAbility() == CHLOROPHYL && field->getWeather() == SUN){ // CHLOROPHYL
//         speed *= 2;
//     }
// }

// Battle ------------------------------------------------------------------------------------------------------------

Battle::Battle() {
    player_active = nullptr;
    opponent_active = nullptr;
    player_team = nullptr;
    opponent_team = nullptr;
    turn = 0;
}

Battle::Battle(unsigned int cpu_skill, EventHandler* handler,
        MonsterTeam* player_team, MonsterTeam* opponent_team,
        Bag * user_bag, Bag* opponent_bag) {
    init(cpu_skill, handler, player_team, opponent_team, user_bag, opponent_bag,CLEAR,NO_TERRAIN);
}

Battle::Battle(unsigned int cpu_skill, EventHandler* handler,
        MonsterTeam* player_team, MonsterTeam* opponent_team,
        Weather weather, Terrain terrain,
        Bag * user_bag, Bag* opponent_bag) {
    init(cpu_skill, handler, player_team, opponent_team, user_bag, opponent_bag,weather,terrain);
}

void Battle::init(unsigned int cpu_skill, EventHandler* handler, 
        MonsterTeam* player_team, MonsterTeam* opponent_team, 
        Bag * user_bag, Bag* opponent_bag,
        Weather weather, Terrain terrain) {
    this->event_handler = handler;
    this->cpu_ai = new CPUAI(cpu_skill);
    this->player_team = new MonsterTeam(*player_team);
    this->opponent_team = new MonsterTeam(*opponent_team);
    field = new Field(this,handler);
    if(player_team->getActiveMonster() == nullptr){
        std::cerr << "Error: Player team has no alive monster!" << std::endl;
        exit(NO_ALIVE_ERROR);
    }
    if(opponent_team->getActiveMonster() == nullptr){
        std::cerr << "Error: Opponent team has no alive monster!" << std::endl;
        exit(NO_ALIVE_ERROR);
    }
    player_active = new Battler(player_team->getActiveMonster(),player_team,field,PLAYER,handler);
    opponent_active = new Battler(opponent_team->getActiveMonster(),opponent_team,field,OPPONENT,handler);
    field->setDefaultWeather(weather);
    field->setWeather(weather);
    field->setTerrain(terrain,-1);
    turn = 0;
    last_attack_used_id = 0;
    rounds_used_this_turn = 0;
    turn_of_opponent_last_kill = -2;
    turn_of_player_last_kill = -2;
    this->player_bag = user_bag;
    this->opponent_bag = opponent_bag;
    item_on_the_ground_player = NO_ITEM_TYPE;
    item_on_the_ground_opponent = NO_ITEM_TYPE;
    battle_gives_exp = false;
    is_wild_battle = false;
    caught_wild_monster = false;
    is_wild_battle_over = false;
    runaway_attempts = 0;
    monsters_defeated_by_opponent = std::set<Monster*>();
    monsters_defeated_by_player = std::set<Monster*>();
    actions = std::map<Battler*,BattleAction>();
    uproaring_monsters = std::set<Monster*>();
    scheduled_futuresights = std::vector<ScheduledFutureSight>();
}

Battle::~Battle() {
    #ifdef DEBUG
    std::cout<<"Deleting battle"<<std::endl;
    std::cout.flush();
    #endif
    delete player_active;
    delete opponent_active;
    delete field;
}

void Battle::startBattle(){
    event_handler->displayMsg("Turn "+std::to_string(turn+1));
    #ifdef DEBUG
    std::cout<<"Starting battle"<<std::endl;
    std::cout.flush();
    #endif
    // unsigned int speed_player = player_active->getModifiedSpeed();
    // unsigned int speed_opponent = opponent_active->getModifiedSpeed();
    player_active->addSeenOpponent(opponent_active->getMonster());
    resetOpponents();
    checkZenModes();
    #ifdef DEBUG
    std::cout<<"Applying enter battle effects"<<std::endl;
    std::cout.flush();
    #endif
    for(auto battler: getBattlersSortedBySpeed()){
        applySwitchInFormChange(battler->getActor());
    }
    for(auto battler: getBattlersSortedBySpeed()){
        onTerrainChange(battler->getActor());
    }
    for(auto battler: getBattlersSortedBySpeed()){
        applySwitchInAbilitiesEffects(battler);
    }
    for(auto battler: getBattlersSortedBySpeed()){
        applySwitchInItemsEffects(battler->getActor());
    }

    while(!isOver()){
        performTurn();
        incrementTurn();
    }
    if(playerWon()){
        // std::cout<<"Player won!" << std::endl;
        event_handler->displayMsg("Player won!");
    }else{
        // std::cout<<"Opponent won!" << std::endl;
        event_handler->displayMsg("Opponent won!");
    }
    player_team->clearBattleEffectsAll();
    opponent_team->clearBattleEffectsAll();
}

void Battle::incrementTurn(){
    turn++;
    rounds_used_this_turn = 0;
    #ifdef DEBUG
    std::cout<<"Decrementing Disable"<<std::endl;
    std::cout.flush();
    #endif
    // decrement disable effect
    for(auto battler: getBattlersSortedBySpeed()){
        battler->decrementDisabledTurns();
        battler->leaveGround();
        battler->removeVolatileCondition(CHARGED_2);
        battler->removeVolatileCondition(ENDURE);
        battler->removeVolatileCondition(STAT_JUST_DROPPED);
        battler->removeVolatileCondition(STAT_JUST_RAISED);
        battler->removeVolatileCondition(FOCUSED);
        battler->removeVolatileCondition(MAGIC_COATED);   
        //apply cud chew
        if(battler->hasVolatileCondition(JUST_EATEN_BERRY)){
            battler->decrementVolatileCondition(JUST_EATEN_BERRY);
            if(!battler->hasVolatileCondition(JUST_EATEN_BERRY)&&
                battler->hasAbility(CUD_CHEW)){
                battler->useItem(battler->getConsumedItem(),0);
            }
        }
        if(battler->hasVolatileCondition(HEAL_BLOCKED)){
            battler->decrementVolatileCondition(HEAL_BLOCKED);
        }
        //apply harvest
        if(battler->hasAbility(HARVEST) && RNG::coinFlip() && 
            !battler->isFainted()){
            if(battler->restoreBerry()){
                event_handler->displayMsg(battler->getNickname() + " harvested a berry!");
            }
        }
    }
    // collect items on the ground
    if(player_active->hasAbility(PICKUP) && item_on_the_ground_player != NO_ITEM_TYPE && 
        !player_active->isFainted()){
        if(!player_active->hasHeldItem()){
            player_active->setHeldItem(item_on_the_ground_player);
            item_on_the_ground_player = NO_ITEM_TYPE;
        }
    }
    item_on_the_ground_player = NO_ITEM_TYPE;
    if(opponent_active->hasAbility(PICKUP) && item_on_the_ground_opponent != NO_ITEM_TYPE && 
        !opponent_active->isFainted()){
        if(!opponent_active->hasHeldItem()){
            opponent_active->setHeldItem(item_on_the_ground_opponent);
            item_on_the_ground_opponent = NO_ITEM_TYPE;
        }
    }
    item_on_the_ground_opponent = NO_ITEM_TYPE;
    //check uproars
    checkUproars();
    // check for form change due to zen mode
    checkZenModes();
}

void Battle::checkUproars(){
    // check if uproaring monster is on the field
    std::set<Monster*> uproaring_monster_to_remove;
    for(auto uproaring_monster: uproaring_monsters){
        if((uproaring_monster != player_active->getMonster())&&
            (uproaring_monster != opponent_active->getMonster())){
            uproaring_monster_to_remove.insert(uproaring_monster);
        }
    }
    // all uproaring monster not on field are removed
    for(auto uproaring_monster: uproaring_monster_to_remove){
        uproaring_monsters.erase(uproaring_monster);
    }
}

bool Battle::isUproar() {
    return !uproaring_monsters.empty();
}

bool Battle::isOver()const {
    if(caught_wild_monster)
        return true;
    if(is_wild_battle_over)
        return true;
    return player_team->isDead() || opponent_team->isDead();
}

bool Battle::playerWon()const {
    return opponent_team->isDead() && ! player_team->isDead();
}

bool Battle::opponentWon()const {
    return player_team->isDead();
}

void Battle::performTurn(){
    #ifdef DEBUG
    std::cout<<"Performing turn "<<turn<<std::endl;
    std::cout.flush();
    #endif
    // 0: display battle situation
    event_handler->displayBattleSituation(player_active,player_team,opponent_active,opponent_team);
    for(auto battler: getBattlersSortedBySpeed()){
        battler->nextTurn();
    }
    // 1: choose actions
    #ifdef DEBUG
    std::cout<<"Choosing action player "<<std::endl;
    std::cout.flush();
    #endif
    BattleAction player_action = event_handler->chooseAction(player_active,player_team,opponent_active,field,player_bag,is_wild_battle);
    actions[player_active] = player_action;
    #ifdef DEBUG
    std::cout<<"Choosing action CPU "<<std::endl;
    std::cout.flush();
    #endif
    BattleAction opponent_action = cpu_ai->chooseAction(opponent_active,opponent_team,player_active,field,opponent_bag);    
    actions[opponent_active] = opponent_action;
    // apply mega evolutions
    performMegaEvolutions();
    
    // 2: focus monsters
    if(player_action.getAttackId() == FOCUS_PUNCH_ID){
        player_active->addVolatileCondition(FOCUSED,5);
    }
    if(opponent_action.getAttackId() == FOCUS_PUNCH_ID){
        opponent_active->addVolatileCondition(FOCUSED,5);
    }
    // 2.1 modify actions
    applyBattleActionModifiers();
    
    // 3: sort actions by priority
    #ifdef DEBUG
    std::cout<<"Computing action order "<<std::endl;
    std::cout.flush();
    #endif
    std::vector<Battler*> sorted_battlers = getBattlersSortedByAction();
    int count = 1;
    for(auto battler: sorted_battlers){
        actions[battler].setOrder(count++);
    }
    #ifdef DEBUG
    std::cout<<"Performing actions "<<std::endl;
    std::cout.flush();
    #endif
    // 4: perform actions
    bool player_was_changed = false;
    bool opponent_was_changed = false;
    for(auto action_battler: sorted_battlers){
        BattleAction action = actions[action_battler];
        //skip acxtions that were assigned to a different monster
        if(action.getActor()==PLAYER && player_was_changed){
            continue;
        }
        if(action.getActor()==OPPONENT && opponent_was_changed){
            continue;
        }
        performAction(action, sorted_battlers);
        checkForExp();
        if(isOver()){
            return;
        }
        
        
        if(player_active->isFainted()){
            removeVolatilesFromOpponentOfMonsterLeavingField(PLAYER);
            checkMonsterLeavingAbilities(PLAYER);
        }
        if(opponent_active->isFainted()){
            removeVolatilesFromOpponentOfMonsterLeavingField(OPPONENT);
            checkMonsterLeavingAbilities(OPPONENT);
            checkForExp();
        }
        if(tryEjectPack(PLAYER))
            player_was_changed = true;
        if(tryEjectPack(OPPONENT))
            opponent_was_changed = true;
    }
    // std::cout<<"ATTACKS DONE!"<<std::endl;std::cout.flush();
    #ifdef DEBUG
    std::cout<<"Applying scheduled future sights "<<std::endl;
    std::cout.flush();
    #endif
    applyScheduledFutureSights();
    checkForExp();
    if(isOver()){return;}
    // 4.5 check if something is dead and in case remove some volatiles
    if(player_active->isFainted()){
        checkMonsterLeavingAbilities(PLAYER);
        removeVolatilesFromOpponentOfMonsterLeavingField(PLAYER);
    }
    if(opponent_active->isFainted()){
        checkMonsterLeavingAbilities(OPPONENT);
        removeVolatilesFromOpponentOfMonsterLeavingField(OPPONENT);
    }
    // 5: apply after turn effects
    #ifdef DEBUG
    std::cout<<"Applying post damage "<<std::endl;
    std::cout.flush();
    #endif
    applyPostDamage();
    // std::cout<<"POST DMG DONE!"<<std::endl;std::cout.flush();
    // 6: force switches if anything is dead
    #ifdef DEBUG
    std::cout<<"Forcing switches "<<std::endl;
    std::cout.flush();
    #endif
    if(isOver()){return;}
    if(RNG::coinFlip()){//randomly force one at a time the players to switch
        while(player_active->isFainted()){
            forceSwitch(PLAYER);
            if(isOver()){return;}
        }
        while(opponent_active->isFainted()){
            forceSwitch(OPPONENT);
            if(isOver()){return;}
        }
    }else{
        while(opponent_active->isFainted()){
            forceSwitch(OPPONENT);
            if(isOver()){return;}
        }
        while(player_active->isFainted()){
            forceSwitch(PLAYER);
            if(isOver()){return;}
        }
    }
    #ifdef DEBUG
    std::cout<<"End turn "<<std::endl;
    std::cout.flush();
    #endif
}

void Battle::performAction(BattleAction action, std::vector<Battler*>& all_actions){
    Battler* active_user = getActorBattler(action.getActor());
    Battler* other_active = getActorBattler(otherBattleActionActor(action.getActor()));
    // MonsterTeam* user_team = getActorTeam(action.getActor());
    // MonsterTeam* target_team = getActorTeam(otherBattleActionActor(action.getActor()));
    // check if user is alive
    if(active_user->isFainted()){
        return;
    }
    // check that the user is the one who was ordered to complete the action
    if(active_user->getMonster() != action.getMonsterToAct()){
        // event_handler->displayMsg("Action not valid!");
        return;
    }
    
    // perform action
    BattleActionType atype = action.getActionType();
    if(isAttackingActionType(atype)){
        if(active_user->isFainted()){
            return;
        }
        performAttack(action,all_actions);
        // deactivate mold breaker
        active_user->deactivateMoldBreaker();
        other_active->deactivateMoldBreaker();
        //change meloetta form
        active_user->changeFormOnSuccessfulLastAttack();
    }else if(atype == SWITCH){
        if(active_user->isFainted()){
            return;
        }
        performSwitch(action);
    }else if(atype==RECHARGE_TURN){
        performRechargeTurn(action);
    }else if(atype==TRUANT_TURN){
        performTruantTurn(action);
    }else if(atype == USE_ITEM){
        performUseItem(action);
    }else if(atype == ESCAPE){
        performEscape(action);
    }
}

void Battle::performTruantTurn(BattleAction action){
    Battler* active_user = getActorBattler(action.getActor());
    active_user->removeVolatileCondition(TRUANTING);
    if(active_user->isFainted())
        return;
    std::string user_mon_name = getActorBattlerName(action.getActor());
    event_handler->displayMsg(user_mon_name+" is loafing around!");
}

void Battle::performUseItem(BattleAction action){
    ItemType item_to_use = action.getItemToUse();
    ItemData* item_data = ItemData::getItemData(item_to_use);
    if(item_data==nullptr){
        event_handler->displayMsg("Item not found!");
        return;
    }
    ItemCategory item_category = item_data->getCategory();
    std::string player_name;
    Bag * bag_used;
    if(action.getActor() == PLAYER){
        player_name = "Player";
        bag_used = player_bag;
    }else{
        player_name = "Opponent";
        bag_used = opponent_bag;
    }
    switch(item_category){
        case MEDICINE:case BERRY:{
            unsigned int target = action.getSwitchId();
            if(target==0){
                //use on active
                Battler* active_target = getActorBattler(action.getActor());
                // if(active_target->isFainted()){
                //     return;
                // }
                if(!active_target->itemWouldHaveEffect(item_to_use)){
                    event_handler->displayMsg(player_name+" tried to use "+item_data->getName()+" on "+active_target->getNickname()+"!");
                    event_handler->displayMsg("But it had no effect!");
                    return;
                }else{
                    event_handler->displayMsg(player_name+" used "+item_data->getName()+" on "+active_target->getNickname()+"!");
                    active_target->useItem(item_to_use,action.getAttackId());
                    bag_used->removeItem(item_to_use);
                }
            }else{
                //use on bench
                MonsterTeam* target_team = getActorTeam(action.getActor());
                Monster* monster_target = target_team->getMonster(target);
                if(!monster_target->itemWouldHaveEffect(item_to_use)){
                    event_handler->displayMsg(player_name+" used "+item_data->getName()+" on "+monster_target->getNickname()+", but it had no effect!");
                    return;
                }else{
                    event_handler->displayMsg(player_name+" used "+item_data->getName()+" on "+monster_target->getNickname()+"!");
                    monster_target->useItem(item_to_use, event_handler, action.getAttackId());
                    bag_used->removeItem(item_to_use);
                }
            }
            break;
        }
        case BALL:{
            if(!is_wild_battle){
                event_handler->displayMsg("You cannot use a ball in a trainer battle!");
                return;
            }
            tryToCatchWildMonster(item_to_use);
            break;
        }
        default:{
            event_handler->displayMsg(player_name+" tried to use "+item_data->getName()+"!");
            event_handler->displayMsg("But it had no effect!");
            break;
        }
    }
}

void Battle::performRechargeTurn(BattleAction action){
    Battler* active_user = getActorBattler(action.getActor());
    if(active_user->isFainted())
        return;
    std::string user_mon_name = getActorBattlerName(action.getActor());
    active_user->removeVolatileCondition(RECHARGING);
    event_handler->displayMsg(user_mon_name+" is recharging!");
}

void Battle::performAttack(BattleAction action, std::vector<Battler*>& all_actions){
    Attack* attack = Attack::getAttack(action.getAttackId());
    BattleActionActor actor = action.getActor();
    Battler* active_user = getActorBattler(actor);
    if(active_user->isFainted())
        return;
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    // MonsterTeam* user_team = getActorTeam(actor);
    // MonsterTeam* target_team = getActorTeam(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    std::string user_player_name;
    std::string opponents_player_name;
    // get opponent action to check for sucker punch
    BattleAction other_action;
    if(action.getActor() == PLAYER){
        for(Battler* action_battler: all_actions){
            if(action_battler->getActor() == OPPONENT){
                other_action = actions[active_target];
                break;
            }
        }
        user_player_name = "Player";
        opponents_player_name = "Opponent";
    }else{
        for(Battler* action_battler: all_actions){
            if(action_battler->getActor() == PLAYER){
                other_action = actions[active_target];
                break;
            }
        }
        user_player_name = "Opponent";
        opponents_player_name = "Player";
    }
    // check permanent conditions that may not allow to attack
    PermanentStatusCondition status = active_user->getPermanentStatus();
    bool attack_after_target = action.getOrder() > other_action.getOrder();
    if(other_action.getActionType()==SWITCH)// reset to false if opponent does not attack
        attack_after_target = false;
    // activate Mold Breaker on the target
    if(active_user->hasAbility(MOLD_BREAKER) || 
        active_user->hasAbility(TERAVOLT) ||
        active_user->hasAbility(TURBOBLAZE))
        active_target->activateMoldBreaker();
    // activate truant to signal that next turn the user will not be able to attack
    if(active_user->hasAbility(TRUANT)){
        active_user->addVolatileCondition(TRUANTING,-1);
    }

    // reset Destiny Bond
    if(active_user->hasVolatileCondition(DESTINY_BOND))
        active_user->removeVolatileCondition(DESTINY_BOND);
    // remove protects if move is not protect
    if(attack->getEffectId() != 26 && 
        attack->getEffectId() != 268){
        active_user->resetConsecutiveProtect();
    }
    //reset charged if move is electric type
    Type attack_type = attack->getType(active_user,field);
    if(attack_type == ELECTRIC && active_user->hasVolatileCondition(CHARGED)){
        active_user->removeVolatileCondition(CHARGED);
        active_user->addVolatileCondition(CHARGED_2,-1);
    }
    // sleep
    // remove sleep during uproars
    if(active_user->isAsleep() && isUproar() && !active_user->hasAbility(SOUNDPROOF)){
        event_handler->displayMsg(user_mon_name+" woke up!");
        active_user->clearPermanentStatus();
    }
    // remove sleep from vital spirit and insomnia holders
    if((active_user->hasAbility(VITAL_SPIRIT) || active_user->hasAbility(INSOMNIA)) && active_user->isAsleep()){
        event_handler->displayMsg(user_mon_name+" woke up!");
        active_user->clearPermanentStatus();
    }

    if(active_user->isAsleep()){
        PermanentStatusCondition new_condition = decrementSleep(status);
        active_user->setPermanentStatusForced(new_condition);
        if(active_user->hasAbility(EARLY_BIRD) && new_condition != SLEEP_1){
            // early bird users wake up twice as fast
            new_condition = decrementSleep(status);
            active_user->setPermanentStatusForced(new_condition);
        }
        if(status != SLEEP_1){// it is not waking up yet
            event_handler->displayMsg(user_mon_name+" is sleeping!");
            forgetMoveVolatiles(active_user);
            if(attack->getEffectId()!=194 && attack->getEffectId()!=195)
                return;
        }else{// wakes up now
            active_user->clearPermanentStatus();
            event_handler->displayMsg(user_mon_name+" woke up!");
        }
    } 
    // apply flinch
    if(active_user->hasVolatileCondition(FLINCH)){
        event_handler->displayMsg(user_mon_name+" flinched!");
        if(active_user->hasAbility(STEADFAST)){
            StatCV changes = {{5,1}};
            changeStats(actor,changes,false);
            // active_user->changeSpeedModifierForced(1);
        }
        forgetMoveVolatiles(active_user);
        active_user->setLastAttackFailed();
        return;
    }
    // apply freeze
    if(active_user->isFrozen()){
        unsigned int random_number = RNG::getRandomInteger(1,100);
        if(random_number > 20 && 
            attack->getEffectId()!=253 && //253: sacred fire immediately thaws user out
            attack->getEffectId()!=306 &&//306: fusion flare immediately thaws user out
            attack->getId()!=482 && // 482: SCALD id
            attack->getId()!=520){// 520: Scorching sands id
            event_handler->displayMsg(user_mon_name+" is frozen solid!");
            forgetMoveVolatiles(active_user);
            active_user->setLastAttackFailed();
            return;
        }else
            active_user->clearPermanentStatus();
    }
    
    // apply paralysis
    if(active_user->isParalyzed() && 
        !active_user->hasAbility(MAGIC_GUARD) &&//magic guard prevents from being fully paralyzed
        RNG::oneFourth()){
        event_handler->displayMsg(user_mon_name+" is paralyzed and cannot move!");
        forgetMoveVolatiles(active_user);
        active_user->setLastAttackFailed();
        return;
    }
    // apply infatuation
    if(active_user->hasAbility(OBLIVIOUS))
        active_user->removeVolatileCondition(INFATUATION);
    if(active_user->hasVolatileCondition(INFATUATION) && 
        !active_user->hasAbility(MAGIC_GUARD)){//magic guard prevents the user to be affected by infatuation
        event_handler->displayMsg(user_mon_name+" is infatuated!");
        if(RNG::coinFlip()){
            active_user->setLastAttackFailed();
            event_handler->displayMsg(user_mon_name+" is infatuated and cannot move!");
            forgetMoveVolatiles(active_user);
            return;
        }
        // }
    }

    // apply confusion
    if(active_user->hasAbility(OWN_TEMPO))
        active_user->removeVolatileCondition(CONFUSION);
    if(active_user->hasVolatileCondition(CONFUSION)){
        active_user->decrementVolatileCondition(CONFUSION);
        if(active_user->hasVolatileCondition(CONFUSION)){
            //check for hits imself
            event_handler->displayMsg(user_mon_name+" is confused...");
            if(0==RNG::getRandomInteger(0,2)){
                event_handler->displayMsg(user_mon_name+" hurt itself in confusion!");
                active_user->removeVolatileCondition(CHARGING_METEORBEAM);
                forgetMoveVolatiles(active_user);
                unsigned int level = active_user->getLevel();
                unsigned int attack_stat = active_user->getModifiedAttack();
                unsigned int defense_stat = active_user->getModifiedDefense();
                unsigned int damage = max(int(baseDamage(
                    level, 40, attack_stat, defense_stat)),1);
                unsigned int actual_damage = active_user->addDirectDamage(damage);
                // event_handler->displayMsg(user_mon_name+" dealt "+std::to_string(actual_damage)+" damage to itself!");
                event_handler->displayDmgDealt(actual_damage,active_user,"recoil");
                return;
            }
        }
    }

    //check if move is diabled
    unsigned int attack_id = action.getAttackId();
    if(active_user->isAttackDisabled(attack_id)){
        event_handler->displayMsg(user_mon_name+" cannot use "+attack->getName()+"!");
        attack_id = STRUGGLE_ID;
    }
    
    // check if user has PPs for the move
    if(!active_user->hasPP(attack_id)){
        event_handler->displayMsg(user_mon_name+" has no PP left for "+attack->getName()+"!");
        attack_id = STRUGGLE_ID;
        attack = Attack::getAttack(attack_id);
    }

    // activate protean
    if(active_user->hasAbility(PROTEAN) &&//check protean
        attack_type != NO_TYPE &&// cannot turn to type NO_TYPE
        !active_user->hasType(attack_type)){//cannot turn to a type I already have
        event_handler->displayMsg(user_mon_name+" changed its type to "+typeToString(attack_type)+"!");
        active_user->clearTypes();
        active_user->addType(attack_type);
    }


    event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
    active_user->setLastAttackUsed(attack_id);
    if(attack->getEffectId() != 80){// I need to preserve this value if copycat is being used
        last_attack_used_id = attack_id;
    }
    if(action.getActionType() == ATTACK && attack_id != STRUGGLE_ID){
        // cannot remove PP to struggle
        unsigned int PP_amount = 1;
        if(active_target->hasAbility(PRESSURE))
            PP_amount = 2;
        active_user->usePP(attack_id, PP_amount);
    }
    // sleep talk changes attack
    if(attack->getEffectId()==194){
        if(!active_user->isAsleep()){
            event_handler->displayMsg(user_mon_name+"But it failed!");
            active_user->removeVolatileCondition(CHARGING_METEORBEAM);
            forgetMoveVolatiles(active_user);
            active_user->setLastAttackFailed();
            active_user->removeVolatileCondition(LASER_FOCUS);
            return;
        }
        auto available_attacks = active_user->getAttacks();
        std::vector<unsigned int> choices;
        for(auto item: available_attacks){
            if(item.first == STRUGGLE_ID)
                continue;
            if(item.first == 0)
                continue;
            if(item.first == attack_id)
                continue;
            choices.push_back(item.first);
        }
        if(choices.empty()){
            event_handler->displayMsg("But it failed!");
            decrementVolatiles(active_user);
            active_user->setLastAttackFailed();
            active_user->removeVolatileCondition(LASER_FOCUS);
            return;
        }
        unsigned int random_index = RNG::getRandomInteger(0,choices.size()-1);
        attack_id = choices[random_index];
        attack = Attack::getAttack(attack_id);
        event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
        last_attack_used_id = attack_id;
    }
    // copycat changes move
    if(attack->getEffectId() == 80){
        // event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
        Attack* copied_attack = Attack::getAttack(last_attack_used_id);
        if(copied_attack==nullptr || !copied_attack->canBeCopycat()){
            event_handler->displayMsg("But it failed!");
            decrementVolatiles(active_user);
            active_user->removeVolatileCondition(LASER_FOCUS);
            return;
        }else{
            attack_id = last_attack_used_id;
            attack = copied_attack;
            event_handler->displayMsg(user_mon_name+" copies "+copied_attack->getName()+"!");
        }
    }
    // metronome changes move
    if(attack->getEffectId() == 84){
        // event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
        event_handler->displayMsg(user_mon_name+" shakes its finger in the air!");
        attack = Attack::getRandomMetronomeAttack();
        attack_id = attack->getId();
        event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
        last_attack_used_id = attack_id;
    }
    
    //increment rounds counter
    if(attack->getEffectId() == 94){
        rounds_used_this_turn++;
    }

    if(checkIfAttackFails(attack,action,other_action)){
        return;
    }
    active_user->setLastAttackHit();

    bool acts_second = action.getOrder() > other_action.getOrder();
    
    // check if attack hits
    if(checkIfMoveMisses(attack,action.getActor(),acts_second)){// IF MISS
        event_handler->displayMsg(opponent_mon_name+" avoided the attack!");
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->removeVolatileCondition(ROLLINGOUT);
        if(attack->getEffectId()==100){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
            return;
        }
        if(attack->getEffectId()==187){
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        if((attack->getEffectId() == 171 || attack->getEffectId() == 174 || attack->getEffectId() == 277)&&!active_user->hasAbility(MAGIC_GUARD)){
            // user takes 50% maxHP recoil damage
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int damage = max_hp / 2;
            unsigned int actual_damage = active_user->addDirectDamage(damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage from recoil!"); 
            event_handler->displayDmgDealt(actual_damage,active_user,"recoil");
            if(active_user->isFainted()){
                return;
            }
        }
        unsigned int effect_id = attack->getEffectId();
        if(active_user->hasHeldItem(BLUNDER_POLICY) && effect_id!=103 && effect_id!=156){
            // blunder policy activates only if the move misses and is not a OHKO move
            active_user->consumeHeldItem();
        }
        return;
    }

    double effectiveness = computeEffectiveness(attack,active_user->getActor(),active_target->getActor());
    
    std::pair<unsigned int, bool> actual_damage = applyDamage(attack,action.getActor(),attack_after_target,effectiveness,acts_second);
    if(active_user->isFainted())
        return;

    // apply effects
    // if(attack->getCategory()==STATUS || !active_user->isFainted() || actual_damage.first>0)
    //     applyAttackEffect(attack,action.getActor(),actual_damage.first,actual_damage.second);
    if(is_wild_battle_over)//some attacks may end wild battles as a side effect
        return;
    // these pointers may have changed due to move effects!!! (es: switching)
    active_user = getActorBattler(action.getActor());
    active_target = getActorBattler(otherBattleActionActor(action.getActor()));
    if(active_user->isFainted())
        return;

    // apply recoil damage
    applyRecoil(attack,actual_damage.first,action.getActor());
    if(active_user->isFainted() || isOver())
        return;
    
    if(active_user->hasHeldItem(THROAT_SPRAY) && attack->isSoundBased()){
        // throat spray activates if the user uses a sound based move
        active_user->consumeHeldItem();
        StatCV changes = {{3,1}};
        changeStats(action.getActor(),changes,false);
    }

    // decrement volatile
    decrementVolatiles(active_user);
}

bool Battle::checkIfMoveMisses(Attack* attack, BattleActionActor actor, bool acts_second){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    AttackTarget target = attack->getTarget();
    //no guard bypasses accuracy check and moves never miss
    if(active_user->hasAbility(NO_GUARD))
        return false;
    // toxic bypasses accuracy checks if used by a poison type
    if(attack->getEffectId() == 298 && active_user->hasType(POISON)){
        return false;
    }
    // if the user is locked onto thew target, it cannot miss
    // (even in case of semi-invulnerable moves)
    if(active_user->hasLockOn(active_target->getMonster())){
        return false;
    }
    unsigned int effect_id = attack->getEffectId();
    // fly (target is flying high and cannot be hit)
    if(active_target->hasVolatileCondition(FLYING_HIGH) && target == TARGET_OPPONENT){
        if((effect_id!=45)&&(effect_id!=227)&&(effect_id!=67)&&(effect_id!=299)&&(effect_id!=138)&&(effect_id!=300))//thunder, twister, hurricane, smack down, whirlwind and gust also hits monsters that are flying
            return true;   
        if(effect_id==138){
            event_handler->displayMsg(opponent_mon_name+" was smacked to the ground!");
            active_target->removeVolatileCondition(FLYING_HIGH);
        }
    }
    // bounce (target is flying high and cannot be hit)
    if(active_target->hasVolatileCondition(BOUNCING) && target == TARGET_OPPONENT){
        if((effect_id!=45)&&(effect_id!=227)&&(effect_id!=67)&&(effect_id!=299))//thunder, twister, hurricane and gust also hits monsters that are bouncing
            return true; 
    }
    // dig (target is underground and cannot be hit)
    if(active_target->hasVolatileCondition(UNDERGROUND) && target == TARGET_OPPONENT){
        if(attack->getEffectId()!=73)//earthquake also hits monsters that are underground
            return true;   
    }
    // phantom force (target has vanished and cannot be hit)
    if(active_target->hasVolatileCondition(VANISHED) && target == TARGET_OPPONENT){
        return true;
    }
    // shadow force (target has vanished and cannot be hit)
    if(active_target->hasVolatileCondition(VANISHED_2) && target == TARGET_OPPONENT){
        return true;
    }
    // dive (target is underwater and cannot be hit)
    if(active_target->hasVolatileCondition(UNDERWATER) && target == TARGET_OPPONENT){
        if(attack->getEffectId()!=135)//surf also hits monsters that are underwater
            return true;  
        if(attack->getEffectId()!=158)//whirlpool also hits monsters that are underwater
            return true;
    }
    //stomp always hits minimize users
    if((attack->getEffectId()==141 || attack->getEffectId()==277) && active_target->hasUsedAttack(MINIMIZE_ID)){
        return false;
    }

    unsigned int attack_accuracy = attack->getAccuracy();
    if(attack_accuracy != ALWAYS_HITS && target == TARGET_OPPONENT){
        unsigned int base_modified_accuracy;
        unsigned int base_modified_evasion;
        if(active_user->hasAbility(UNAWARE)){//UNAWARE ignores Accuracy and Evasion stats changes
            base_modified_accuracy = 100;
            base_modified_evasion = 100;
        }else{
            base_modified_accuracy = active_user->getModifiedAccuracy();
            base_modified_evasion = active_target->getModifiedEvasion();
        }
        if(effect_id==286){
            //sacred sword ignores evasion modifiers
            base_modified_evasion = 100;
        }
        if(active_user->hasHeldItem(ZOOM_LENS) && acts_second){
            // ZOOM LENS increases accuracy by 20% if the user acts second
            base_modified_accuracy *= 1.2;
        }
        if(active_target->hasAbility(WONDER_SKIN) && attack->getCategory() == STATUS){
            // WONDER SKIN drops accuracy of status moves that apply permanent conditions
            switch(attack->getEffectId()){
                case 4:case 5:case 14:
                case 16:case 61:case 67:
                case 76:case 104:case 210:
                case 222:case 232:case 308:
                case 309:{
                    if(attack_accuracy > 50)
                        attack_accuracy = 50;
                    break;
                }
                default:break;
            }
        }
        //sheer cold
        if(attack->getEffectId() == 156 && !active_target->hasType(ICE)){
            // SHEER COLD is less acuurate if used by a non-ice type
            attack_accuracy = 20;
            unsigned int user_level = active_user->getLevel();
            unsigned int target_level = active_target->getLevel();
            if(user_level < target_level)// always misses when used by a lower level mon
                return true;
            // more likely to hit if user is higher level depending on the level delta
            attack_accuracy += user_level - target_level;
            // no modifiers affect this move accuracy
            if(RNG::getRandomInteger(1,100) > attack_accuracy){
                // miss
                return true;
            }else{
                // hit
                return false;
            }
        }
        //fissure
        if(attack->getEffectId() == 103){
            unsigned int user_level = active_user->getLevel();
            unsigned int target_level = active_target->getLevel();
            if(user_level < target_level)// always misses when used by a lower level mon
                return true;
            // more likely to hit if user is higher level depending on the level delta
            attack_accuracy = 30 + user_level - target_level;
            // no modifiers affect this move accuracy
            if(RNG::getRandomInteger(1,100) > attack_accuracy){
                // miss
                return true;
            }else{
                // hit
                return false;
            }
        }
        unsigned int modified_accuracy = base_modified_accuracy * attack_accuracy / 100;
        unsigned int modified_evasion = base_modified_evasion;

        // THUNDER ignore accuracy under RAIN and has worse accuracy under SUN
        if(attack->getEffectId() == 67 && !thereIsaCloudNine()){
            if(field->getWeather() == RAIN || field->getWeather() == HEAVY_RAIN)
                modified_evasion = 0;
            else if(field->getWeather() == SUN || field->getWeather() == EXTREME_SUN)
                modified_accuracy = active_user->getModifiedAccuracy() * 0.5;
        }
        // HURRICANE: halve accuracy under SUN
        if(attack->getEffectId() == 299 && !thereIsaCloudNine()){
            if(field->getWeather() == SUN || field->getWeather() == EXTREME_SUN)
                modified_accuracy = active_user->getModifiedAccuracy() * 0.5;
        }
        // BLIZZARD ignores accuracy checks under HAIL and SNOW
        if(attack->getEffectId() == 67 && !thereIsaCloudNine()){
            if(field->getWeather() == HAIL || field->getWeather() == SNOWSTORM)
                modified_evasion = 0;
        }
        if(active_user->hasAbility(KEEN_EYE) && modified_evasion > 100){
            modified_evasion = 100;
        }
        if(active_target->hasAbility(SAND_VEIL) && 
            field->getWeather() == SANDSTORM &&
            !thereIsaCloudNine()){
            modified_accuracy = modified_accuracy * 3277.0/4096;
        }
        if(active_target->hasAbility(SNOW_CLOAK) && 
            (field->getWeather() == HAIL || field->getWeather() == SNOWSTORM)&&
            !thereIsaCloudNine()){
            modified_accuracy = modified_accuracy * 4.0/5;
        }
        // INCERASED ACCURACY effect
        if(active_user->hasVolatileCondition(INCREASED_ACCURACY)){
            modified_accuracy *= 1.2;
            active_user->removeVolatileCondition(INCREASED_ACCURACY);
        }
        // TANGLED FEET effect
        if(active_target->hasAbility(TANGLED_FEET) && active_target->hasVolatileCondition(CONFUSION)){
            modified_accuracy *= 0.5;
        }
        // Bright Powder and Lax Incense reduce accuracy by 10%
        if(active_target->hasHeldItem(BRIGHT_POWDER) || active_target->hasHeldItem(LAX_INCENSE)){
            modified_accuracy *= 0.9;
        }

        // actual check
        if(modified_accuracy < modified_evasion){
            unsigned int check_result = RNG::getRandomInteger(1, modified_evasion);
            if(check_result > modified_accuracy){
                // miss
                return true;
            }
        }
    }
    return false;
}

std::pair<unsigned int,bool> Battle::applyDamage(Attack* attack,BattleActionActor actor, bool target_attack_first, double effectiveness, bool acts_second){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    MonsterTeam* user_team = getActorTeam(actor);
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    // check if present heals instead of dealing damage (20% chance)
    if(attack->getEffectId() == 249 && RNG::getRandomInteger(1,5)==1){
        // present heals the target
        unsigned int heal_amount = (active_target->getMaxHP()+3) / 4;
        unsigned int actual_heal = active_target->removeDamage(heal_amount);
        if(actual_heal>0)
            event_handler->displayMsg(user_mon_name+"'s present healed "+opponent_mon_name+" for "+std::to_string(actual_heal)+" HP!");
        return {0,active_target->hasSubstitute()};
    }
    ItemData* old_held_item = ItemData::getItemData(active_user->getMonster()->getHeldItem());
    // check if move is multi hit
    unsigned int number_of_hits = 1;
    bool parental_bond_has_effect = false;
    if(attack->getEffectId() == 36){//moves hitting from 2 to 5 times
        int random_number = RNG::getRandomInteger(1,8);
        if(active_user->hasHeldItem(LOADED_DICE)){
            // loaded dice always hits 4 to 5 times
            if(random_number<=4)
                number_of_hits = 5;
            else
                number_of_hits = 4;
        }else{
            // attcks hitting 2 to 5 times
            if(random_number==8)
                number_of_hits = 5;
            else if(random_number==7)
                number_of_hits = 4;
            else if(random_number>3)
                number_of_hits = 3;
            else
                number_of_hits = 2;
        }
        //skill link always guarantees 5 shots
        if(active_user->hasAbility(SKILL_LINK))
            number_of_hits = 5;
    }else if(attack->getEffectId()==75){//double hitting moves
        number_of_hits = 2;
    }else if(attack->getEffectId()==246){//beat up hits once for each team member
        number_of_hits = user_team->getSize();
    }else if(attack->getEffectId()==251){//triple kick hits at most 3 times
        number_of_hits = 3;
    }else if(active_user->hasAbility(PARENTAL_BOND)){
        // parental bond always guarantees 2 shots
        number_of_hits = 2;
        parental_bond_has_effect = true;
    }
    // perform damage calc and apply damage
    std::pair<unsigned int,bool> actual_damage = {0,active_target->hasSubstitute()};
    unsigned int total_actual_damage = 0;
    unsigned int actual_hits = 0;
    unsigned int effect_id = attack->getEffectId();
    // get category
    AttackType category = attack->getCategory();
    Type attack_type = attack->getType(active_user,field);
    if(category != STATUS){
        // check immunity
        if(effectiveness == 0){
            event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
            decrementVolatiles(active_user);
            if((effect_id == 171 || effect_id == 174 || effect_id == 277)&&!active_user->hasAbility(MAGIC_GUARD)){
                // user takes 50% maxHP recoil damage
                unsigned int max_hp = active_user->getMaxHP();
                unsigned int damage = max_hp / 2;
                unsigned int actual_recoil_damage = active_user->addDirectDamage(damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" damage from recoil!"); 
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
            }
            return {0,active_target->hasSubstitute()};
        }
        // compute effective category for attack
        if(effect_id==210){//shell side arm chooses which category to be
            unsigned int physical_dmg = baseDamage(active_user->getLevel(), attack->getPower(), active_user->getModifiedAttack(), active_target->getModifiedDefense());
            unsigned int special_dmg = baseDamage(active_user->getLevel(), attack->getPower(), active_user->getModifiedSpecialAttack(), active_target->getModifiedSpecialDefense());
            if(physical_dmg > special_dmg){
                category = PHYSICAL;
            }else if(special_dmg > physical_dmg || RNG::coinFlip()){
                category = SPECIAL;
            }else{
                category = PHYSICAL;
            }
        }
        //cycle in order to deal with multi hit moves
        for(unsigned int i=0;i<number_of_hits;i++){
            if(effect_id==246){
                if(user_team->getMonster(i)->isFainted()){
                // beat up skips fainted monsters
                continue;
                }else{
                    event_handler->displayMsg(user_team->getMonster(i)->getNickname()+" attacks!");
                }
            }
            if(effect_id==251 && i>0 && checkIfMoveMisses(attack,actor,acts_second) && !active_user->hasHeldItem(LOADED_DICE)){
                //attack misses at i-th hit
                break;
            }
            actual_hits++;
            if(i==1 && parental_bond_has_effect){
                // display second attack msg for parental bond
                event_handler->displayMsg(user_mon_name+" attacks again!");
            }
            //check for critical hit;
            bool is_critical_hit = isCriticalHit(attack,active_user->getActor(),active_target->getActor());
            if(is_critical_hit){
                event_handler->displayMsg("It's a critical hit!");
            }

            unsigned int beatup_id = 0;
            if(effect_id==246 || effect_id==251){
                // beat up attack for i-th member of the team
                // triple kick attack for i-th hit has different power each time
                beatup_id = i;
            }
            // apply damage
            unsigned int damage = max(computeDamage(attack->getId(), actor, is_critical_hit, target_attack_first,effectiveness,category,beatup_id),1);
            if(i==1 && parental_bond_has_effect){
                // parental bond second shot is weakened in power
                damage = max(1, damage / 4);
            }
            if(effectiveness>1.1 && active_user->hasHeldItem(EXPERT_BELT)){
                //expert belt boosts damage of supereffective moves by 20%
                damage *= 1.2;
            }
            if(active_user->hasHeldItem(LIFE_ORB)){
                damage *= 1.3;
            }
            if(effect_id == 150 && damage >= active_target->getCurrentHP()){// user cannot be killed by this move
                damage = active_target->getCurrentHP()-1;
            }
            if(effect_id == 103 || effect_id==156){//one hit KO
                event_handler->displayMsg("It's a one hit KO!");//notice that this may only OHKO the substitute!
                damage = active_target->getMaxHP() * 1000;
            }
            if(effect_id == 280 && old_held_item!=nullptr){
                //custom message for poltergeist
                event_handler->displayMsg(opponent_mon_name+" is about to be attacked by its "+old_held_item->getName());
            }
            actual_damage = active_target->addDamage(damage,category, effectiveness, attack->isSoundBased());
            active_target->hitOnceMore(attack_type);
            //weak armor activation
            if(actual_damage.first>0 && !actual_damage.second && active_target->hasAbility(WEAK_ARMOR) && !active_target->isFainted() && category==PHYSICAL){
                StatCV changes = {{2,-1},{5,2}};
                changeStats(otherBattleActionActor(actor),changes,false);
            }
            // berserk activation
            if((active_target->getCurrentHP()>=active_target->getMaxHP()/2) && 
                (active_target->getCurrentHP()<active_target->getMaxHP()/2) && active_target->hasAbility(BERSERK)){
                StatCV changes = {{1,1}};
                changeStats(otherBattleActionActor(actor),changes,false);
            }
            // berry check
            active_target->tryEatLowHPBerry();    
            if(!actual_damage.second)        
                total_actual_damage += actual_damage.first;
            if(number_of_hits>1 && actual_damage.first>0 && !actual_damage.second){
                // event_handler->displayMsg(opponent_mon_name+" took "+std::to_string(actual_damage.first)+" damage!");
                event_handler->displayDmgDealt(actual_damage.first,active_target);
            }
            
            bool makes_contact = false;
            if(attack->makesContact() || 
                (effect_id == 210 && category==PHYSICAL))//shell side arm becomes physical sometimes
                makes_contact=true;

            // contact effects
            if(actual_damage.first>0 && !actual_damage.second)
                applyContactEffects(attack,actor,makes_contact);
            if(active_target->isFainted()){
                if(actor == PLAYER)
                    turn_of_player_last_kill = turn;
                else
                    turn_of_opponent_last_kill = turn;
                break;
            }
            
            // justified increase attack by 1 when hit by a dark move
            if(attack_type==DARK && active_target->hasAbility(JUSTIFIED) && !active_target->isFainted() && !actual_damage.second){
                event_handler->displayMsg(opponent_mon_name+"'s Justified triggers!");
                // active_user->changeAttackModifierForced(1);
                StatCV changes = {{1,1}};
                changeStats(actor,changes,false);
            }

            //anger point raises attack of taget when hit by a crit
            if(is_critical_hit && active_target->hasAbility(ANGER_POINT) && !actual_damage.second && !active_target->isFainted()){
                event_handler->displayMsg(opponent_mon_name+"'s Anger Point triggers!");
                // active_target->changeAttackModifierForced(12);
                StatCV changes = {{1,12}};
                changeStats(otherBattleActionActor(actor),changes,false);
            }
            active_target->tryEatAfterGettingHitBerry(category,effectiveness,active_user);
            if(active_user->isFainted())
                break;
            //restore HP with shell bell
            if(active_user->hasHeldItem(SHELL_BELL) && actual_damage.first>0){
                unsigned int shell_bell_heal = max(1,actual_damage.first / 8);
                unsigned int actual_shell_bell_heal = active_user->removeDamage(shell_bell_heal);
                if(actual_shell_bell_heal>0)
                    event_handler->displayMsg(user_mon_name+" restored "+std::to_string(actual_shell_bell_heal)+" HP thanks to its Shell Bell!");
            }
        }
        // check super effectiveness
        // std::cout<<"Effectiveness: "<<effectiveness<<std::endl;
        if(effectiveness > 1.01){
            event_handler->displayMsg("It's super effective!");
        }
        if(effectiveness < 0.99){
            event_handler->displayMsg("It's not very effective...");
        }
        // print action result
        if(number_of_hits>1)
            event_handler->displayMsg(opponent_mon_name+" was hit "+std::to_string(actual_hits)+" times!");
        else if(actual_damage.first>0 && !actual_damage.second){ // number_of hits == 1
            // event_handler->displayMsg(opponent_mon_name+" took "+std::to_string(actual_damage.first)+" damage!");
            event_handler->displayDmgDealt(actual_damage.first,active_target);
        }
        //active_user->setLastAttackUsed(attack->getId());
        last_attack_used_id = attack->getId();

        bool effect_is_applied=true;
        unsigned int effect_chance = attack->getEffectChance();
        if (effect_chance!=ALWAYS_HITS){
            if(active_user->hasAbility(SERENE_GRACE))//serene grace doubles up likelihood of applying effects
                effect_chance*= 2;
            unsigned int random = RNG::getRandomInteger(1,100);
            if(random > effect_chance){
                effect_is_applied = false;
            }
        }
        // covert cloak prevents secondary effects
        if(active_user->hasHeldItem(COVERT_CLOAK) && 
            attack->getEffectTarget() == TARGET_OPPONENT){
            effect_is_applied = false;
        }
        // covert cloak prevents secondary effects
        // SHIELD DUST PROTECTS OPPONENT FROM SECONDARY EFFECTS
        if(active_target->hasAbility(SHIELD_DUST) && 
            attack->getEffectTarget() == TARGET_OPPONENT){
            effect_is_applied = false;
        }
        // SHEER FORCE PREVENTS INFLICTING SECONDARY EFFECTS TO OPPONENT
        if(active_user->hasAbility(SHEER_FORCE) &&
            attack->getEffectTarget() == TARGET_OPPONENT){
            effect_is_applied = false;
        }
        // OVERCOAT PREVENTS POWDER BASED MOVES EFFECTS
        if((active_target->hasAbility(OVERCOAT) || active_target->hasHeldItem(SAFETY_GOGGLES)) && 
            attack->isPowder() &&
            attack->getEffectTarget() == TARGET_OPPONENT){
            effect_is_applied = false;
        }
        Monster * old_user = active_user->getMonster();
        ItemType user_held_item = active_user->getHeldItem();
        if(effect_is_applied)
            applyAttackEffect(attack,actor,otherBattleActionActor(actor),actual_damage.second);
        active_user = getActorBattler(actor);
        Monster * new_user = active_user->getMonster();
        bool user_changed = false;
        if(old_user != new_user){
            // user changed due to move effect
            user_changed = true;
        }
        
        // check if target is dead
        if(active_target->isFainted()){
            if(effect_id==44 && !active_user->isFainted()){// if the damage of the attack kills the target, the user gains +2 ATT
                // bool res = active_user->changeAttackModifier(+2);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                StatCV changes = {{1,2}};
                changeStats(actor,changes,false);
            }
            //apply grudge effect
            if(active_target->hasVolatileCondition(GRUDGED) && !active_user->isFainted() && !user_changed){
                event_handler->displayMsg(opponent_mon_name+" removed all PPs of "+user_mon_name+"'s "+attack->getName()+"!");
                active_user->usePP(attack->getId(),1000);//no attack should have more than 1000 PP, otherwise this code fails
            }
            //apply destiny bond effect
            if(active_target->hasVolatileCondition(DESTINY_BOND) && !active_user->isFainted() && !user_changed){
                event_handler->displayMsg(opponent_mon_name+" takes "+user_mon_name+" with it!");
                active_user->addDirectDamage(active_user->getMaxHP());
            }
            if(active_target->hasAbility(AFTERMATH) &&
                !active_user->hasAbility(DAMP) &&
                !active_user->isFainted()){
                event_handler->displayMsg(opponent_mon_name+"'s Aftermath triggers!");
                unsigned int aftermath_damage = active_user->getMaxHP() / 4;
                unsigned int actual_aftermath_damage = active_user->addDirectDamage(aftermath_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_aftermath_damage)+" damage!");
                event_handler->displayDmgDealt(actual_aftermath_damage,active_user,opponent_mon_name+"'s Aftermath");
            }
            if(!active_user->isFainted() && active_user->hasAbility(MOXIE) && active_user->getAttackModifier()!=MAX_MODIFIER && !user_changed){
                // moxie increases attack by 1 when a target is KOed by a user's attack
                event_handler->displayMsg(user_mon_name+"'s Moxie triggers!");
                // active_user->changeAttackModifier(1);
                StatCV changes = {{1,1}};
                changeStats(actor,changes,false);
            }
            return {total_actual_damage,false};
        }else{
            if(active_target->hasAbility(COLOR_CHANGE) &&
            // color change changes the type of the target to the type of the attack that hits it
                !active_target->hasType(attack_type) &&
                attack_type!=NO_TYPE &&
                !actual_damage.second){//does not activate if a substitute is hit
                active_target->clearTypes();
                active_target->addType(attack_type);
                event_handler->displayMsg(opponent_mon_name+"'s became "+typeToString(attack_type)+" type thanks to its Color Change ability!");
            }
            if(active_target->isFrozen() && (attack_type==FIRE || attack->getId()==482 || attack->getId()==520))//scald/scorching sands and fire type moves thaw frozen target
                // 482: SCALD id
                // 520: SCORCHING_SANDS id
                active_target->clearPermanentStatus();
            // unsigned int effect_id = effect_id;
            if((user_held_item==KINGS_ROCK || user_held_item==RAZOR_FANG) &&//kings rock effect
                actual_damage.first>0 && !actual_damage.second &&
                effect_id!=21 && effect_id!=45 && effect_id!=105 && 
                effect_id!=141 && effect_id!=195 && effect_id!=197 &&
                effect_id!=279 &&// attacks that cause flinch
                RNG::getRandomInteger(1,10)==1){//10% chance
                active_target->addVolatileCondition(FLINCH,-1);
            }
            if(active_target->hasHeldItem(EJECT_BUTTON) && opponent_team->hasAliveBackup() && 
                actual_damage.first>0 && !actual_damage.second){
                // Eject button effect
                active_target->consumeHeldItem();
                forceSwitch(otherBattleActionActor(actor));
                return {total_actual_damage,actual_damage.second};
            }
            if(active_target->hasHeldItem(RED_CARD) && user_team->hasAliveBackup() && 
                actual_damage.first>0 && !actual_damage.second && !user_changed){
                // Red card effect
                active_target->consumeHeldItem();
                event_handler->displayMsg(opponent_mon_name+" used its Red Card!");
                if(!active_user->hasAbility(SUCTION_CUPS) && !active_user->hasVolatileCondition(INGRAINED)){
                    event_handler->displayMsg(user_mon_name+" was forced to switch!");
                    forceSwitch(actor);
                    return {total_actual_damage,actual_damage.second};
                }else{
                    event_handler->displayMsg("But it failed!");
                }
            }
            if(active_target->hasHeldItem(ABSORB_BULB) && actual_damage.first>0 && !actual_damage.second && attack_type==WATER){
                // Absorb bulb effect
                int modifier = active_target->getSpecialAttackModifier();
                if(!((modifier == MAX_MODIFIER && !active_target->hasAbility(CONTRARY))||
                    (modifier == MIN_MODIFIER && active_target->hasAbility(CONTRARY)))){
                    active_target->consumeHeldItem();
                    // event_handler->displayMsg(opponent_mon_name+"'s Absorb Bulb triggers!");
                    StatCV changes = {{3,1}};
                    changeStats(otherBattleActionActor(actor),changes,false);
                }
            }
            if(active_target->hasHeldItem(LUMINOUS_MOSS) && actual_damage.first>0 && !actual_damage.second && attack_type==WATER){
                // Luminous moss effect
                int modifier = active_target->getSpecialDefenseModifier();
                if(!((modifier == MAX_MODIFIER && !active_target->hasAbility(CONTRARY))||
                    (modifier == MIN_MODIFIER && active_target->hasAbility(CONTRARY)))){
                    active_target->consumeHeldItem();
                    // event_handler->displayMsg(opponent_mon_name+"'s Luminous Moss triggers!");
                    StatCV changes = {{4,1}};
                    changeStats(otherBattleActionActor(actor),changes,false);
                }
            }
            if(active_target->hasHeldItem(CELL_BATTERY) && actual_damage.first>0 && !actual_damage.second && attack_type==ELECTRIC){
                // Cell Battery effect
                int modifier = active_target->getAttackModifier();
                if(!((modifier == MAX_MODIFIER && !active_target->hasAbility(CONTRARY))||
                    (modifier == MIN_MODIFIER && active_target->hasAbility(CONTRARY)))){
                    active_target->consumeHeldItem();
                    // event_handler->displayMsg(opponent_mon_name+"'s Cell Battery triggers!");
                    StatCV changes = {{1,1}};
                    changeStats(otherBattleActionActor(actor),changes,false);
                }
            }
            if(active_target->hasHeldItem(SNOWBALL) && actual_damage.first>0 && !actual_damage.second && attack_type==ICE){
                // Snowball effect
                int modifier = active_target->getAttackModifier();
                if(!((modifier == MAX_MODIFIER && !active_target->hasAbility(CONTRARY))||
                    (modifier == MIN_MODIFIER && active_target->hasAbility(CONTRARY)))){
                    active_target->consumeHeldItem();
                    StatCV changes = {{1,1}};
                    changeStats(otherBattleActionActor(actor),changes,false);
                }
            }
            if(active_target->hasHeldItem(SNOWBALL) && actual_damage.first>0 && !actual_damage.second && effectiveness>1.1){
                // weakness policy effect
                int modifier1 = active_target->getAttackModifier();
                int modifier2 = active_target->getSpecialAttackModifier();
                if(!((modifier1 == MAX_MODIFIER && modifier2 == MAX_MODIFIER && !active_target->hasAbility(CONTRARY))||
                    (modifier1 == MIN_MODIFIER && modifier2 == MIN_MODIFIER && active_target->hasAbility(CONTRARY)))){
                    active_target->consumeHeldItem();
                    StatCV changes = {{1,2},{3,2}};
                    changeStats(otherBattleActionActor(actor),changes,false);
                }
            }
        }
    }else{
        // STATUS MOVES
        //active_user->setLastAttackUsed(attack->getId());
        last_attack_used_id = attack->getId();
        active_user->removeVolatileCondition(LASER_FOCUS);
        BattleActionActor other_actor = otherBattleActionActor(actor);
        if((active_target->hasAbility(MAGIC_BOUNCE)||active_target->hasVolatileCondition(MAGIC_COATED)) && attack->isReflectable()){
            event_handler->displayMsg(opponent_mon_name+" bounces the attack back!");
            active_target = getActorBattler(actor);
            opponent_mon_name = getActorBattlerName(actor);
            other_actor = actor;
            actor = otherBattleActionActor(actor);
            active_user = getActorBattler(actor);
        }
        if(attack->getEffectTarget() == TARGET_OPPONENT && active_target->isFainted()){
            // check if target is already fainted
            return {0,false};
        }
        // SOUNDPROOF PREVENTS SOUND BASED MOVES EFFECTS
        if(active_target->hasAbility(SOUNDPROOF) && 
            attack->isSoundBased() &&
            attack->getEffectTarget() == TARGET_OPPONENT){
            event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
            active_user->setLastAttackFailed();
            return {0,active_target->hasSubstitute()};
        }
        // OVERCOAT PREVENTS POWDER BASED MOVES EFFECTS
        if((active_target->hasAbility(OVERCOAT) || active_target->hasHeldItem(SAFETY_GOGGLES)) && 
            attack->isPowder() &&
            attack->getEffectTarget() == TARGET_OPPONENT){
            event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
            active_user->setLastAttackFailed();
            return {0,active_target->hasSubstitute()};
        }
        //GROUND TYPES ARE IMMUNE TO THUNDER WAVE
        if(attack->getId()==115 && 
            active_target->hasType(GROUND) &&
            !active_target->hasHeldItem(RING_TARGET)){
            event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
            active_user->setLastAttackFailed();
            return {0,active_target->hasSubstitute()};
        }
        bool hits_substitute = false;
        if(active_target->hasSubstitute() && attack->getEffectTarget() == TARGET_OPPONENT){
            // hitting substitute blocks effects
            hits_substitute = true;
        }
        if(active_user->hasAbility(INFILTRATOR))
            hits_substitute = false;
        if(attack->isSoundBased())
            hits_substitute = false;
        applyAttackEffect(attack,actor,other_actor,hits_substitute);
    }
    return {total_actual_damage,actual_damage.second};
}

void Battle::applyRecoil(Attack* attack,unsigned int actual_damage,BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    unsigned int effect = attack->getEffectId();
    if(active_user->isFainted())
        return;
    //Magic Guard and Rock Head prevent non-struggle recoil
    if((active_user->hasAbility(MAGIC_GUARD) ||
        active_user->hasAbility(ROCK_HEAD)) && 
        effect !=11 && //struggle recoil
        effect != 97 && // heal
        effect != 162 && // heal
        effect != 100 && // death after use
        effect != 275 && //death after use
        effect != 117) // death after use
        return;
    if(actual_damage>0){
        switch(effect){
            case 6:case 229:{//recoil 1/4 damage dealt
                unsigned int recoil_damage = max(actual_damage / 4,1);
                if(effect==229)//recoil 1/2 damage dealt
                    recoil_damage = max(actual_damage / 2,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
                if(active_user->isFainted()){
                    return;
                }
                break;
            }
            case 11:{ // Struggle
                unsigned int recoil_damage = max(active_user->getMaxHP() / 4,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
                if(active_user->isFainted())
                    return;
                break;
            }
            case 20:case 30:case 51:case 296:{//recoil 1/3 damage dealt
                unsigned int recoil_damage = max(actual_damage / 3,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
                if(active_user->isFainted())
                    return;
                break;
            }
            case 97:case 162:{//heal 50% of damage dealt (negative recoil)
                unsigned int heal_amount = max((actual_damage+1) / 2,1);
                if(active_user->hasHeldItem(BIG_ROOT))
                    heal_amount *= 1.3;
                if(!active_target->hasAbility(LIQUID_OOZE)){
                    unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                    if(actual_heal_amount>0)
                        event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
                }else{
                    unsigned int actual_damage_amount = active_user->addDirectDamage(heal_amount);
                    // event_handler->displayMsg(opponent_mon_name+"'s Liquid Ooze triggers!");
                    if(actual_damage_amount>0)
                        // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage_amount)+" damage!");
                        event_handler->displayDmgDealt(actual_damage_amount,active_user,opponent_mon_name+"'s Liquid Ooze");
                    if(active_user->isFainted())
                        return;
                }
                break;
            }
            case 100:case 117:case 275:{//user faints
                unsigned int max_hp = active_user->getMaxHP();
                active_user->addDirectDamage(max_hp);
                return;
            }
            case 212:{
                // 1/2 user HP recoil
                unsigned int recoil_damage = max(active_user->getMaxHP() / 2,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
                if(active_user->isFainted())
                    return;
                break;
            }
            case 276:{
                //recoil 50% user health
                unsigned int recoil_damage = max(active_user->getMaxHP() / 2,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                event_handler->displayDmgDealt(actual_recoil_damage,active_user,"recoil");
                if(active_user->isFainted())
                    return;
                break;
            }
            default: break;
        }
    }
    // apply item recoil
    ItemType held_item = active_user->getHeldItem();
    switch(held_item){
        case LIFE_ORB:{
            if(actual_damage == 0)
                break;
            unsigned int recoil_damage = max(active_user->getMaxHP() / 10,1);
            unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" damage from Life Orb!");
            event_handler->displayDmgDealt(actual_recoil_damage,active_user,"its Life Orb");
            if(active_user->isFainted())
                return;
            break;
        }
        default: break;
    }
}



//     if(effect_is_applied){
void Battle::applyAttackEffect(Attack* attack,BattleActionActor actor,BattleActionActor other_actor, bool hits_substitute){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(other_actor);
    unsigned int effect = attack->getEffectId();
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(other_actor);
    Type attack_type = attack->getType(active_user,field);
    MonsterTeam* user_team = getActorTeam(actor);
    MonsterTeam* target_team = getActorTeam(other_actor);
    switch(effect){
        case 1:case 247: { // lower attack by 1 opponent
            //in case of 247, user heals attack stat HP
            if(active_target->isFainted())
                return;
            unsigned int attack_stat = active_target->getModifiedAttack();
            // bool res = active_target->changeAttackModifier(-1);
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-1}};
            changeStats(other_actor,changes,false);      
            if(effect==247){
                //heal attack_stat HP of user
                unsigned int heal_amount = max(attack_stat,1);
                if(active_user->hasHeldItem(BIG_ROOT))
                    heal_amount *= 1.3;
                if(!active_user->hasAbility(LIQUID_OOZE)){
                    unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                    if(actual_heal_amount>0)
                        event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
                }else{
                    unsigned int actual_damage_amount = active_user->addDirectDamage(heal_amount);
                    // event_handler->displayMsg(opponent_mon_name+"'s Liquid Ooze triggers!");
                    if(actual_damage_amount>0){
                        // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage_amount)+" damage!");
                        event_handler->displayDmgDealt(actual_damage_amount,active_user,opponent_mon_name+"'s Liquid Ooze");
                    }
                    if(active_user->isFainted())
                        return;
                }
            }
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            break;
        }
        case 46:{ // lower attack by 2 opponent
            // bool res = active_target->changeAttackModifier(-2);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-2}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 2:{// increase Att and SpAtk by 1 user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeSpecialAttackModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{3,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 3:{ // Leech Seed
            if(active_target->isFainted())
                return;
            if(active_target->hasType(GRASS)){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
            }else if(active_target->hasVolatileCondition(LEECH_SEED)){
                event_handler->displayMsg(opponent_mon_name+" is already seeded!");
            }else{
                active_target->addVolatileCondition(LEECH_SEED, 5);
            }
            break;
        }
        case 4:case 210:case 222:case 232:case 241:{//poison opponent
            if(active_target->isFainted())
                return;
            if(effect==222){
                if(!hits_substitute || 
                    active_user->hasAbility(INFILTRATOR)){
                    StatCV changes = {{5,-1}};
                    changeStats(other_actor,changes,false);
                }
            }   // active_target->changeSpeedModifier(-1);
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being poisoned!");
                if(attack->getCategory() == STATUS && effect!=222)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from being poisoned!");
                if(attack->getCategory() == STATUS && effect!=222)
                    active_user->setLastAttackFailed();
                break;
            }
            bool result = active_target->setPermanentStatus(POISONED);
            if(!result){
                if(attack->getCategory() == STATUS && effect!=222){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
            }else{
                if(active_target->hasAbility(SYNCHRONIZE) && 
                    !active_user->hasPermanentStatus()){
                    event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                    active_user->setPermanentStatus(POISONED);
                }
            }
            break;
        }
        case 76:case 298:{//bad poison opponent
            if(active_target->isFainted())
                return;
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being badly poisoned!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from being badly poisoned!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            bool result = active_target->setPermanentStatus(BAD_POISON);
            if(!result ){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
            }else{
                if(active_target->hasAbility(SYNCHRONIZE) && 
                    !active_user->hasPermanentStatus()){
                    event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                    active_user->setPermanentStatus(BAD_POISON);
                }
            }
        
            break;
        }
        case 5:{//sleep opponent
            if(active_target->isFainted())
                return;
            if(attack_type==GRASS && active_target->hasType(GRASS)){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from falling asleep!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from falling asleep!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            if(isUproar() && !active_target->hasAbility(SOUNDPROOF)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg(user_mon_name+"But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            bool result;
            unsigned int random_integer = RNG::getRandomInteger(1,3);
            if(random_integer == 1)
                result = active_target->setPermanentStatus(SLEEP_4);
            else if(random_integer == 2)
                result = active_target->setPermanentStatus(SLEEP_3);
            else
                result = active_target->setPermanentStatus(SLEEP_2);
            if(!result && attack->getCategory() == STATUS){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 7:{//-2 evasiveness opponent
            // bool res = active_target->changeEvasionModifier(-2);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{7,-2}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 8:{ // Synthesis
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(HEAL_BLOCKED)){
                event_handler->displayMsg(user_mon_name+" is prevented from healing!");
                active_user->setLastAttackFailed();
                break;
            }
            if(active_user->isAtFullHP()){
                event_handler->displayMsg(user_mon_name+" is already at full health!");
            }else{
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int heal_amount;
                if(field->getWeather() == CLEAR || thereIsaCloudNine()){
                    heal_amount = max((maxHP+1) / 2,1);
                }else if((field->getWeather() == SUN || field->getWeather() == EXTREME_SUN) &&
                    !active_user->hasHeldItem(UTILITY_UMBRELLA)){
                    heal_amount = max((maxHP+2) / 3 * 2,1);
                }else{
                    heal_amount = max((maxHP+3) / 4,1);
                }
                unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
            }
            break;
        }
        case 13:case 281:{ // Worry Seed/Simple Beam
            if(active_target->isFainted())
                return;
            Ability new_ability;
            if(effect==13)
                new_ability = INSOMNIA;
            else if(effect==281)
                new_ability = SIMPLE;
            else
                break;
            if(active_target->hasSubstitute() && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(active_target->hasAbility(new_ability) || 
                active_target->hasHeldItem(ABILITY_SHIELD) || 
                abilityCannotBeChanged(active_target->getAbility())){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
            }else{
                active_target->setAbility(new_ability);
                AbilityItem* ability_data = AbilityItem::getAbilityItem(new_ability);
                event_handler->displayMsg(opponent_mon_name+"'s ability was changed to "+ability_data->getIGName()+"!");
            }
            // neutralizing gas ability may have been changed
            checkMonsterLeavingAbilities(other_actor);
            break;
        }
        case 14:case 20:case 219:case 253:case 274:case 308:{//burn opponent
            if(active_target->isFainted())
                return;
            if(effect==274 && !active_target->hasVolatileCondition(STAT_JUST_RAISED)){
                break;
            }
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being burned!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from being burned!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            if(active_target->hasType(FIRE) && attack_type == FIRE){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            bool res = active_target->setPermanentStatus(BURN);
            if(!res){
                if(attack->getCategory() == STATUS){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
                }
            }else{
                if(active_target->hasAbility(SYNCHRONIZE) && 
                    !active_user->hasPermanentStatus()){
                    event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                    active_user->setPermanentStatus(BURN);
                }
            }
            break;
        }
        case 15:{ //-1 opponent accuracy
            // bool res = active_target->changeAccuracyModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{6,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 16:case 67:case 226:case 296:case 309:{ // paralyze opponent - paralysis
            if(active_target->isFainted())
                return;
            if(active_target->hasType(GRASS) && attack_type == GRASS && attack->getCategory() == STATUS){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
                break;
            }
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from the paralysis!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from the paralysis!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            bool res = active_target->setPermanentStatus(PARALYSIS);
            if(!res){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
            }else{
                if(active_target->hasAbility(SYNCHRONIZE) && 
                    !active_user->hasPermanentStatus()){
                    event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                    active_user->setPermanentStatus(PARALYSIS);
                }
            }
            break;
        }
        case 18:{ //-2 opponent Speed
            // bool res = active_target->changeSpeedModifier(-2);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{5,-2}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 19:{ // Fire Spin
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(FIRESPIN)){
                active_target->addVolatileCondition(FIRESPIN,  active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            // }else{
            //     event_handler->displayMsg(opponent_mon_name+" is already trapped in a Fire Spin!");
            }
            break;
        }
        case 287:{ // magma storm
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(MAGMA_STORM)){
                active_target->addVolatileCondition(MAGMA_STORM,  active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 220:{ // infestation
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(INFESTED)){
                active_target->addVolatileCondition(INFESTED,  active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 158:{ // Whirlpool
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(WHIRLPOOL)){
                active_target->addVolatileCondition(WHIRLPOOL,  active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 74:{ // Sand Tomb
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(SANDTOMB)){
                active_target->addVolatileCondition(SANDTOMB,  active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 21:case 45:case 105:case 141:case 195:case 197:case 279:{//flinch
            if(active_target->isFainted())
                return;
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR))
                return;
            active_target->addVolatileCondition(FLINCH, 2);
            break;
        }
        case 22:{ //-1 opponent Def
            // bool res = active_target->changeDefenseModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{2,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 23:{ //+1 user Def
            // bool res = active_user->changeDefenseModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 29:{//+2 Def user
            // bool res = active_user->changeDefenseModifier(+2);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 24:{// rapid spin
            // bool res = active_user->changeSpeedModifier(+1);
            field->clearFieldEffectsSuchThat(&isFieldEffectClearedByRapidSpin,PLAYER);
            active_user->clearVolatilesSuchThat(&isVolatileConditionClearedByRapidSpin);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 25:case 77:case 113:case 174:case 270:case 299:{ // confuse target
            if(active_target->isFainted())
                return;
            if(effect==270 && !active_target->hasVolatileCondition(STAT_JUST_RAISED))
                break;
            if(active_target->hasVolatileCondition(CONFUSION)){
                if(attack->getCategory()==STATUS){
                    event_handler->displayMsg(opponent_mon_name+" is already confused!");
                    active_user->setLastAttackFailed();
                }
            }else if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                    !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being confused!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from being confused!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                return;
            }else if(field->getTerrain() == MISTY_FIELD){
                if(attack->getCategory() == STATUS && effect==25){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed(); 
                }
            }else{  
                active_target->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
            }
            // bool res = false;
            StatCV changes;
            if(effect==72)//flatter
                changes.push_back({3,1});
                // res = active_target->changeSpecialAttackModifier(1);
            if(effect==113)//swagger
                changes.push_back({1,2});
                // res = active_target->changeAttackModifier(2);
            // if(res && active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                break;
            }
            changeStats(other_actor,changes,false);
            break;
        }
        case 26:{//PROTECT
            if(active_user->isFainted())
                return;
            if(active_user->getConsecutiveProtects() == 0){
                active_user->incrementConsecutiveProtect();
                active_user->addVolatileCondition(PROTECT,5);
            }else{
                unsigned int probability_size = 3 * active_user->getConsecutiveProtects();
                if(0==RNG::getRandomInteger(0,probability_size-1)){
                    active_user->incrementConsecutiveProtect();
                    active_user->addVolatileCondition(PROTECT,5);
                }else{
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    active_user->resetConsecutiveProtect();
                }
            }
            break;
        }
        case 268:{//SPIKY PROTECT
            if(active_user->isFainted())
                return;
            if(active_user->getConsecutiveProtects() == 0){
                active_user->incrementConsecutiveProtect();
                active_user->addVolatileCondition(SPIKY_PROTECT,5);
            }else{
                unsigned int probability_size = 3 * active_user->getConsecutiveProtects();
                if(0==RNG::getRandomInteger(0,probability_size-1)){
                    active_user->incrementConsecutiveProtect();
                    active_user->addVolatileCondition(SPIKY_PROTECT,5);
                }else{
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    active_user->resetConsecutiveProtect();
                }
            }
            break;
        }
        case 27:{ // start rain
            if(field->getWeather()==RAIN || thereIsaCloudNine() || field->weatherCannotChange()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();   
            }else
                field->setWeather(RAIN,active_user->hasHeldItem(DAMP_ROCK)?8:5);
            break;
        }
        case 28:{ // +2 ATT SPATT SPD, -1 DEF SPDEF
            // bool res = active_user->changeAttackModifier(+2);
            // res = res || active_user->changeSpecialAttackModifier(+2);
            // res = res || active_user->changeSpeedModifier(+2);
            // bool res2 = active_user->changeDefenseModifierForced(-1);
            // res2 = res2 || active_user->changeSpecialDefenseModifierForced(-1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            // else if(res2 && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,2},{3,2},{5,2},{2,-1},{4,-1}};
            changeStats(actor,changes,false);
            break;
        }
        case 30:{// + 1 SPEED USER
            // bool res = active_user->changeSpeedModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 31:{// -1 SPDEF opponent
            // bool res = active_target->changeSpecialDefenseModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{4,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 54:{//-2 SPDEF opponent
            // bool res = active_target->changeSpecialDefenseModifier(-2);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{4,-2}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 32:case 300:{// whirlwind/roar/dragontail
            if(active_target->isFainted())
                return;
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory()==STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                return;
            }
            if(active_target->getActor()==OPPONENT  && is_wild_battle){
                if(!active_target->hasAbility(SUCTION_CUPS) &&
                    !active_target->hasVolatileCondition(INGRAINED)){
                    //ends the wild battle
                    is_wild_battle_over = true;
                }else{
                    if(attack->getCategory()==STATUS){
                        event_handler->displayMsg("But it failed!");
                        active_user->setLastAttackFailed();
                    }
                }
                break;
            }
            if(! target_team->hasAliveBackup() || active_target->hasAbility(SUCTION_CUPS) || 
                active_target->hasVolatileCondition(INGRAINED)){
                if(attack->getCategory()==STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
            }else{
                field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,actor);
                if(active_target->hasAbility(REGENERATOR)){
                    //regenerate 1/3 of user HP
                    unsigned int maxHP = active_target->getMaxHP();
                    unsigned int heal_amount = max((maxHP+2) / 3,1);
                    active_target->removeDamage(heal_amount);
                }
                if(active_target->hasAbility(NATURAL_CURE)){
                    //remove permanent status
                    active_target->clearPermanentStatus();
                }
                delete active_target;
                target_team->swapRandomMonster();
                active_target = new Battler(target_team->getActiveMonster(),getActorTeam(other_actor),field,other_actor,event_handler);
                resetOpponents();
                if(actor == PLAYER){
                    opponent_active = active_target;
                }else{
                    player_active = active_target;
                }
                opponent_mon_name = getActorBattlerName(other_actor);
                event_handler->displayMsg(opponent_mon_name+" was forced in!");
                checkMonsterLeavingAbilities(other_actor);
                player_active->addSeenOpponent(opponent_active->getMonster());
                removeVolatilesFromOpponentOfMonsterLeavingField(other_actor);
                applySwitchInFormChange(other_actor);
                if (onTerrainChange(other_actor))
                    return;
                if (applySwitchInAbilitiesEffects(active_target))
                    return;
                if (applySwitchInItemsEffects(other_actor))
                    return;
                if (performEntryHazardCheck(other_actor))
                    return;
                checkUproars();
                if(active_target->isFainted())
                    return;
            }
            break;
        }
        case 33:{// SAFEGUARD
            if(active_user->isFainted())
                return;
            if(field->hasFieldEffect(SAFEGUARD,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(SAFEGUARD, 5, actor);
                event_handler->displayMsg(user_mon_name+"'s side is protected by Safeguard!");
            }
            break;
        }
        case 34:{//TAILWIND
            if(active_user->isFainted())
                return;
            if(field->hasFieldEffect(TAILWIND,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(TAILWIND, 5, actor);
                event_handler->displayMsg("A strong wind starts to blow on "+user_mon_name+"'s side!");
                if(active_user->hasAbility(WIND_RIDER)){
                    // wind rider holders get a speed boost on tailwind activation for their side
                    StatCV changes = {{5,1}};
                    changeStats(actor,changes,false);
                }
            }
            break;
        }
        case 35:{ // user + 1 spatt spdef speed
            // bool res = active_user->changeSpecialAttackModifier(+1);
            // res = res || active_user->changeSpecialDefenseModifier(+1);
            // res = res || active_user->changeSpeedModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,1},{4,1},{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 38:{// laser focus effect (crit next turn)
            if(active_user->isFainted())
                return;
            active_user->addVolatileCondition(LASER_FOCUS, 5);
            break;
        }
        case 39:{//focus energy
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(FOCUS_ENERGY)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(FOCUS_ENERGY, -1);
            }
            break;
        }
        case 41:{//Toxic spikes
            if(field->hasFieldEffect(BAD_TOXIC_SPIKES,other_actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else if(field->hasFieldEffect(TOXIC_SPIKES,other_actor)){
                field->setFieldEffect(BAD_TOXIC_SPIKES, -1, other_actor);
                field->clearFieldEffect(TOXIC_SPIKES,other_actor);
                event_handler->displayMsg(user_mon_name+" scattered toxic spikes towards its opponent!");
            }else{
                field->setFieldEffect(TOXIC_SPIKES, -1, other_actor);
                event_handler->displayMsg(user_mon_name+" scattered toxic spikes towards its opponent!");
            }
            break;
        }
        case 159:{
            //spikes
            if(field->hasFieldEffect(SPIKES_3,other_actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else if(field->hasFieldEffect(SPIKES_2,other_actor)){
                field->setFieldEffect(SPIKES_3, -1, other_actor);
                field->clearFieldEffect(SPIKES_2,other_actor);
                event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
            }else if(field->hasFieldEffect(SPIKES,other_actor)){
                field->setFieldEffect(SPIKES_2, -1, other_actor);
                field->clearFieldEffect(SPIKES,other_actor);
                event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
            }else{
                field->setFieldEffect(SPIKES, -1, other_actor);
                event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
            }
            break;
        }
        case 42:case 240:{// +2 speed user
            if(active_user->isFainted())
                return;
            // bool res = active_user->changeSpeedModifier(+2);
            if(effect==240){
                unsigned int old_weight = active_user->getWeight();
                active_user->changeWeight(-1000);
                if(old_weight != active_user->getWeight())
                    event_handler->displayMsg(user_mon_name+" became nimbler!");
            }
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            StatCV changes = {{5,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 47:{//roost
            if(active_user->isFainted())
                return;
            if(active_user->getCurrentHP() == active_user->getMaxHP()){
                event_handler->displayMsg(user_mon_name+" is already at full health!");
            }else{
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int heal_amount = max((maxHP+1) / 2,1);
                unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
                active_user->landOnGround();
            }
            break;
        }
        case 52:{//+2 ATT user
            // bool res = active_user->changeAttackModifier(+2);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 53:{ // Wrap
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(WRAP)){
                active_target->addVolatileCondition(WRAP, active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 164:{ // Bind
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(BIND)){
                active_target->addVolatileCondition(BIND, active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 304:{ // Snap Trap
            if(active_target->isFainted())
                return;
            if(hits_substitute)
                return;
            if(!active_target->hasVolatileCondition(SNAP_TRAP)){
                active_target->addVolatileCondition(SNAP_TRAP, active_user->hasHeldItem(GRIP_CLAW)?7:(RNG::coinFlip()?4:5));
            }
            break;
        }
        case 55:{ // stockpile
            if(active_user->isFainted())
                return;
            if(active_user->getStockpiles() == 3){
                event_handler->displayMsg(user_mon_name+" cannot stockpile anymore!");
                active_user->setLastAttackFailed();
            }else{
                event_handler->displayMsg(user_mon_name+" is stockpiling!");
                active_user->incrementStockpiles();
                // active_user->changeDefenseModifier(+1);
                // active_user->changeSpecialDefenseModifier(+1);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
            }
            break;
        }
        case 56:{//56 and 57 are moves related to stockpile,
            if(active_user->isFainted())
                return;
            unsigned int heal = 0;  
            if(active_user->isAtFullHP()){
                event_handler->displayMsg(user_mon_name+" is already at full health!");
                active_user->setLastAttackFailed();
                break;
            }
            switch(active_user->getStockpiles()){
                case 0:
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    break;
                case 1:
                    heal = (active_user->getMaxHP() + 3 )/ 4;
                    break;
                case 2:
                    heal = (active_user->getMaxHP() + 1) / 2;
                    break;
                case 3:
                    heal = active_user->getMaxHP();
                    break;
            }
            if(heal>0){
                unsigned int actual_heal_amount = active_user->removeDamage(heal);
                event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP to itself!");
            }
            // NOTICE THAT I DO NOT BREAK HERE!!!
        }case 57:{// spit up
            if(active_user->isFainted())
                return;
            active_user->resetStockpiles();
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            break;
        }
        case 58:{ // suppress ability
            if(active_target->isFainted())
                return;
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            if(active_target->isAbilitySuppressed()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                event_handler->displayMsg(opponent_mon_name+"'s ability was suppressed!");
                active_target->suppressAbility();
            }
            //neutralizing gas may have been suppressed
            checkMonsterLeavingAbilities(other_actor);
            break;
        }
        case 59:{// clear stat changes
            active_user->resetAllStatChanges();
            active_target->resetAllStatChanges();
            event_handler->displayMsg(user_mon_name+"'s haze cleared all stat changes!");
            break;
        }
        case 60:{// +1 ATT DEF ACC user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeDefenseModifier(+1);
            // res = res || active_user->changeAccuracyModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{2,1},{6,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 61:{//freeze opponent
            if(active_target->isFainted())
                return;
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being frozen!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }else if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("The substitute protects "+opponent_mon_name+" from being frozen!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
                break;
            }
            bool result = active_target->setPermanentStatus(FREEZE);
            if(!result && attack->getCategory() == STATUS){
                event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 62:{// +2 spatt user
            // bool res = active_user->changeSpecialAttackModifier(+2);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 63:{//+1 evasiveness user
            // bool res = active_user->changeEvasionModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{7,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 66:{//light screen
            if(field->hasFieldEffect(LIGHT_SCREEN,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(LIGHT_SCREEN, active_user->hasHeldItem(LIGHT_CLAY)?8:5, actor);
                event_handler->displayMsg(user_mon_name+" set up a light screen that reduces damage coming from special attacks!");
            }
            break;
        }
        case 208:{//aurora veil
            if(field->hasFieldEffect(AURORA_VEIL,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(AURORA_VEIL, active_user->hasHeldItem(LIGHT_CLAY)?8:5, actor);
                event_handler->displayMsg(user_mon_name+" set up an aurora screen that reduces damage coming from attacks!");
            }
            break;
        }
        case 69:{//-1 speed opponent
            // bool res = active_target->changeSpeedModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{5,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 72:{//start sandstorm
            if(field->getWeather()==SANDSTORM || thereIsaCloudNine() || field->weatherCannotChange()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else
                field->setWeather(SANDSTORM,active_user->hasHeldItem(SMOOTH_ROCK)?8:5);
            break;
        }
        case 78:{// -1 ATT DEF user
            // bool res = active_user->changeAttackModifierForced(-1);
            // res = res || active_user->changeDefenseModifierForced(-1);
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,-1},{2,-1}};
            changeStats(actor,changes,true);
            break;
        }
        case 79:{//nothing happens
            if(active_user->isFainted())
                return;
            event_handler->displayMsg("But nothing happens!");
            break;
        }
        case 82:{//encore
            if(active_target->isFainted())
                return;
            if(active_target->isFainted() ||
                active_target->hasVolatileCondition(ENCORE) || 
                active_target->getLastAttackUsed() == 0 || 
                active_target->getLastAttackUsed() == STRUGGLE_ID){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->addVolatileCondition(ENCORE, 3);
                Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
                event_handler->displayMsg(opponent_mon_name+" is locked in using "+last_attack->getName()+" for 3 turns!");
            }
            break;
        }
        case 83:
        case 291:{//heal 25%
            //in the case of 291 also heal status
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(HEAL_BLOCKED)){
                event_handler->displayMsg(user_mon_name+" is prevented from healing!");
                active_user->setLastAttackFailed();
                break;
            }
            if(active_user->isAtFullHP()){
                event_handler->displayMsg(user_mon_name+" is already at full health!");
            }else{
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int heal_amount = max((maxHP+3) / 4,1);
                unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP to itself!");
            }
            if(effect == 291){
                // heal all status
                active_user->clearPermanentStatus();
            }
            break;
        }
        case 85:{//GRAVITY
            if(active_user->isFainted())
                return;
            if(!field->setFullFieldEffect(GRAVITY,5)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 86:{// +1 ATT user
            // bool res = active_user->changeAttackModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 87:{// +1 DEF SPDEF user
            // bool res = active_user->changeDefenseModifier(+1);
            // res = res || active_user->changeSpecialDefenseModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,1},{4,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 88:{// -1 SPATT user
            // bool res = active_user->changeSpecialAttackModifierForced(-1);
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,-1}};
            changeStats(actor,changes,true);
            break;
        }
        case 89:case 290:{
            // user dies entry Monster is fully healed
            // only in the case of 290, all PP of attacks are restored
            if(active_user->isFainted())
                return;
            if(!user_team->hasAliveBackup()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                // faint active user
                unsigned int maxHP_old = active_user->getMaxHP();
                unsigned int actual_damage = active_user->addDirectDamage(maxHP_old);
                // event_handler->displayMsg(user_mon_name+" inflicted to itself "+std::to_string(actual_damage)+" damage!");
                checkForExp();
                delete active_user;
                field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,actor);
                if(actor == PLAYER){
                    unsigned int new_active_index = event_handler->chooseSwitchForced(player_team);
                    player_team->swapActiveMonster(new_active_index);
                    player_active = new Battler(player_team->getActiveMonster(),player_team,field,actor,event_handler);
                    active_user = player_active;
                    event_handler->displayMsg("Player switched in "+player_active->getNickname());
                }else{
                    unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
                    opponent_team->swapActiveMonster(new_active_index);
                    opponent_active = new Battler(player_team->getActiveMonster(),opponent_team,field,actor,event_handler);
                    active_user = opponent_active;
                    event_handler->displayMsg("Opponent switched in "+opponent_active->getNickname());
                }
                resetOpponents();
                checkMonsterLeavingAbilities(actor);
                removeVolatilesFromOpponentOfMonsterLeavingField(actor);
                // heal active user
                user_mon_name = getActorBattlerName(actor);
                unsigned int maxHP = active_user->getMaxHP();
                active_user->removeDamage(maxHP);
                active_user->clearPermanentStatus();
                if(effect==290){
                    auto attacks = active_user->getAttacks();
                    for(auto attack_data : attacks){
                        active_user->recoverPP(attack_data.first,1000);
                    }
                }
                event_handler->displayMsg(user_mon_name+" was fully healed!");
                player_active->addSeenOpponent(opponent_active->getMonster());
                applySwitchInFormChange(actor);
                if(applySwitchInAbilitiesEffects(active_user))
                    return;
                if(applySwitchInItemsEffects(actor))
                    return;
                if(performEntryHazardCheck(actor))
                    return;
                checkUproars();
                if(active_user->isFainted())
                    return;
            }
            break;
        }
        case 90:{// disable last move used by opponenet
            if(active_target->isFainted())
                return;
            Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
            if(active_target->hasDiabledAttack() || last_attack == nullptr){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->disableAttack(last_attack->getId(),5);
                event_handler->displayMsg(opponent_mon_name+"'s "+last_attack->getName()+" was disabled!");
            }
            break;
        }
        case 91:case 236:{
            // 91: remove 4 PP from opponents last move used
            //236: remove 3 PP
            if(active_target->isFainted())
                return;
            unsigned int pp_amount;
            if(effect == 91)
                pp_amount = 4;
            else
                pp_amount = 3;
            Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
            if(last_attack == nullptr || ! active_target->hasPP(last_attack->getId())){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
            }else{
                active_target->usePP(last_attack->getId(),pp_amount);
                event_handler->displayMsg(opponent_mon_name+"'s "+last_attack->getName()+" lost some PP!");
            }
            break;
        }
        case 92:{//imprison
            if(active_target->isFainted())
                return;
            if(active_user->hasVolatileCondition(IMPRISON)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(IMPRISON, -1);
            }
            break;
        }
        case 93:{//Mimic
            if(active_user->isFainted())
                return;
            Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
            if(last_attack==nullptr || !last_attack->canBeSketched() || last_attack->getId()==TRANSFORM_ID){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(!active_user->setMimickedAttack(active_target->getLastAttackUsed())){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                event_handler->displayMsg(user_mon_name+" mimicked "+opponent_mon_name+"'s "+last_attack->getName()+"!");
            }
            break;
        }
        case 95:{//rest
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(HEAL_BLOCKED)){
                event_handler->displayMsg(user_mon_name+" is prevented from healing!");
                active_user->setLastAttackFailed();
                break;
            }
            if(isUproar() && !active_user->hasAbility(SOUNDPROOF)){
                event_handler->displayMsg(user_mon_name+" But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            unsigned int maxHP = active_user->getMaxHP();
            active_user->setPermanentStatusForced(SLEEP_3);
            event_handler->displayMsg(user_mon_name+" was fully healed!");
            active_user->removeDamage(maxHP);
            break;
        }
        case 96:{//mean look
            if(active_target->isFainted())
                return;
            if(active_target->hasVolatileCondition(MEAN_LOOK) || active_target->hasType(GHOST)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->addVolatileCondition(MEAN_LOOK, -1);
            }
            break;
        }
        case 98:{//quick guard
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(QUICK_GUARD)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(QUICK_GUARD, 5);
                event_handler->displayMsg(user_mon_name+" is protecting its team from priority attacks!");
            }
            break;
        }
        case 99:{//set up grassy terrain
            if(field->getTerrain() == GRASSY_FIELD){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setTerrain(GRASSY_FIELD,active_user->hasHeldItem(TERRAIN_EXTENDER)?8:5);
                // consumeSeeds();
            }
            break;
        }
        case 102:{//transform
            if(active_user->isFainted())
                return;
            if(active_target->isFainted()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(active_target->hasSubstitute()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            active_user->transformInto(active_target->getMonster());
            event_handler->displayMsg(user_mon_name+" transformed into "+opponent_mon_name+"!");
            break;
        }
        case 104:{//tri attack
            if(active_target->isFainted())
                return;
            if(!active_target->hasPermanentStatus()){
                if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    break;
                }
                unsigned int random_number = RNG::getRandomInteger(1,3);
                if(random_number == 1 && active_target->canBeBurned() 
                    && !active_target->hasType(FIRE)){// try to burn
                    active_target->setPermanentStatus(BURN);
                    event_handler->displayMsg(opponent_mon_name+" was burned!");
                    if(active_target->hasAbility(SYNCHRONIZE) && 
                        !active_user->hasPermanentStatus()){
                        event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                        active_user->setPermanentStatus(BURN);
                    }
                }else if(random_number==2 && 
                    active_target->canBeParalyzed()){ //try to paralyze
                    active_target->setPermanentStatus(PARALYSIS);
                    event_handler->displayMsg(opponent_mon_name+" was paralyzed!");
                    if(active_target->hasAbility(SYNCHRONIZE) && 
                        !active_user->hasPermanentStatus()){
                        event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
                        active_user->setPermanentStatus(PARALYSIS);
                    }
                }else if(active_target->canBeFrozen() && 
                    !active_target->hasType(ICE)){
                    active_target->setPermanentStatus(FREEZE);
                    event_handler->displayMsg(opponent_mon_name+" was frozen solid!");
                }
            }
            break;
        }
        case 106:{//taunt
            if(active_target->isFainted())
                return;
            if(!active_target->hasVolatileCondition(TAUNTED))
                event_handler->displayMsg(opponent_mon_name+" was taunted!");
            else{
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 234:{//torment
            if(active_target->isFainted())
                return;
            if(!active_target->hasVolatileCondition(TORMENTED))
                event_handler->displayMsg(opponent_mon_name+" was tormented! It wiil be unable to use the same move in twice in a row!");
            else{
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 107:{//turn opponent into water type
            if(active_target->isFainted())
                return;
            auto types = active_target->getTypes();
            if(types.size() == 1 && active_target->hasType(WATER)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->changeType(WATER);
                event_handler->displayMsg(opponent_mon_name+" was turned into a water type!");
            }
            break;
        }
        case 108:{//copy enemy stat changes
            int attack_mod = active_target->getAttackModifier();
            int special_attack_mod = active_target->getSpecialAttackModifier();
            int defense_mod = active_target->getDefenseModifier();
            int special_defense_mod = active_target->getSpecialDefenseModifier();
            int speed_mod = active_target->getSpeedModifier();
            int accuracy_mod = active_target->getAccuracyModifier();
            int evasion_mod = active_target->getEvasionModifier();
            if(!attack_mod && !special_attack_mod && !defense_mod &&
                !special_defense_mod && !speed_mod && !accuracy_mod && !evasion_mod){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->setAttackModifier(attack_mod);
                active_user->setSpecialAttackModifier(special_attack_mod);
                active_user->setDefenseModifier(defense_mod);
                active_user->setSpecialDefenseModifier(special_defense_mod);
                active_user->setSpeedModifier(speed_mod);
                active_user->setAccuracyModifier(accuracy_mod);
                active_user->setEvasionModifier(evasion_mod);
                event_handler->displayMsg(user_mon_name+" copied "+opponent_mon_name+"'s stat changes!");
            }
            break;
        }
        case 109:{//+2 spdef user
            // bool res = active_user->changeSpecialDefenseModifier(+2);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{4,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 110:{//wonder room
            if(active_user->isFainted())
                return;
            if(field->hasFullFieldEffect(WONDER_ROOM)){
                field->clearFullFieldEffect(WONDER_ROOM);
            }else{
                field->setFullFieldEffect(WONDER_ROOM, 5);
            }
            break;
        }
        case 115:{
            //-1 def spdef user
            // bool res = active_user->changeDefenseModifierForced(-1);
            // res = res || active_user->changeSpecialDefenseModifierForced(-1);
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,-1},{4,-1}};
            changeStats(actor,changes,true);
            break;
        }
        case 121:{
            //user halves its HP and then maximizes its attack
            if(active_user->isFainted())
                return;
            unsigned int maxHP = active_user->getMaxHP();
            unsigned int currentHP = active_user->getCurrentHP();
            unsigned int half_hp = max(1,maxHP/2);
            if(currentHP <= half_hp){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addDirectDamage(half_hp);
                event_handler->displayMsg(user_mon_name+" halved its HP!");
                // bool res = active_user->changeAttackModifier(12);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                StatCV changes = {{1,12}};
                changeStats(actor,changes,false);
            }
            break;
        }
        case 122:{
            //-1 speed opponent
            // bool res = active_target->changeSpeedModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{5,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 123:{
            // target is forced to switch
            if(active_target->isFainted())
                return;
            if(hits_substitute &&
                !active_user->hasAbility(INFILTRATOR)){
                // event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(is_wild_battle && active_target->getActor()==OPPONENT){
                if(!active_target->hasAbility(SUCTION_CUPS) && 
                    !active_target->hasVolatileCondition(INGRAINED)){
                    //ends the wild battle
                    is_wild_battle_over = true;
                }else if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(!target_team->hasAliveBackup() || 
                active_target->hasAbility(SUCTION_CUPS) ||
                active_target->hasVolatileCondition(INGRAINED)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            //force opponent to switch
            forceSwitch(other_actor);
            active_target = getActorBattler(other_actor);
            checkUproars();
            if(active_target->isFainted())
                return;
            break;
        }
        case 124:case 231:case 235:case 284:{
            // user switches out, 
            // fails if trapped in the case of 124
            // start snow in case of 235
            if(active_user->isFainted())
                return;
            if(effect==124 && is_wild_battle && active_user->getActor()==OPPONENT){
                if(active_user->canSwitchOut(active_target)){
                    is_wild_battle_over = true;
                    return;
                }else{
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(effect==284){
                StatCV changes = {{1,-1},{3,-1}};
                changeStats(other_actor,changes,true);
            }
            if(!user_team->hasAliveBackup()){
                if(attack->getCategory() == STATUS && effect!=284){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(!active_user->canSwitchOut(active_target) && effect==124){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(effect==235 && field->getWeather()!=SNOWSTORM  && 
                !thereIsaCloudNine() && !field->weatherCannotChange()){
                event_handler->displayMsg(user_mon_name+" started a snowstorm!");
                field->setWeather(SNOWSTORM,active_user->hasHeldItem(ICY_ROCK)?8:5);
            }
            //force user to switch
            forceSwitch(actor);
            active_user = getActorBattler(actor);
            checkUproars();
            if(active_user->isFainted())
                return;
            break;
        }
        case 125:{
            //reflect
            if(field->hasFieldEffect(REFLECT,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(REFLECT, active_user->hasHeldItem(LIGHT_CLAY)?8:5, actor);
                event_handler->displayMsg(user_mon_name+" sets up a barrier that reduces damage coming from physical attacks!");
            }
            break;
        }
        case 126:{
            //heal 50% HP
            if(active_user->isFainted())
                return;
            if(active_user->isAtFullHP()){
                event_handler->displayMsg(user_mon_name+" is already at full health!");
                active_user->setLastAttackFailed();
            }else{
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int heal_amount = max((maxHP+1) / 2,1);
                unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
            }
            break;
        }
        case 127:{
            // copy target ability
            if(active_target->isFainted() || active_user->isFainted())
                return;
            if(active_user->getAbility() == active_target->getAbility() ||
                active_target->hasAbility(NO_ABILITY) ||
                active_user->hasAbility(NO_ABILITY) ||
                abilityCannotBeChanged(active_user->getAbility()) ||
                abilityCannotBeCopied(active_target->getAbility()) ||
                active_user->hasHeldItem(ABILITY_SHIELD)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                event_handler->displayMsg(user_mon_name+" copied "+opponent_mon_name+"'s ability!");
                active_user->setAbility(active_target->getAbility());
            }
            break;
        }
        case 129:{
            //+1 spatt spdef user
            // bool res = active_user->changeDefenseModifier(+1);
            // res = res || active_user->changeSpecialDefenseModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,1},{4,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 132:{
            //+1 att def user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeDefenseModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{2,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 133:{
            // -2 SPATT user
            // bool res = active_user->changeSpecialAttackModifierForced(-2);
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,-2}};
            changeStats(actor,changes,true);
            break;
        }
        case 137:{
            //user becomes same type as target
            if(active_user->isFainted())
                return;
            auto types = active_target->getTypes();
            if(active_target->isFainted() || types.size()==0){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }else{
                active_user->clearTypes();
                for(auto type : types){
                    active_user->addType(type);
                }
                event_handler->displayMsg(user_mon_name+" became the same type as "+opponent_mon_name+"!");
            }
            break;
        }
        case 138:{
            //smacks target down
            if(active_target->isFainted())
                return;
            if(hits_substitute &&
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(active_target->hasVolatileCondition(SMACKED_DOWN)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
            }else{
                active_target->addVolatileCondition(SMACKED_DOWN, 5);
                event_handler->displayMsg(opponent_mon_name+" was smacked down!");
                if(active_target->hasVolatileCondition(MAGNET_RISE))
                    active_target->removeVolatileCondition(MAGNET_RISE);
            }
            break;
        }
        case 139:{
            //stealth rocks
            if(field->hasFieldEffect(STEALTH_ROCKS,other_actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(STEALTH_ROCKS, -1, other_actor);
                event_handler->displayMsg("Pointed rocks float in the air at "+opponent_mon_name+"'s side!");
            }
            break;
        }
        case 221:{
            //sticky web
            if(field->hasFieldEffect(STICKY_WEB,other_actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(STICKY_WEB, -1, other_actor);
                event_handler->displayMsg("A sticky web slows down all Pokemon entering "+opponent_mon_name+"'s side!");
            }
            break;
        }
        case 142:
        case 305:{
            //+1 spd user
            // bool res = active_user->changeSpeedModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 143:{
            //opponent drowsy
            if(active_target->isFainted())
                return;
            if(field->hasFieldEffect(SAFEGUARD,other_actor) &&
                !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("Safeguard protects "+opponent_mon_name+"!");
                if(attack->getCategory() == STATUS)
                    active_user->setLastAttackFailed();
            }else if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
            }else if(!active_target->canFallAsleep() ||
                active_target->hasVolatileCondition(DROWSY)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
            }else{
                active_target->addVolatileCondition(DROWSY, 2);
            }
            break;
        }
        case 144:{
            //curse
            if(active_user->hasType(GHOST)){
                if(active_target->isFainted())
                    return;
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int actual_damage = active_user->addDirectDamage(maxHP / 2);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage to send a curse to its opponent!");
                event_handler->displayMsg(user_mon_name+" lost "+std::to_string(actual_damage)+" HP to send a curse to "+opponent_mon_name+"!");
                if(active_target->hasVolatileCondition(CURSED)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->addVolatileCondition(CURSED, -1);
                }
            }else{
                // bool res1 = active_user->changeSpeedModifierForced(-1);
                // bool res2 = active_user->changeAttackModifier(+1);
                // res2 = res2 || active_user->changeDefenseModifier(+1);
                // if(res2 && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                // else if(res1 && !active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                if(active_user->isFainted())
                    return;
                StatCV changes = {{1,1},{2,1},{5,-1}};
                changeStats(actor,changes,true);
            }
            break;
        }
        case 145:{
            //heal 50% target
            if(active_target->isFainted())
                return;
            if(active_target->isAtFullHP()){
                event_handler->displayMsg(opponent_mon_name+" is already at full health!");
                active_user->setLastAttackFailed();
            }else{
                unsigned int maxHP = active_target->getMaxHP();
                unsigned int heal_amount;
                if(active_user->hasAbility(MEGA_LAUNCHER) && attack->isPulse())
                    //mega launcher also powers up heal pulse
                    heal_amount = max((maxHP*3+3) / 4,1);
                else
                    heal_amount = max((maxHP+1) / 2,1);
                unsigned int actual_heal_amount = active_target->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(opponent_mon_name+" was healed by "+std::to_string(actual_heal_amount)+" HP!");
            }
            break;
        }
        case 146:{
            //infatuation opponent
            if(active_target->isFainted())
                return;
            Gender user_gender = active_user->getGender();
            Gender target_gender = active_target->getGender();
            if(active_target->hasAbility(OBLIVIOUS) ||
                !areMaleAndFemale(user_gender,target_gender) ||
                active_target->hasVolatileCondition(INFATUATION)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->addVolatileCondition(INFATUATION, -1);
                if(active_target->hasHeldItem(DESTINY_KNOT))
                    active_user->addVolatileCondition(INFATUATION, -1);
            }
            break;
        }
        case 147:{
            //lock on target
            if(active_target->isFainted())
                return;
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(active_user->hasLockOn(active_target->getMonster())){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addLockOn(active_target->getMonster());
            }
            break;
        }
        case 148:{
            //electric terrain
            if(field->getTerrain() == ELECTRIC_FIELD){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setTerrain(ELECTRIC_FIELD,active_user->hasHeldItem(TERRAIN_EXTENDER)?8:5);
                // consumeSeeds();
            }
            break;
        }
        case 273:{
            //psychic terrain
            if(field->getTerrain() == PSYCHIC_FIELD){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setTerrain(PSYCHIC_FIELD,active_user->hasHeldItem(TERRAIN_EXTENDER)?8:5);
                // consumeSeeds();
            }
            break;
        }
        case 149:{
            //magnet rise
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(MAGNET_RISE) || 
                field->hasFullFieldEffect(GRAVITY)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(MAGNET_RISE, 5);
                event_handler->displayMsg(user_mon_name+" raised from the ground through magnetism!");
                if(active_target->hasVolatileCondition(SMACKED_DOWN))
                    active_target->removeVolatileCondition(SMACKED_DOWN);
            }
            break;
        }
        case 151:{
            //uproar
            if(active_user->isFainted())
                return;
            event_handler->displayMsg(user_mon_name+" is making an uproar!");
            uproaring_monsters.insert(active_user->getMonster());
            break;
        }
        case 152:{
            //raise a random stat by +2
            if(active_user->isFainted())
                return;
            std::vector<int> options;
            if(active_user->getAttackModifier() < MAX_MODIFIER)
                options.push_back(1);
            if(active_user->getSpecialAttackModifier() < MAX_MODIFIER)
                options.push_back(2);
            if(active_user->getDefenseModifier() < MAX_MODIFIER)
                options.push_back(3);
            if(active_user->getSpecialDefenseModifier() < MAX_MODIFIER)
                options.push_back(4);
            if(active_user->getSpeedModifier() < MAX_MODIFIER)
                options.push_back(5);
            if(active_user->getAccuracyModifier() < MAX_MODIFIER)
                options.push_back(6);
            if(active_user->getEvasionModifier() < MAX_MODIFIER)
                options.push_back(7);
            if(options.size() == 0){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            int chosen_option = options[RNG::getRandomInteger(0,options.size()-1)];
            StatCV changes = {{chosen_option,2}};
            changeStats(actor,changes,false);
            break;
        }
        case 153:{
            //start snowstorm
            if(field->getWeather() == SNOWSTORM || thereIsaCloudNine() || field->weatherCannotChange()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else
                field->setWeather(SNOWSTORM,active_user->hasHeldItem(ICY_ROCK)?8:5);
            break;
        }
        case 154:{
            //aqua ring
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(AQUA_RING)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(AQUA_RING, -1);
                event_handler->displayMsg(user_mon_name+" is surrounded by a veil of water!");
            }
            break;
        }
        case 157:{
            //memento
            if(active_user->isFainted())
                return;
            if(active_target->hasVolatileCondition(PROTECT) || 
                active_target->hasVolatileCondition(SPIKY_PROTECT)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            //user faints
            active_user->addDirectDamage(active_user->getMaxHP());
            // opponent stats are lowered
            // bool res = active_target->changeAttackModifier(-12);
            // res = res || active_target->changeSpecialAttackModifier(-12);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-12},{3,-12}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 161:{
            //destiny bond
            if(active_user->isFainted())
                return;
            int times_used = active_user->getSameAttackCounter();
            if(times_used==1){
                active_user->addVolatileCondition(DESTINY_BOND,3);
            }else if(times_used==2){
                if(RNG::getRandomInteger(0,2)==0){
                    active_user->addVolatileCondition(DESTINY_BOND,3);
                }else{
                    active_user->setLastAttackFailed();
                    event_handler->displayMsg("But it failed!");
                }
            }else{
                if(RNG::oneEight()){
                    active_user->addVolatileCondition(DESTINY_BOND,3);
                }else{
                    active_user->setLastAttackFailed();
                    event_handler->displayMsg("But it failed!");
                }
            }
            break;
        }
        case 163:{
            //perish song
            if(active_user->isFainted())
                return;
            event_handler->displayMsg("All Pokèmon that hear "+user_mon_name+"'s song will faint in 3 turns!");
            if(!active_user->hasVolatileCondition(PERISH_SONG))
                active_user->addVolatileCondition(PERISH_SONG,4);
            if(!active_target->hasVolatileCondition(PERISH_SONG) && 
                !active_target->isFainted() &&
                !active_target->hasAbility(SOUNDPROOF))
                active_target->addVolatileCondition(PERISH_SONG,4);
            break;
        }
        case 166:{
            //-1 speed user
            // bool res = active_user->changeSpeedModifierForced(-1);
            // if(res && !active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{5,-1}};
            changeStats(actor,changes,true);
            break;
        }
        case 167:{
            // charge user, +1 spdef user
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(CHARGED)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(CHARGED, -1);
                // bool res = active_user->changeSpecialDefenseModifier(+1);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                StatCV changes = {{4,1}};
                changeStats(actor,changes,false);
            }
            break;
        }
        case 169:{
            // +1 SPATT user
            // bool res = active_user->changeSpecialAttackModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 170:{
            // +1 DEF SPDEF user iff ability is PLUS or MINUS
            if(active_user->isFainted())
                return;
            if(active_user->hasAbility(PLUS) || 
                active_user->hasAbility(MINUS)){
                // bool res = active_user->changeDefenseModifier(+1);
                // res = res || active_user->changeSpecialDefenseModifier(+1);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                StatCV changes = {{2,1},{4,1}};
                changeStats(actor,changes,false);
            }else{
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 303:{
            // +1 ATT SPATT user iff ability is PLUS or MINUS
            if(active_user->isFainted())
                return;
            if(active_user->hasAbility(PLUS) || 
                active_user->hasAbility(MINUS)){
                // bool res = active_user->changeDefenseModifier(+1);
                // res = res || active_user->changeSpecialDefenseModifier(+1);
                // if(res && active_user->hasAbility(CONTRARY))
                //     tryEjectPack(actor);
                StatCV changes = {{1,1},{3,1}};
                changeStats(actor,changes,false);
            }else{
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 173:{
            //user endures all hits this turn
            if(active_user->isFainted())
                return;
            active_user->addVolatileCondition(ENDURE,5);
            break;
        }
        case 177:{
            //reset target stat changes
            if(hits_substitute &&
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    active_user->setLastAttackFailed();
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                }
                break;
            }
            active_target->setAccuracyModifier(0);
            active_target->setEvasionModifier(0);
            active_target->setAttackModifier(0);
            active_target->setDefenseModifier(0);
            active_target->setSpecialAttackModifier(0);
            active_target->setSpecialDefenseModifier(0);
            active_target->setSpeedModifier(0);
            break;
        }
        case 179:{
            //ingrain
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(INGRAINED)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_user->addVolatileCondition(INGRAINED, -1);
            }
            break;
        }
        case 180:{
            // +1 ATT DEF SPATT SPDEF SPEED user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeDefenseModifier(+1);
            // res = res || active_user->changeSpecialAttackModifier(1);
            // res = res || active_user->changeSpecialDefenseModifier(1);
            // res = res || active_user->changeSpeedModifier(1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{2,1},{3,1},{4,1},{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 181:{
            //-1 att def target
            // bool res = active_target->changeAttackModifier(-1);
            // res = res || active_target->changeDefenseModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-1},{2,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 182:{
            //+1 att speed user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeSpeedModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 183:{
            //switch att spatt with opponent
            if(active_target->isFainted())
                return;
            int old_user_att = active_user->getAttackModifier();
            int old_user_spatt = active_user->getSpecialAttackModifier();
            int old_target_att = active_target->getAttackModifier();
            int old_target_spatt = active_target->getSpecialAttackModifier();
            active_user->setAttackModifier(old_target_att);
            active_user->setSpecialAttackModifier(old_target_spatt);
            active_target->setAttackModifier(old_user_att);
            active_target->setSpecialAttackModifier(old_user_spatt);
            event_handler->displayMsg(user_mon_name+" switched Atk and Sp. Atk stat changes with "+opponent_mon_name+"!");
            break;
        }
        case 184:{
            //switch att spatt with opponent
            if(active_target->isFainted())
                return;
            int old_user_def = active_user->getDefenseModifier();
            int old_user_spdef = active_user->getSpecialDefenseModifier();
            int old_target_def = active_target->getDefenseModifier();
            int old_target_spdef = active_target->getSpecialDefenseModifier();
            active_user->setAttackModifier(old_target_def);
            active_user->setSpecialAttackModifier(old_target_spdef);
            active_target->setAttackModifier(old_user_def);
            active_target->setSpecialAttackModifier(old_user_spdef);
            event_handler->displayMsg(user_mon_name+" switched Def and Sp. def stat changes with "+opponent_mon_name+"!");
            break;
        }
        case 185:{
            // baton pass
            if(active_user->isFainted())
                return;
            //user switches out and entering monster keeps sta changes and volatiles
            if(!user_team->hasAliveBackup()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(active_user->hasAbility(REGENERATOR)){
                //regenerate 1/3 of user HP
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int heal_amount = max((maxHP+2) / 3,1);
                active_user->removeDamage(heal_amount);
            }
            if(active_user->hasAbility(NATURAL_CURE)){
                //remove status
                active_user->clearPermanentStatus();
            }
            if(actor==PLAYER){
                unsigned int new_active_index = event_handler->chooseSwitchForced(player_team);
                player_team->swapActiveMonster(new_active_index);
                player_active->setMonster(player_team->getActiveMonster(),player_team);
                active_target = player_active;
            }else{
                unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
                opponent_team->swapActiveMonster(new_active_index);
                opponent_active->setMonster(opponent_team->getActiveMonster(),opponent_team);
                active_target = opponent_active;
            }
            checkMonsterLeavingAbilities(actor);
            removeVolatilesFromOpponentOfMonsterLeavingField(actor);
            player_active->addSeenOpponent(opponent_active->getMonster());
            applySwitchInFormChange(actor);
            if(onTerrainChange(actor))
                return;
            if(applySwitchInAbilitiesEffects(active_target))
                return;
            if(applySwitchInItemsEffects(actor))
                return;
            if(performEntryHazardCheck(actor))
                return;
            checkUproars();
            break;
        }
        case 187:{
            //user needs to recharge next turn
            if(active_user->isFainted())
                return;
            active_user->addVolatileCondition(RECHARGING, -1);
            break;
        }
        case 188:{
            //set sun
            if(field->getWeather() == SUN || field->weatherCannotChange() || thereIsaCloudNine()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setWeather(SUN,active_user->hasHeldItem(HEAT_ROCK)?8:5);
            }
            break;
        }
        case 190:{
            //mist
            if(field->hasFieldEffect(MIST,actor)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setFieldEffect(MIST, 5, actor);
                event_handler->displayMsg(user_mon_name+" is shrouded in mist!");
            }
            break;
        }
        case 191:{
            //user change type to first attack type
            if(active_user->isFainted())
                return;
            auto attacks = active_user->getAttacks();
            if(attacks.size() == 0){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            auto attack_id = attacks[0];
            Attack* attack_pnt = Attack::getAttack(attack_id);
            if(attack_pnt == nullptr){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            Type attack_type = attack_pnt->getType();
            active_user->clearTypes();
            active_user->addType(attack_type);
            event_handler->displayMsg(user_mon_name+" changed type was changed to "+typeToString(attack_type)+" type!");
            break;
        }
        case 192:{
            //change type to resist opponent last move
            if(active_user->isFainted())
                return;
            if(opponent_active->getLastAttackUsed() == 0){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            Attack* opponent_last_attack = Attack::getAttack(opponent_active->getLastAttackUsed());
            if(opponent_last_attack == nullptr){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            // Type attack_type = opponent_last_attack->getType();
            if(attack_type==NO_TYPE){
                attack_type = NORMAL;
            }
            std::set<Type> all_types = getAllTypesSet();
            std::vector<Type> resist_types;
            for(auto type:all_types){
                if(getTypeEffectiveness(attack_type,type,true,false,false) <= 0.9){
                    resist_types.push_back(type);
                }
            }
            unsigned int choice = RNG::getRandomInteger(0,resist_types.size()-1);
            active_user->clearTypes();
            active_user->addType(resist_types[choice]);
            event_handler->displayMsg(user_mon_name+" changed type to resist "+opponent_mon_name+"'s last move!");
            break;
        }
        case 193:{
            //block
            if(active_target->isFainted())
                return;
            if(active_target->hasVolatileCondition(BLOCKED)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                active_target->addVolatileCondition(BLOCKED, 5);
            }
            break;
        }
        case 198:{
            //switch item with target
            if(active_target->isFainted())
                return;
            if(active_user->isFainted())
                return;
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            //nobody has items
            if((!active_target->hasHeldItem() && !active_user->hasHeldItem())){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            //some of the items involved cannot be passed around
            if(active_user->hasHeldItem() && !active_user->canStealItem()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(active_target->hasHeldItem() && !active_target->canStealItem()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            ItemType user_item = active_user->removeHeldItem();
            ItemType target_item = active_target->removeHeldItem();
            active_user->setHeldItem(target_item);
            active_target->setHeldItem(user_item);
            event_handler->displayMsg(user_mon_name+" switched items with "+opponent_mon_name+"!");
            break;
        }
        case 199:{
            //steal target item if it is a berry and use it
            if(hits_substitute)
                return;
            if(!active_user->hasHeldItem())
                return;
            //retrieve item directly from monster to avoid magic room item negation
            ItemType target_item = active_target->getMonster()->getHeldItem();
            ItemData * target_item_data = ItemData::getItemData(target_item);
            if(!active_target->hasHeldItem() || 
                target_item_data->getCategory()!=BERRY || 
                !active_target->canStealItem()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            active_target->removeHeldItem();
            active_user->consumeItem(target_item);
            event_handler->displayMsg(user_mon_name+" stole "+opponent_mon_name+"'s "+target_item_data->getName()+" and ate it!");
            break;
        }
        case 201:{
            //delete opponent item if it is a berry
            if(hits_substitute)
                return;
            ItemType target_item = active_target->getMonster()->getHeldItem();
            ItemData * target_item_data = ItemData::getItemData(target_item);
            if(!active_target->hasHeldItem() || 
                target_item_data->getCategory()!=BERRY){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            // ItemData * target_item_data = ItemData::getItemData(target_item);
            active_target->removeHeldItem();
            event_handler->displayMsg(opponent_mon_name+"'s "+target_item_data->getName()+" was destroyed!");
            break;
        }
        case 202:{
            // steal opponent item
            if(hits_substitute)
                return;
            ItemType target_item = active_target->getMonster()->getHeldItem();
            if(active_user->hasHeldItem() || 
                active_target->hasHeldItem() || 
                !active_target->canStealItem()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            ItemData * target_item_data = ItemData::getItemData(target_item);
            active_target->removeHeldItem();
            active_user->setHeldItem(target_item);
            event_handler->displayMsg(user_mon_name+" stole "+opponent_mon_name+"'s "+target_item_data->getName()+"!");
            break;
        }
        case 204:{
            //fling
            // check if user has held item that would have some effect (it would not have some effect under magic room)
            bool has_held_item_effect = (active_user->getHeldItem() != NO_ITEM_TYPE);
            ItemType user_item = active_user->removeHeldItem();
            if(actor==PLAYER)
                item_on_the_ground_player = user_item;
            else
                item_on_the_ground_opponent = user_item;
            if(active_target->hasSubstitute() && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(!has_held_item_effect){
                break;
            }
            switch(user_item){
                case KINGS_ROCK:
                case RAZOR_FANG:{
                    //flinch
                    active_target->addVolatileCondition(FLINCH, 1);
                    break;
                }
                case FLAME_ORB:{
                    if(active_target->canBeBurned())
                        active_target->setPermanentStatus(BURN);
                    break;
                }
                case LIGHT_BALL:{
                    if(active_target->canBeParalyzed())
                        active_target->setPermanentStatus(PARALYSIS);
                    break;
                }
                case TOXIC_ORB:{
                    if(active_target->canBeBadlyPoisoned())
                        active_target->setPermanentStatus(BAD_POISON);
                    break;
                }
                case POISON_BARB:{
                    if(active_target->canBePoisoned())
                        active_target->setPermanentStatus(POISONED);
                    break;
                }
                case WHITE_HERB:{
                    //reset decreased stat changes of target
                    if(active_target->getAttackModifier()<0)
                        active_target->setAttackModifier(0);
                    if(active_target->getSpecialAttackModifier()<0)
                        active_target->setSpecialAttackModifier(0);
                    if(active_target->getDefenseModifier()<0)
                        active_target->setDefenseModifier(0);
                    if(active_target->getSpecialDefenseModifier()<0)
                        active_target->setSpecialDefenseModifier(0);
                    if(active_target->getSpeedModifier()<0)
                        active_target->setSpeedModifier(0);
                    if(active_target->getAccuracyModifier()<0)
                        active_target->setAccuracyModifier(0);
                    if(active_target->getEvasionModifier()<0)
                        active_target->setEvasionModifier(0);
                    break;
                }
                case MENTAL_HERB:{
                    //reset target volatiles
                    active_target->removeVolatileCondition(INFATUATION);
                    active_target->removeVolatileCondition(ENCORE);
                    active_target->removeVolatileCondition(TAUNTED);
                    active_target->removeVolatileCondition(TORMENTED);
                    active_target->removeDisable();
                    break;
                }
                default:break;
            }
            break;
        }
        case 207:{
            //+1 att acc user
            // bool res = active_user->changeAttackModifier(+1);
            // res = res || active_user->changeAccuracyModifier(+1);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{6,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 211:{
            //defog
            // bool res = active_target->changeEvasionModifier(-1);
            field->clearFieldSide(other_actor);
            field->clearTerrain();
            event_handler->displayMsg(user_mon_name+" cleared "+opponent_mon_name+"'s side of the field!");
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{7,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 212:{
            //-1 speed opponent
            // bool res = active_target->changeSpeedModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{5,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 213:{
            //set up misty terrain
            if(field->getTerrain() == MISTY_FIELD){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setTerrain(MISTY_FIELD,active_user->hasHeldItem(TERRAIN_EXTENDER)?8:5);
                // consumeSeeds();
            }
            break;
        }
        case 214:{
            //restore consumed item
            if(active_user->isFainted())
                return;
            if(active_user->restoreItem()){
                event_handler->displayMsg(user_mon_name+" restored its item!");
            }else{
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 216:{
            //trick room
            if(field->hasFullFieldEffect(TRICK_ROOM)){
                field->clearFullFieldEffect(TRICK_ROOM);
                event_handler->displayMsg("The trick room wore off!");
            }else{
                field->setFullFieldEffect(TRICK_ROOM, 5);
                event_handler->displayMsg(user_mon_name+" set up trick room!");
                checkRoomService();
            }
            break;
        }
        case 217:{
            //drop money 5 times user level
            unsigned int money_dropped = active_user->getLevel() * 5;
            addMoney(money_dropped);
            event_handler->displayMsg(user_mon_name+" dropped "+std::to_string(money_dropped)+"$!");
            break;
        }
        case 223:{
            //wish
            event_handler->displayMsg(user_mon_name+" made a wish!");
            field->setFieldEffect(WISH,2,actor);
            break;
        }
        case 224:{
            //swap status with opponent
            if(active_user->isFainted() || active_target->isFainted())
                return;
            if(!active_user->hasPermanentStatus()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(hits_substitute && !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            //permanent status
            PermanentStatusCondition old_status = active_user->getPermanentStatus();
            active_user->clearPermanentStatus();
            if(!field->hasFieldEffect(SAFEGUARD,other_actor) ||
                active_user->hasAbility(INFILTRATOR))
                active_target->setPermanentStatus(old_status);
            event_handler->displayMsg(user_mon_name+" tried to transfer its status to "+opponent_mon_name+"!");
            break;
        }
        case 225:{
            //+3 def user
            // bool res = active_user->changeDefenseModifier(+3);
            // if(res && active_user->hasAbility(CONTRARY))
            //     tryEjectPack(actor);
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,3}};
            changeStats(actor,changes,false);
            break;
        }
        case 228:{
            //-1 att spatt opponent
            // bool res = active_target->changeAttackModifier(-1);
            // res = res || active_target->changeSpecialAttackModifier(-1);
            // if(res && !active_target->hasAbility(CONTRARY))
            //     tryEjectPack(other_actor);
            if(active_target->isFainted())
                return;
            if(hits_substitute && 
                !active_user->hasAbility(INFILTRATOR)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("The substitute protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-1},{3,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 237:{
            //pain split
            if(active_user->isFainted() || active_target->isFainted())
                return;
            if(active_target->hasSubstitute() && !active_user->hasAbility(INFILTRATOR)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            unsigned int old_hp_user = active_user->getCurrentHP();
            unsigned int old_hp_target = active_target->getCurrentHP();
            unsigned int new_hp_user = (old_hp_user + old_hp_target) / 2;
            unsigned int new_hp_target = (old_hp_user + old_hp_target) - new_hp_user;
            unsigned int user_amount = 0;
            unsigned int target_amount = 0;
            event_handler->displayMsg(user_mon_name+" and "+opponent_mon_name+" share their HP!");
            if(old_hp_user > new_hp_user){
                user_amount = active_user->addDirectDamage(old_hp_user - new_hp_user);
                // event_handler->displayMsg(user_mon_name+" took "+std::to_string(user_amount)+" damage!");
                event_handler->displayDmgDealt(user_amount,active_user);
            }else if(old_hp_user < new_hp_user){
                user_amount = active_user->removeDamage(new_hp_user - old_hp_user);
                if(user_amount > 0)
                    event_handler->displayMsg(user_mon_name+" was healed by "+std::to_string(user_amount)+" HP!");
            }
            if(old_hp_target > new_hp_target){
                target_amount = active_target->addDirectDamage(old_hp_target - new_hp_target);
                // event_handler->displayMsg(opponent_mon_name+" took "+std::to_string(target_amount)+" damage!");
                event_handler->displayDmgDealt(target_amount,active_target);
            }else if(old_hp_target < new_hp_target){
                target_amount = active_target->removeDamage(new_hp_target - old_hp_target);
                if(target_amount > 0)
                    event_handler->displayMsg(opponent_mon_name+" was healed by "+std::to_string(target_amount)+" HP!");
            }
            break;
        }
        case 242:{
            //power split
            Stats user_battle_stats = active_user->getBattleStats();
            Stats target_battle_stats = active_target->getBattleStats();
            unsigned int user_atk = user_battle_stats.getAtk();
            unsigned int user_spatk = user_battle_stats.getSpatk();
            unsigned int target_atk = target_battle_stats.getAtk();
            unsigned int target_spatk = target_battle_stats.getSpatk();
            unsigned int new_user_atk = (user_atk + target_atk) / 2;
            unsigned int new_user_spatk = (user_spatk + target_spatk) / 2;
            unsigned int new_target_atk = (user_atk + target_atk) - new_user_atk;
            unsigned int new_target_spatk = (user_spatk + target_spatk) - new_user_spatk;
            event_handler->displayMsg(user_mon_name+" and "+opponent_mon_name+" split their attacking stats!");
            active_user->setBattleAttack(new_user_atk);
            active_user->setBattleSpecialAttack(new_user_spatk);
            active_target->setBattleAttack(new_target_atk);
            active_target->setBattleSpecialAttack(new_target_spatk);
            break;
        }
        case 243:{
            //guard split
            Stats user_battle_stats = active_user->getBattleStats();
            Stats target_battle_stats = active_target->getBattleStats();
            unsigned int user_def = user_battle_stats.getDef();
            unsigned int user_spdef = user_battle_stats.getSpdef();
            unsigned int target_def = target_battle_stats.getDef();
            unsigned int target_spdef = target_battle_stats.getSpdef();
            unsigned int new_user_def = (user_def + target_def) / 2;
            unsigned int new_user_spdef = (user_spdef + target_spdef) / 2;
            unsigned int new_target_def = (user_def + target_def) - new_user_def;
            unsigned int new_target_spdef = (user_spdef + target_spdef) - new_user_spdef;
            event_handler->displayMsg(user_mon_name+" and "+opponent_mon_name+" split their defending stats!");
            active_user->setBattleAttack(new_user_def);
            active_user->setBattleSpecialAttack(new_user_spdef);
            active_target->setBattleAttack(new_target_def);
            active_target->setBattleSpecialAttack(new_target_spdef);
            break;
        }
        case 244:{
            //power trick
            if(active_user->isFainted())
                return;
            Stats user_battle_stats = active_user->getBattleStats();
            unsigned int user_atk = user_battle_stats.getAtk();
            unsigned int user_def = user_battle_stats.getDef();
            active_user->setBattleAttack(user_def);
            active_user->setBattleDefense(user_atk);
            break;
        }
        case 245:{
            //target cannot use sound based attacks for 2 turns
            if(active_target->isFainted())
                return;
            active_target->addVolatileCondition(THROAT_CHOPPED, 2);
            break;
        }
        case 248:{
            //grudge
            if(active_user->isFainted())
                return;
            if(active_user->hasVolatileCondition(GRUDGED)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break; 
            }
            active_user->addVolatileCondition(GRUDGED, -1);
            break;
        }
        case 250:{
            //sketch
            unsigned int last_attack_target_id = active_target->getLastAttackUsed();
            Attack* last_attack_target = Attack::getAttack(last_attack_target_id);
            if(last_attack_target == nullptr){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(!last_attack_target->canBeSketched()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(!active_user->getMonster()->replaceAttack(SKETCH_ID,last_attack_target_id)){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            break;
        }
        case 252:{
            //heal all team of status conditions
            bool has_effect=false;
            for(unsigned int i=0;i<user_team->getSize();i++){
                Monster* user_mon = user_team->getMonster(i);
                if(user_mon->isFainted())
                    continue;
                if(attack->isSoundBased() && user_mon->getAbility() == SOUNDPROOF)//heal bell does not affect soundproof holders
                    continue;
                if(user_mon->getPermanentStatus()==NO_PERMANENT_CONDITION)
                    continue;
                user_mon->setPermanentStatus(NO_PERMANENT_CONDITION);
                has_effect=true;
            }
            if(!has_effect){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }
            break;
        }
        case 255:{
            //create substitute and switch out
            if(active_user->isFainted())
                return;
            unsigned int substitute_hp = max(1,active_user->getMaxHP() / 4);
            unsigned int substitute_cost = max(1,active_user->getMaxHP() / 2);
            if(substitute_cost >= active_user->getCurrentHP()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            if(!user_team->hasAliveBackup()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            if(!active_user->canSwitchOut(active_target)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            unsigned int actual_cost_dmg = active_user->addDirectDamage(substitute_cost);
            event_handler->displayMsg(user_mon_name+" lost "+std::to_string(actual_cost_dmg)+" HP in order to create a substitute!");
            //force user to switch
            forceSwitch(actor);
            active_user = getActorBattler(actor);
            checkUproars();
            if(active_user->isFainted())
                return;
            if(!active_user->hasSubstitute())
                active_user->setSubstituteHP(substitute_hp);
            break;
        }
        case 256:{
            if(active_user->isFainted())
                return;
            unsigned int substitute_hp = max(1,active_user->getMaxHP() / 4);
            unsigned int substitute_cost = max(1,active_user->getMaxHP() / 4);
            if(substitute_cost >= active_user->getCurrentHP() || active_user->hasSubstitute()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;
            }
            active_user->setSubstituteHP(substitute_hp);
            break;
        }
        case 260:{
            //clear terrain
            if(field->getTerrain() == NO_TERRAIN){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            field->clearTerrain();
            break;
        }
        case 262:{
            //change weather to hail
            if(field->getWeather() == HAIL || thereIsaCloudNine() || field->weatherCannotChange()){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
            }else{
                field->setWeather(HAIL,active_user->hasHeldItem(HEAT_ROCK)?8:5);
            }
            break;
        }
        case 265:{
            //user ability is copied to target
            if(active_target->isFainted())
                return;
            if(active_user->hasAbility(NO_ABILITY) ||
                abilityCannotBeCopied(active_user->getAbility()) ||
                abilityCannotBeChanged(active_target->getAbility())){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;   
            }
            active_target->setAbility(active_user->getAbility());
            event_handler->displayMsg(opponent_mon_name+"gained "+user_mon_name+"'s ability!");
            break;
        }
        case 267:{
            //target gets 2 turns of heal block
            if(active_target->isFainted())
                return;
            if(active_target->hasVolatileCondition(HEAL_BLOCKED)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            active_target->addVolatileCondition(HEAL_BLOCKED, 2);
            event_handler->displayMsg(opponent_mon_name+" will not be able to heal for 2 turns!");
            break;
        }
        case 271:{
            //set up magic room
            if(field->hasFullFieldEffect(MAGIC_ROOM)){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }else{
                field->setFullFieldEffect(MAGIC_ROOM, 5);
                event_handler->displayMsg(user_mon_name+" set up a dimension where Items don't have any effects!");
            }
            break;
        }
        case 272:{
            // switch ability with target
            if(active_target->isFainted())
                return;
            if(active_user->hasAbility(NO_ABILITY) ||
                active_target->hasAbility(NO_ABILITY) ||
                abilityCannotBeChanged(active_user->getAbility()) ||
                abilityCannotBeCopied(active_target->getAbility())){
                event_handler->displayMsg("But it failed!");
                active_user->setLastAttackFailed();
                break;   
            }
            Ability old_user_ability = active_user->getAbility();
            Ability old_target_ability = active_target->getAbility();
            active_user->setAbility(old_target_ability);
            active_target->setAbility(old_user_ability);
            event_handler->displayMsg(user_mon_name+" switched abilities with "+opponent_mon_name+"!");
            // apply new ability effects of user
            applySwitchInFormChange(actor);
            bool res1 = applySwitchInAbilitiesEffects(active_user);
            // apply new ability effects of target
            applySwitchInFormChange(other_actor);
            bool res2 = applySwitchInAbilitiesEffects(active_target);
            if(res1 || res2){
                return;
            }
            break;
        }
        case 282:{
            //boost defence of all GRASS types on the field
            StatCV changes = {{2,1}};
            if(!active_user->isFainted() && active_user->hasType(GRASS)){
                changeStats(actor,changes,false);
            }
            if(!active_target->isFainted() && active_target->hasType(GRASS)){
                changeStats(other_actor,changes,false);
            }
            break;
        }
        case 283:{
            //magic caot
            if(active_user->isFainted())
                return;
            active_user->addVolatileCondition(MAGIC_COATED, 1);
            event_handler->displayMsg(user_mon_name+" is ready to reflect some status moves back to the user!");
            break;
        }
        case 285:{
            //-1 att spatt spd target only if it is poisoned
            if(active_target->isFainted())
                return;
            if(!active_target->isPoisoned()){
                if(attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            StatCV changes = {{1,-1},{3,-1},{5,-1}};
            changeStats(other_actor,changes,false);
            break;
        }
        case 292:{
            //boost spatt spdef and heal status of user
            if(active_user->isFainted())
                return;
            StatCV changes = {{3,1},{4,1}};
            changeStats(actor,changes,false);
            active_user->clearPermanentStatus();
            break;
        }
        case 293:{
            //switch stat changes with target
            if(active_user->isFainted() || active_target->isFainted())
                return;
            int attack_mod_user = active_user->getAttackModifier();
            int special_attack_mod_user = active_user->getSpecialAttackModifier();
            int defense_mod_user = active_user->getDefenseModifier();
            int special_defense_mod_user = active_user->getSpecialDefenseModifier();
            int speed_mod_user = active_user->getSpeedModifier();
            int accuracy_mod_user = active_user->getAccuracyModifier();
            int evasion_mod_user = active_user->getEvasionModifier();
            int attack_mod_target = active_target->getAttackModifier();
            int special_attack_mod_target = active_target->getSpecialAttackModifier();
            int defense_mod_target = active_target->getDefenseModifier();
            int special_defense_mod_target = active_target->getSpecialDefenseModifier();
            int speed_mod_target = active_target->getSpeedModifier();
            int accuracy_mod_target = active_target->getAccuracyModifier();
            int evasion_mod_target = active_target->getEvasionModifier();
            active_user->setAttackModifier(attack_mod_target);
            active_user->setSpecialAttackModifier(special_attack_mod_target);
            active_user->setDefenseModifier(defense_mod_target);
            active_user->setSpecialDefenseModifier(special_defense_mod_target);
            active_user->setSpeedModifier(speed_mod_target);
            active_user->setAccuracyModifier(accuracy_mod_target);
            active_user->setEvasionModifier(evasion_mod_target);
            active_target->setAttackModifier(attack_mod_user);
            active_target->setSpecialAttackModifier(special_attack_mod_user);
            active_target->setDefenseModifier(defense_mod_user);
            active_target->setSpecialDefenseModifier(special_defense_mod_user);
            active_target->setSpeedModifier(speed_mod_user);
            active_target->setAccuracyModifier(accuracy_mod_user);
            active_target->setEvasionModifier(evasion_mod_user);
            event_handler->displayMsg(user_mon_name+" switched all changes in their stats with "+opponent_mon_name+"!");
            break;
        }
        case 295:{
            //-1 def spdef speed user
            if(active_user->isFainted())
                return;
            StatCV changes = {{2,-1},{4,-1},{5,-1}};
            changeStats(actor,changes,false);
            break;
        }
        case 297:{
            //+2 speed + 1 att user
            if(active_user->isFainted())
                return;
            StatCV changes = {{5,2},{1,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 301:{
            //+1 att def speed user
            if(active_user->isFainted())
                return;
            StatCV changes = {{1,1},{2,1},{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case 302:{
            //crafty shield
            if(active_user->isFainted())
                return;
            active_user->addVolatileCondition(CRAFTY_SHIELD, 1);
            break;
        }
        default:break;
    }
}


void Battle::decrementVolatiles(Battler* active_user){
    if(active_user->hasVolatileCondition(PETAL_DANCING)){
        active_user->decrementVolatileCondition(PETAL_DANCING);
        if(!active_user->hasVolatileCondition(PETAL_DANCING) && 
            !active_user->hasVolatileCondition(CONFUSION) &&
            field->getTerrain() != MISTY_FIELD){
            active_user->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
        }
    }
    if(active_user->hasVolatileCondition(RAGING_FURING)){
        active_user->decrementVolatileCondition(RAGING_FURING);
        if(!active_user->hasVolatileCondition(RAGING_FURING) && 
            !active_user->hasVolatileCondition(CONFUSION) &&
            field->getTerrain() != MISTY_FIELD){
            active_user->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
        }
    }
    if(active_user->hasVolatileCondition(THRASHING)){
        active_user->decrementVolatileCondition(THRASHING);
        if(!active_user->hasVolatileCondition(THRASHING) && 
            !active_user->hasVolatileCondition(CONFUSION) &&
            field->getTerrain() != MISTY_FIELD){
            active_user->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
        }
    }
    if(active_user->hasVolatileCondition(OUTRAGING)){
        active_user->decrementVolatileCondition(OUTRAGING);
        if(!active_user->hasVolatileCondition(OUTRAGING) && 
            !active_user->hasVolatileCondition(CONFUSION)&&
            field->getTerrain() != MISTY_FIELD){
            active_user->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
        }
    }
    if(active_user->hasVolatileCondition(UPROARING)){
        active_user->decrementVolatileCondition(UPROARING);
        if(!active_user->hasVolatileCondition(UPROARING)){
            uproaring_monsters.erase(active_user->getMonster());
            std::string user_mon_name = getActorBattlerName(active_user->getActor());
            event_handler->displayMsg(user_mon_name+" stopped making an uproar!");
        }
    }
    if(active_user->hasVolatileCondition(ROLLINGOUT)){
        active_user->decrementVolatileCondition(ROLLINGOUT);
    }
    if(active_user->hasVolatileCondition(TAUNTED)){
        active_user->decrementVolatileCondition(TAUNTED);
    }
    if(active_user->hasVolatileCondition(TORMENTED)){
        active_user->decrementVolatileCondition(TORMENTED);
    }
    active_user->removeVolatileCondition(FLINCH);
    active_user->decrementVolatileCondition(THROAT_CHOPPED);
}

void Battle::forgetMoveVolatiles(Battler* active_user){
    active_user->removeVolatileCondition(PETAL_DANCING);
    active_user->removeVolatileCondition(RAGING_FURING);
    active_user->removeVolatileCondition(UPROARING);
    uproaring_monsters.erase(active_user->getMonster());
    active_user->removeVolatileCondition(THRASHING);
    active_user->removeVolatileCondition(OUTRAGING);
    active_user->removeVolatileCondition(CHARGING_SOLARBEAM);
    active_user->removeVolatileCondition(CHARGING_SOLARBLADE);
    active_user->removeVolatileCondition(CHARGING_SKYATTACK);
    active_user->removeVolatileCondition(CHARGING_ICEBURN);
    active_user->removeVolatileCondition(CHARGING_FREEZESHOCK);
    // METEOR BEAM is still charged until it completes
    // active_user->removeVolatileCondition(CHARGING_METEORBEAM);
    active_user->removeVolatileCondition(VANISHED);
    active_user->removeVolatileCondition(VANISHED_2);
    active_user->removeVolatileCondition(FLYING_HIGH);
    active_user->removeVolatileCondition(BOUNCING);
    active_user->removeVolatileCondition(UNDERGROUND);
    active_user->removeVolatileCondition(ROLLINGOUT);
    active_user->removeVolatileCondition(FLINCH);
    active_user->removeVolatileCondition(LASER_FOCUS);
}

void Battle::performSwitch(BattleAction action){
    std::string old_active_monster_name;
    std::string new_active_monster_name;
    Battler* new_active_monster;
    if(action.getActor() == PLAYER){
        if(player_active->isFainted())
            return;
        // std::cout<<"Player switched to monster " << action.getSwitchId() << std::endl;
        field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,PLAYER);
        old_active_monster_name = player_active->getNickname();
        if(player_active->hasAbility(REGENERATOR)){
            //regenerate 173 of user HP
            unsigned int maxHP = player_active->getMaxHP();
            unsigned int heal_amount = max((maxHP+2) / 3,1);
            player_active->removeDamage(heal_amount);
        }
        if(player_active->hasAbility(NATURAL_CURE)){
            //remove permanent status
            player_active->clearPermanentStatus();
        }
        // event_handler->displayMsg("Player switched to monster " + std::to_string(action.getSwitchId()));
        delete player_active;
        player_team->swapActiveMonster(action.getSwitchId());
        player_active = new Battler(player_team->getActiveMonster(),player_team,field,PLAYER,event_handler);
        new_active_monster_name = player_active->getNickname();
        new_active_monster = player_active;
        event_handler->displayMsg("Player switched out " + old_active_monster_name + " and sent " + new_active_monster_name);
    }else{
        if(opponent_active->isFainted())
            return;
        // std::cout<<"Opponent switched to monster " << action.getSwitchId() << std::endl;
        field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,OPPONENT);
        if(opponent_active->hasAbility(REGENERATOR)){
            //regenerate 1/3 of user HP
            unsigned int maxHP = opponent_active->getMaxHP();
            unsigned int heal_amount = max((maxHP+2) / 3,1);
            opponent_active->removeDamage(heal_amount);
        }
        if(opponent_active->hasAbility(NATURAL_CURE))
            opponent_active->clearPermanentStatus();
        old_active_monster_name = opponent_active->getNickname();
        delete opponent_active;
        opponent_team->swapActiveMonster(action.getSwitchId());
        opponent_active = new Battler(opponent_team->getActiveMonster(),opponent_team,field,OPPONENT,event_handler);
        new_active_monster_name = opponent_active->getNickname();
        new_active_monster = opponent_active;
        event_handler->displayMsg("Opponent switched out " + old_active_monster_name + " and sent " + new_active_monster_name);
    }
    checkMonsterLeavingAbilities(action.getActor());
    player_active->addSeenOpponent(opponent_active->getMonster());
    resetOpponents();
    removeVolatilesFromOpponentOfMonsterLeavingField(action.getActor());
    applySwitchInFormChange(action.getActor());
    if(applySwitchInAbilitiesEffects(new_active_monster))
        return;
    if(applySwitchInItemsEffects(action.getActor()))
        return;
    if(onTerrainChange(action.getActor()))
        return;
    if(performEntryHazardCheck(action.getActor()))
        return;
    checkUproars();
    
}

unsigned int Battle::computeDamage(unsigned int attack_id, BattleActionActor user, bool critical_hit, 
    bool attack_after_target, float effectiveness, AttackType category,unsigned int beat_up_index){
    Attack* attack = Attack::getAttack(attack_id);
    if(category == STATUS){
        return 0;
    }
    unsigned int effect = attack->getEffectId();
    Battler* enemy_monster;
    Battler* user_monster;
    if(user == PLAYER){
        enemy_monster = opponent_active;
        user_monster = player_active;
    }else{
        enemy_monster = player_active;
        user_monster = opponent_active;
    }
    double attack_stat;
    double defense_stat;
    Type attack_type = attack->getType(user_monster,field);
    
    //check immunity
    if(effectiveness == 0)
        return 0;//immunities
    // check for sheer cold immunity
    if(effect == 156 && enemy_monster->hasType(ICE)){
        return 0;
    }
    // check for soundproof immunity
    if(attack->isSoundBased() && 
        category != STATUS &&
        enemy_monster->hasAbility(SOUNDPROOF)){
        return 0;
    }
    // check for overcoat immunity
    if(attack->isPowder() && 
        category != STATUS &&
        (enemy_monster->hasAbility(OVERCOAT) ||
        enemy_monster->hasHeldItem(SAFETY_GOGGLES))){
        return 0;
    }
    // check for endeavor
    if(effect == 43){
        int current_hp_user = user_monster->getCurrentHP();
        int current_hp_target = enemy_monster->getCurrentHP();
        return max(0,current_hp_target - current_hp_user);
    }
    // check for moves dealing 50% of opponent health
    if(effect == 50){
        return max(1,enemy_monster->getCurrentHP()/2);
    }
    // check for moves reflecting damage
    if(effect == 168){
        // reflect double special damage taken
        unsigned int dmg_taken = user_monster->getSpecialDamageTakenThisTurn();
        return max(1,dmg_taken*2);
    }
    if(effect == 175){
        // reflect double physical damage taken
        unsigned int dmg_taken = user_monster->getPhysicalDamageTakenThisTurn();
        return max(1,dmg_taken*2);
    }
    if(effect==206){
        //reflect 1.5 times damage taken
        unsigned int dmg_taken = user_monster->getPhysicalDamageTakenThisTurn() + 
            user_monster->getSpecialDamageTakenThisTurn();
        return max(1,dmg_taken*1.5);
    }

    float burn_multiplier = 1;
    unsigned int physical_attack_stat;
    unsigned int physical_defense_stat;
    unsigned int special_attack_stat;
    unsigned int special_defense_stat;
    // get stats for calculation
    // attack stats
    if(critical_hit || enemy_monster->hasAbility(UNAWARE)){
        physical_attack_stat = user_monster->getStats().getAtk();
        special_attack_stat = user_monster->getStats().getSpatk();
    }else{
        physical_attack_stat = user_monster->getModifiedAttack();
        special_attack_stat = user_monster->getModifiedSpecialAttack();
    }
    //defense stats
    if(user_monster->hasAbility(UNAWARE) || effect==286){
        //UNAWARE uses base stats instead of modified
        //sacred sword ignores defensive modifiers
        physical_defense_stat = enemy_monster->getStats().getDef();
        special_defense_stat = enemy_monster->getStats().getSpdef();
    }else{
        physical_defense_stat = enemy_monster->getModifiedDefense();
        special_defense_stat = enemy_monster->getModifiedSpecialDefense();
    }

    // apply foul play effect
    if(effect==233){
        // foul play uses the target's attack stat
        physical_attack_stat = enemy_monster->getModifiedAttack();
    }

    if((field->getWeather() == SNOWSTORM || field->getWeather()==HAIL) && 
        !thereIsaCloudNine() && enemy_monster->hasType(ICE)){
        //ice types increase their defense stat under snowstorm
        physical_defense_stat *= 1.5;
    }
        
    if(category == PHYSICAL){
        attack_stat = physical_attack_stat;
        if(effect==257)//body press uses defense instead of attack of user
            attack_stat = user_monster->getModifiedDefense();
        if(user_monster->hasHeldItem(CHOICE_BAND) || user_monster->hasAbility(GORILLA_TACTICS))
            attack_stat*=1.5;
        if(user_monster->hasAbility(HUGE_POWER) ||
            user_monster->hasAbility(PURE_POWER)){
            //HUGE POWER doubles the attack stat (PURE POWER has the same effect)
            attack_stat *= 2;
        }
        defense_stat = physical_defense_stat;   
        if(user_monster->isBurned() && !user_monster->hasAbility(GUTS)){
            burn_multiplier *= 0.5;
        }
    }else{
        attack_stat = special_attack_stat;
        if(user_monster->hasHeldItem(CHOICE_SPECS))
            attack_stat*=1.5;
        if(user_monster->hasAbility(SOLAR_POWER) && !thereIsaCloudNine() && 
            !user_monster->hasHeldItem(UTILITY_UMBRELLA) &&
            (field->getWeather() == SUN || field->getWeather()==EXTREME_SUN)){// SOLAR POWER
            attack_stat*=1.5;
        }
        defense_stat = special_defense_stat; 
        // if(user_monster->isBurned() && !user_monster->hasAbility(GUTS)){
        //     burn_multiplier *= 0.5;
        // }
        if(effect==128)//psystrike hits special move on physical defense
            defense_stat = physical_defense_stat;
    }
    if(effect==100){
        //self destructing moves halve enemy defense stat
        defense_stat *= 0.5;
    }
    if((attack_type==FIRE || attack_type==ICE) &&
        enemy_monster->hasAbility(THICK_FAT)){
        //THICK FAT reduces damage from fire and ice moves by 50% by halving the attack stat
        attack_stat *= 0.5;
    }
    
    double base_power = computePower(attack,user, attack_after_target,beat_up_index);
    // compute modifiers
    unsigned int level = user_monster->getLevel();
    float stab_multiplier = 1;
    if(user_monster->hasType(attack_type)){
        if(user_monster->hasAbility(ADAPTABILITY))//adaptability boosts STAB
            stab_multiplier = 2;
        else
            stab_multiplier = 1.5;
    }
    float weather_multiplier = 1;
    if((field->getWeather() == RAIN || field->getWeather()==HEAVY_RAIN) && 
        !thereIsaCloudNine() && !enemy_monster->hasHeldItem(UTILITY_UMBRELLA)){
        if(attack_type == WATER)
            weather_multiplier = 1.5;
        else if(attack_type == FIRE)
            weather_multiplier = 0.5;
    }else if((field->getWeather() == SUN || field->getWeather()==EXTREME_SUN) && 
        !thereIsaCloudNine() && !enemy_monster->hasHeldItem(UTILITY_UMBRELLA)){
        if(attack_type == FIRE)
            weather_multiplier = 1.5;
        else if(attack_type == WATER)
            weather_multiplier = 0.5;
    }
    float crit_multiplier = 1;
    if(critical_hit){
        crit_multiplier = 1.5;
        if(user_monster->hasAbility(SNIPER)){ // SNIPER boosts critical hit damage
            crit_multiplier *= 1.5;
        }
    }
    
    // compute actual damage
    float random_factor = RNG::getRandomInteger(85,100)/100.0;
    double base_damage = baseDamage(level, base_power, attack_stat, defense_stat);
    double damage = base_damage * stab_multiplier * effectiveness * weather_multiplier * crit_multiplier * random_factor * burn_multiplier;
    
    // remove barriers 
    if(effect==172 || effect==215){
        field->clearFieldEffect(REFLECT,otherBattleActionActor(user));
        field->clearFieldEffect(LIGHT_SCREEN,otherBattleActionActor(user));
        field->clearFieldEffect(AURORA_VEIL,otherBattleActionActor(user));
    }
    //apply field effects
    if(field->hasFieldEffect(LIGHT_SCREEN, otherBattleActionActor(user)) && 
        category==SPECIAL &&
        !user_monster->hasAbility(INFILTRATOR)){
        damage *= 2732.0 / 4096;
    }
    if(field->hasFieldEffect(REFLECT, otherBattleActionActor(user)) && 
        category==PHYSICAL  &&
        !user_monster->hasAbility(INFILTRATOR)){
        damage *= 2732.0 / 4096;
    }
    if(field->hasFieldEffect(AURORA_VEIL, otherBattleActionActor(user)) && 
        (category!=STATUS) &&
        !user_monster->hasAbility(INFILTRATOR)){
        damage *= 2732.0 / 4096;
    }
    // apply solar beam and solar blade modifiers
    if((effect == 9 || effect==263)&&
        !thereIsaCloudNine() &&
        (field->getWeather() == HAIL || 
        field->getWeather() == SNOWSTORM || 
        field->getWeather() == SANDSTORM || 
        field->getWeather()==RAIN  || 
        field->getWeather()==HEAVY_RAIN)
        && !user_monster->hasHeldItem(UTILITY_UMBRELLA)){
            damage *= 0.5;
    }
    // some moves have damage halved under Grassy Terrain
    if(field->getTerrain() == GRASSY_FIELD && (effect == 69 || effect == 73)){
        damage *= 0.5;
    }
    // dry skin opponents take 25% more damage from fire attacks
    if(enemy_monster->hasAbility(DRY_SKIN) && attack_type == FIRE){
        damage *= 1.25;
    }
    // filter (and solid rock) reduces damage from supereffective attacks
    if(effectiveness>1.1 && (enemy_monster->hasAbility(FILTER) || enemy_monster->hasAbility(SOLID_ROCK))){
        damage *= 3.0 / 4.0;
    }
    // fur coat halves physical damage
    if(category == PHYSICAL && 
        enemy_monster->hasAbility(FUR_COAT)){
        damage *= 0.5;
    }
    // heatproof halves damage from fire type moves
    if(attack_type == FIRE && 
        enemy_monster->hasAbility(HEATPROOF)){
        damage *= 0.5;
    }

    // target eats berries to reduce incoming damage
    if(effect!=112 && effect!=117 && // do not eat berries for fixed dmg attacks
        !enemy_monster->hasSubstitute() && 
        enemy_monster->tryEatSuperEffectiveBerry(attack_type,effectiveness>1.1)){//lazy eval will prevent this call if attack is fixed dmg
        damage /= 2;
    }

    if(effect == 112){
        // damage equal to level
        unsigned int level = user_monster->getLevel();
        damage = level;
    }
    if(effect == 117){
        // damage equal to currentHP user
        unsigned int current_hp = user_monster->getCurrentHP();
        damage = current_hp;
    }

    int integer_damage = (int)damage;
    if(integer_damage < 1)
        integer_damage = 1;
    return integer_damage;
}

double Battle::computePower(Attack*attack,BattleActionActor actor,bool attack_after_target,unsigned int beat_up_index){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    MonsterTeam* active_team = getActorTeam(actor);
    unsigned int effect = attack->getEffectId();
    unsigned int attack_id = attack->getId();
    // get base power
    double base_power = attack->getPower();
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));

    Type attack_type = attack->getType(active_user,field);

    //effect modidifiers
    switch(effect){
        case 37:case 68:{
            double original_bp = base_power;
            if(active_user->getLastAttackUsed() == attack_id){//fury cutter
                for(unsigned int i=1; i<active_user->getSameAttackCounter(); i++){
                    base_power *= 2;
                    if(base_power>=4*original_bp)
                        break;
                }
            }
            // rollout doubles power if defense curl was used prior to it
            if(effect==68 && active_user->hasUsedAttack(DEFENSE_CURL_ID))
                base_power *= 2;
            break;
        }
        // compute power for electro ball
        case 64:{
            unsigned int user_speed = active_user->getModifiedSpeed();
            unsigned int enemy_speed = active_target->getModifiedSpeed();
            double speed_comparison = 100.0 *  enemy_speed / user_speed;
            if(speed_comparison>=100.0){
                base_power = 40;
            }else if(speed_comparison > 50.0){
                base_power = 60;
            }else if(speed_comparison > 33.34){
                base_power = 80;
            }else if(speed_comparison > 25.0){
                base_power = 120;
            }else{
                base_power = 160;
            }
            break;
        }
        //base power for gyro ball
        case 71:{
            unsigned int user_speed = active_user->getModifiedSpeed();
            unsigned int enemy_speed = active_target->getModifiedSpeed();
            if(user_speed < 1)
                base_power = 1;
            else
                base_power = min(150,(25*enemy_speed/user_speed)+1);
            break;
        }
        //check venoshock
        case 40:{
            if(active_target->isPoisoned()){ 
                base_power *= 2.0;
            }
            break;
        }
        //check for twister -> doubles power when opponent is flying high
        case 45:case 227:{
            if(active_target->hasVolatileCondition(FLYING_HIGH) ||
                active_target->hasVolatileCondition(BOUNCING)){
                base_power *= 2.0;
            }   
            break;
        }
        //check for earthquake -> doubles power when opponent is underground
        case 73:{
            if(active_target->hasVolatileCondition(UNDERGROUND)){
                base_power *= 2.0;
            }  
            break;  
        }
        //power depends on total ampunt of positive stat changes
        case 81:{
            unsigned int total_stat_changes = 1;//for base power
            if(active_user->getAttackModifier() > 0)
                total_stat_changes += active_user->getAttackModifier();
            if(active_user->getSpecialAttackModifier() > 0)
                total_stat_changes += active_user->getSpecialAttackModifier();
            if(active_user->getDefenseModifier() > 0)
                total_stat_changes += active_user->getDefenseModifier();
            if(active_user->getSpecialDefenseModifier() > 0)
                total_stat_changes += active_user->getSpecialDefenseModifier();
            if(active_user->getSpeedModifier() > 0)
                total_stat_changes += active_user->getSpeedModifier();
            if(active_user->getAccuracyModifier() > 0)
                total_stat_changes += active_user->getAccuracyModifier();
            if(active_user->getEvasionModifier() > 0)
                total_stat_changes += active_user->getEvasionModifier();
            base_power *= total_stat_changes;
            break;
        }
        case 94:{//round
            if(rounds_used_this_turn>0)
                base_power *= 2;
            break;
        }
        case 111:{
            //power depends on target weight
            unsigned int weight = active_target->getWeight();
            if(weight<100)
                base_power = 20;
            else if(weight<250)
                base_power = 40;
            else if(weight<500)
                base_power = 60;
            else if(weight<1000)
                base_power = 80;
            else if(weight<2000)
                base_power = 100;
            else
                base_power = 120;
            break;
        }
        case 116:{
            //double power if last move failed
            if(active_user->lastAttackFailed()){
                base_power *= 2;
            }
            break;
        }
        case 118:{
            // power increases as many times as the use has been hit
            unsigned int hit_counter = active_user->numberOfHitsTaken();
            base_power *= (min(hit_counter,6)+1);
            break;
        }
        case 119:{
            // power doubles if ally was KOed last turn
            unsigned int last_killed;
            if(actor==PLAYER){
                last_killed = turn_of_opponent_last_kill;
            }else{
                last_killed = turn_of_player_last_kill;
            }
            if((last_killed == turn) || (last_killed == turn-1)){
                base_power *= 2;
            }
            break;
        }
        case 120:{
            //power depends on user current % of HP
            double current_hp_percentage = (double)active_user->getCurrentHP() / (double)active_user->getMaxHP() * 100;
            if(current_hp_percentage < 4.2)
                base_power = 200;
            else if(current_hp_percentage < 10.4)
                base_power = 150;
            else if(current_hp_percentage < 20.8)
                base_power = 100;
            else if(current_hp_percentage < 35.4)
                base_power = 80;
            else if(current_hp_percentage < 68.8)
                base_power = 40;
            else
                base_power = 20;
            break;
        }
        case 131:{
            //power is doubled if user was hit this turn
            unsigned int damage_taken = active_user->getPhysicalDamageTakenThisTurn() +
                active_user->getSpecialDamageTakenThisTurn();
            if(damage_taken > 0){
                base_power *= 2;
            }
            break;
        }
        case 134:case 219:{
            // power doubles if target has Permanent status
            if(active_target->hasPermanentStatus()){
                base_power *= 2;
            }
            break;
        }
        //check for surf/whirlpool -> doubles power when opponent is underwater
        case 135:case 158:{
            if(active_target->hasVolatileCondition(UNDERWATER)){
                base_power *= 2.0;
            }  
            break;  
        }
        case 140:{
            //power depends on % oh weight
            double weight_percentage = (double)active_target->getWeight() / (double)active_user->getWeight() * 100;
            if(weight_percentage > 50){
                base_power = 40;
            }else if(weight_percentage > 33.34){
                base_power = 60;
            }else if(weight_percentage > 25){
                base_power = 80;
            }else if(weight_percentage > 20){
                base_power = 100;
            }else{
                base_power = 120;
            }
            break;
        }
        case 155:{
            // power doubles if HP < 50%
            if(active_user->getCurrentHP() <= (active_user->getMaxHP()+1) / 2){
                base_power *= 2;
            }
            break;
        }
        case 160:{
            // power doubles if attack after target
            if(attack_after_target)
                base_power*=2;
            break;
        }
        case 165:{
            // power depends on current user %v of HP
            double current_hp_percentage = (double)active_user->getCurrentHP() / (double)active_user->getMaxHP() * 100;
            if(current_hp_percentage < 4.2)
                base_power = 200;
            else if(current_hp_percentage < 9.4)
                base_power = 150;
            else if(current_hp_percentage < 20.3)
                base_power = 100;
            else if(current_hp_percentage < 34.4)
                base_power = 80;
            else if(current_hp_percentage < 67.2)
                base_power = 40;
            else
                base_power = 20;
            break;
        }
        // remove target item if any and double power if target had an item
        case 203:{
            if(active_user->hasHeldItem() && active_target->canStealItem()){
                if(!active_target->hasSubstitute()){
                    ItemType target_item = active_target->getMonster()->getHeldItem();
                    ItemData * target_item_data = ItemData::getItemData(target_item);
                    active_target->removeHeldItem();
                    if(actor==PLAYER)
                        item_on_the_ground_opponent = target_item;
                    else
                        item_on_the_ground_player = target_item;
                    event_handler->displayMsg(opponent_mon_name+"'s "+target_item_data->getName()+" was knocked off!");
                }
                base_power *= 2;
            }
            break;
        }
        case 204:{
            // fling
            ItemType target_item = active_user->getMonster()->getHeldItem();
            base_power = flingPower(target_item);
            break;
        }
        case 218:{
            //power decreases with lower HP user
            base_power *= (double)active_user->getCurrentHP() / (double)active_user->getMaxHP();
            break;
        }
        case 230:{
            //power is doubled if user does not hold items
            if(!active_user->hasHeldItem()){
                base_power *= 2;
            }
            break;
        }
        case 241:{
            //power is doubled if opponent is poisoned
            if(active_target->isPoisoned()){
                base_power *= 2;
            }
            break;
        }
        case 246:{
            //power depends on i-th team member of the team
            Monster* ith_monster = active_team->getMonster(beat_up_index);
            if(ith_monster == nullptr)
                break;
            if(ith_monster->isFainted())
                break;
            base_power = ith_monster->getStats().getAtk()/10 + 5;            
        }
        case 249:{
            //present: power can be among 40, 80 & 120
            unsigned int random_number = RNG::getRandomInteger(1,80);
            if(random_number<=10)
                base_power = 120;
            else if(random_number<=40)
                base_power = 80;
            else
                base_power = 40;
            break;
        }
        case 251:{
            //power is 10 + 10*beat_up_index
            base_power = 10 + 10*beat_up_index;
            break;
        }
        case 254:{
            //power is doubled in non clear weather
            if(field->getWeather() != CLEAR){
                base_power *= 2;
            }
            break;
        }
        case 257:{
            //power doubles if user has status ammong paralysis, burn and poison
            if(active_user->isBurned() || active_user->isParalyzed() || active_user->isPoisoned()){
                base_power *= 2;
            }
            break;
        }
        case 259:{
            //power depends on target HP
            double current_hp = active_target->getCurrentHP();
            double max_hp = active_target->getMaxHP();
            base_power = 100.0 * current_hp / max_hp;
            break;
        }
        case 289:{
            //power depends on target HP
            double current_hp = active_target->getCurrentHP();
            double max_hp = active_target->getMaxHP();
            base_power = 120.0 * current_hp / max_hp;
            break;
        }
        case 269:{
            //if stats were lowered this turn, power is doubled
            if(active_user->hasVolatileCondition(STAT_JUST_DROPPED)){
                base_power *= 2;
            }
            break;
        }
        case 277:{
            //power is doubled if opponent has used minimize
            if(active_target->hasUsedAttack(MINIMIZE_ID)){
                base_power *= 2;
            }
            break;
        }
        case 306:{
            //power is doubled if last attack used in battle was Fusion Bolt
            if(last_attack_used_id == FUSION_BOLT_ID){
                base_power *= 2;
            }
            break;
        }
        case 307:{
            //power is doubled if last attack used in battle was Fusion Flare
            if(last_attack_used_id == FUSION_FLARE_ID){
                base_power *= 2;
            }
            break;
        }
        default: break;
    }

    // ability modifiers
    switch(active_user->getAbility()){
        case OVERGROW:{
            if(attack_type == GRASS && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case BLAZE:{
            if(attack_type == FIRE && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case TORRENT:{
            if(attack_type == WATER && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case SWARM:{
            if(attack_type == BUG && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case RIVALRY:{
            Gender gender_user = active_user->getGender();
            Gender gender_target = active_target->getGender();
            if(gender_target == gender_user && gender_user!=GENDERLESS)//same gender
                base_power *= 1.25;
            else if(areMaleAndFemale(gender_user,gender_target))//opposite gender
                base_power *= 0.75;
            break;
        }
        case SHEER_FORCE:{
            if(attack->getEffectId()!=0 && attack->getEffectTarget() == TARGET_OPPONENT){
                base_power *= 5325.0 / 4096.0;
            }
            break;
        }
        case FLASH_FIRE:{
            if(attack_type == FIRE && active_user->hasVolatileCondition(FLASH_FIRED)){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case SAND_FORCE:{
            if(field->getWeather() == SANDSTORM &&
                !thereIsaCloudNine() && (
                attack_type==STEEL ||
                attack_type==GROUND ||
                attack_type==ROCK))
                base_power*=1.3;
            break;
        }
        case TECHNICIAN:{
            if(base_power <= 60){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case ANALYTIC:{
            // boosts power of moves if hitting second
            if(attack_after_target){
                base_power *= 5325.0/4096.0;
            }
            break;
        }
        case RECKLESS:{
            //boost power of recoil moves
            if(effect==171 || effect==20 || effect==30 ||
                effect==51 || effect==6 || effect==174 ||
                effect==276 || effect==277)
                base_power *= 1.2;
            break;
        }
        case IRON_FIST:{
            //boost power of puncihng attacks
            if(attack->isPunching())
                base_power*=1.2;
            break;
        }
        case TOUGH_CLAWS:{
            //boost power of contact moves
            if(attack->makesContact())
                base_power*=5325/4096;
            break;
        }
        case MEGA_LAUNCHER:{
            //boost power of pulse moves
            if(attack->isPulse())
                base_power*=5325/4096;
            break;
        }
        case GALVANIZE:
        case REFRIGERATE:
        case PIXILATE:
        case AERILATE:{
            //boost power of moves that changed type from NORMAL to the type of the ability
            if(attack->getType() == NORMAL){
                base_power *= 1.2;
            }
            break;
        }
        case STRONG_JAW:{
            //boost power of biting moves
            if(attack->isBiting())
                base_power *= 1.5;
            break;
        }
        case SHARPNESS:{
            //boost power of slicing moves
            if(attack->isSlicing())
                base_power *= 1.5;
            break;
        }
        case TOXIC_BOOST:{
            //boost power of physical moves if poisoned
            if(active_user->isPoisoned() && attack->getCategory() == PHYSICAL){
                base_power *= 1.5;
            }
            break;
        }
        case FLARE_BOOST:{
            //boost power of special moves if burned
            if(active_user->isBurned() && attack->getCategory() == SPECIAL){
                base_power *= 1.5;
            }
            break;
        }
        default: break;
    }

    //terrain modifiers
    switch(field->getTerrain()){
        case GRASSY_FIELD:{
            if(attack_type == GRASS && active_user->isTouchingGround()){
                base_power *= 1.3;
            }
            break;
        }
        case MISTY_FIELD:{
            if(attack_type == DRAGON){
                base_power *= 0.5;
            }else if(effect==275){
                // misty explosion
                base_power *= 1.5;
            }
            break;
        }
        case ELECTRIC_FIELD:{
            if(attack_type == ELECTRIC && active_user->isTouchingGround()){
                base_power *= 1.3;
            }
            break;
        }
        case PSYCHIC_FIELD:{
            if(attack_type == PSYCHIC && active_user->isTouchingGround()){
                if(effect == 266)
                    base_power *= 1.5;
                else
                    base_power *= 1.3;
            }
            break;
        }
        default: break;
    }

    //volatile conditions modifiers
    if(active_user->hasVolatileCondition(CHARGED_2)){
        if(attack_type == ELECTRIC){
            base_power *= 2;
        }
    }

    // item modifiers
    ItemType item = active_user->getHeldItem();
    switch(item){
        case METAL_COAT:
        case IRON_PLATE:{
            if(attack_type == STEEL){
                base_power *= 1.2;
            }
            break;
        }
        case POISON_BARB:
        case TOXIC_PLATE:{
            if(attack_type == POISON){
                base_power *= 1.2;
            }
            break;
        }
        case BLACK_BELT:
        case FIST_PLATE:{
            if(attack_type == FIGHTING){
                base_power *= 1.2;
            }
            break;
        }
        case BLACK_GLASSES:
        case DREAD_PLATE:{
            if(attack_type == DARK){
                base_power *= 1.2;
            }
            break;
        }
        case CHARCOAL:
        case FLAME_PLATE:{
            if(attack_type == FIRE){
                base_power *= 1.2;
            }
            break;
        }
        case DRAGON_FANG:
        case DRACO_PLATE:{
            if(attack_type == DRAGON){
                base_power *= 1.2;
            }
            break;
        }
        case FAIRY_FEATHER:
        case PIXIE_PLATE:{
            if(attack_type == FAIRY){
                base_power *= 1.2;
            }
            break;
        }
        case HARD_STONE:
        case ROCK_INCENSE:
        case STONE_PLATE:{
            if(attack_type == ROCK){
                base_power *= 1.2;
            }
            break;
        }
        case MAGNET:
        case ZAP_PLATE:{
            if(attack_type == ELECTRIC){
                base_power *= 1.2;
            }
            break;
        }
        case MIRACLE_SEED:
        case ROSE_INCENSE:
        case MEADOW_PLATE:{
            if(attack_type == GRASS){
                base_power *= 1.2;
            }
            break;
        }
        case MYSTIC_WATER:
        case WAVE_INCENSE:
        case SEA_INCENSE:
        case SPLASH_PLATE:{
            if(attack_type == WATER){
                base_power *= 1.2;
            }
            break;
        }
        case NEVER_MELT_ICE:
        case ICICLE_PLATE:{
            if(attack_type == ICE){
                base_power *= 1.2;
            }
            break;
        }
        case SILK_SCARF:{
            if(attack_type == NORMAL){
                base_power *= 1.2;
            }
            break;
        }
        case SHARP_BEAK:
        case SKY_PLATE:{
            if(attack_type == FLYING){
                base_power *= 1.2;
            }
            break;
        }
        case SOFT_SAND:
        case EARTH_PLATE:{
            if(attack_type == GROUND){
                base_power *= 1.2;
            }
            break;
        }
        case SILVER_POWDER:
        case INSECT_PLATE:{
            if(attack_type == BUG){
                base_power *= 1.2;
            }
            break;
        }
        case SPELL_TAG:
        case SPOOKY_PLATE:{
            if(attack_type == GHOST){
                base_power *= 1.2;
            }
            break;
        }
        case TWISTED_SPOON:
        case MIND_PLATE:
        case ODD_INCENSE:{
            if(attack_type == PSYCHIC){
                base_power *= 1.2;
            }
            break;
        }
        case METRONOME_ITEM:{
            if(active_user->getLastAttackUsed() == attack_id){
                unsigned int metronome_counter = active_user->getSameAttackCounter() - 1;
                if(metronome_counter > 1){
                    base_power *= (1 + (0.2 * min(5,metronome_counter)));
                }
            }
            break;
        }
        case MUSCLE_BAND:{
            if(attack->getCategory() == PHYSICAL){
                base_power *= 1.2;
            }
            break;
        }
        case WISE_GLASSES:{
            if(attack->getCategory() == SPECIAL){
                base_power *= 1.2;
            }
            break;
        }
        case PUNCHING_GLOVE:{
            if(attack->isPunching()){
                base_power *= 1.1;
            }
            break;
        }
        case FIRE_GEM:{
            if(attack_type == FIRE){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case WATER_GEM:{
            if(attack_type == WATER){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case GRASS_GEM:{
            if(attack_type == GRASS){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case ELECTRIC_GEM:{
            if(attack_type == ELECTRIC){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case ICE_GEM:{
            if(attack_type == ICE){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case FIGHTING_GEM:{
            if(attack_type == FIGHTING){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case BUG_GEM:{
            if(attack_type == BUG){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case DARK_GEM:{
            if(attack_type == DARK){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case GHOST_GEM:{
            if(attack_type == GHOST){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case DRAGON_GEM:{
            if(attack_type == DRAGON){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case FAIRY_GEM:{
            if(attack_type == FAIRY){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case ROCK_GEM:{
            if(attack_type == ROCK){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case GROUND_GEM:{
            if(attack_type == GROUND){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case PSYCHIC_GEM:{
            if(attack_type == PSYCHIC){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case STEEL_GEM:{
            if(attack_type == STEEL){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case POISON_GEM:{
            if(attack_type == POISON){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case FLYING_GEM:{
            if(attack_type == FLYING){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case NORMAL_GEM:{
            if(attack_type == NORMAL){
                base_power *= 1.3;
                active_user->consumeHeldItem();
            }
            break;
        }
        case GRISEOUS_CORE:
        case GRISEOUS_ORB:{
            //boost power of dragon and ghost moves of giratina (487)
            if(active_user->getMonster()->getSpeciesId() != 487){
                break;
            }
            if(attack_type == GHOST || attack_type == DRAGON){
                base_power *= 1.2;
            }
            break;
        }
        case ADAMANT_ORB:
        case ADAMANT_CRYSTAL:{
            //boost power of dragon and steel moves of dialga (483)
            if(active_user->getMonster()->getSpeciesId() != 483){
                break;
            }
            if(attack_type == STEEL || attack_type == DRAGON){
                base_power *= 1.2;
            }
            break;
        }
        case LUSTROUS_ORB:
        case LUSTROUS_GLOBE:{
            //boost power of dragon and water moves of palkia (484)
            if(active_user->getMonster()->getSpeciesId() != 484){
                break;
            }
            if(attack_type == WATER || attack_type == DRAGON){
                base_power *= 1.2;
            }
            break;
        }
        default:break;
    }

    return base_power;
}

void Battle::applyPostDamage(){
    std::vector<BattleActionActor> actors;
    for(auto battler: getBattlersSortedBySpeed())
        actors.push_back(battler->getActor());
    // apply items
    for(auto actor: actors){
        applyItemPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }
    // update field
    field->nextTurnWeather();
    // apply weather
    for(auto actor: actors){
        applyWeatherPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }
    field->nextTurnTerrain();
    // apply terrain effects
    for(auto actor: actors){
        applyTerrainPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }
    // apply permanent
    for(auto actor: actors){
        applyPermanentStatusPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }
    // apply volatile
    for(auto actor: actors){
        applyVolatileStatusPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }

    // apply abilities
    for(auto actor: actors){
        applyAbilityPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }

    //field effects
    for(auto actor: actors){
        applyFieldEffectsPostDamage(actor);
        checkForExp();
        if(isOver()){
            return;
        }
    }
    field->nextTurnFieldEffect();

    
}

void Battle::applyTerrainPostDamage(BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    std::string mon_name = getActorBattlerName(actor);
    if(active_user->isFainted())
        return;
    Terrain terrain = field->getTerrain();
    switch(terrain){
        case GRASSY_FIELD:{
            if(active_user->isTouchingGround()){
                unsigned int grassy_heal = max((active_user->getMaxHP()+15) / 16,1);
                unsigned int actual_grassy_heal = active_user->removeDamage(grassy_heal);
                if(actual_grassy_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_grassy_heal)+" HP thanks to Grassy Terrain!");
            }
        }
        default: break;
    }
    tryEjectPack(actor);
}
    

void Battle::applyPermanentStatusPostDamage(BattleActionActor actor){
    Battler* active_user=getActorBattler(actor);
    if(active_user->isFainted())
        return;
    std::string mon_name = getActorBattlerName(actor);
    PermanentStatusCondition status = active_user->getPermanentStatus();
    //reset BAD POISON counter if necessary
    if(status != BAD_POISON){
        active_user->resetBadPoisonCounter();
    }
    // check shed skin
    if(active_user->hasAbility(SHED_SKIN) &&  
        (active_user->isPoisoned() || active_user->isBurned() || active_user->isParalyzed())){
        if(RNG::getRandomInteger(0,2) == 0){
            event_handler->displayMsg(mon_name+" Shed Skin activates!");
            active_user->clearPermanentStatus();
            return;
        }
    }
    // apply permanent status damage
    if(active_user->hasAbility(MAGIC_GUARD))//MAGIC GUARD prevents status damage
        return;
    switch(status){
        case BURN:{
            unsigned int burn_damage = max(active_user->getMaxHP() / 16,1);
            unsigned int actual_burn_damage = active_user->addDirectDamage(burn_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_burn_damage)+" burn damage!");
            event_handler->displayDmgDealt(actual_burn_damage,active_user,"its Burn");
            if(active_user->isFainted())
                return;
            break;
        }
        case POISONED:{
            if(active_user->hasAbility(POISON_HEAL)){
                event_handler->displayMsg(mon_name+" is healed by Poison Heal!");
                unsigned int actual_heal = active_user->removeDamage(active_user->getMaxHP()/8);
                if(actual_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_heal)+" HP!");
                break;
            }
            unsigned int psn_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_psn_damage = active_user->addDirectDamage(psn_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_psn_damage)+" poison damage!");
            event_handler->displayDmgDealt(actual_psn_damage,active_user,"Poison");
            if(active_user->isFainted())
                return;
            break;
        }
        case BAD_POISON:{
            if(active_user->hasAbility(POISON_HEAL)){
                event_handler->displayMsg(mon_name+" is healed by Poison Heal!");
                unsigned int actual_heal = active_user->removeDamage(active_user->getMaxHP()/8);
                if(actual_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_heal)+" HP!");
                break;
            }
            unsigned int badpsn_damage = max(active_user->getMaxHP() * active_user->getBadPoisonCounter() / 16,1);
            unsigned int actual_badpsn_damage = active_user->addDirectDamage(badpsn_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_badpsn_damage)+" poison damage!");
            event_handler->displayDmgDealt(actual_badpsn_damage,active_user,"Poison");
            if(active_user->isFainted())
                return;
            active_user->increaseBadPoisonCounter();
            break;
        }
        default: break;
    }
    tryEjectPack(actor);
}

void Battle::applyVolatileStatusPostDamage(BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    if(active_user->isFainted())
        return;
    Battler* active_opponent = getActorBattler(otherBattleActionActor(actor));
    if(active_user->isFainted()){return;}
    std::string mon_name = getActorBattlerName(actor);
    std::string opponent_name = getActorBattlerName(otherBattleActionActor(actor));
    //apply aqua ring heal
    if(active_user->hasVolatileCondition(AQUA_RING)){
        unsigned int aqua_ring_heal = max((active_user->getMaxHP()+15) / 16,1);
        if(active_user->hasHeldItem(BIG_ROOT)){
            aqua_ring_heal *= 1.3;
        }
        unsigned int actual_aqua_ring_heal = active_user->removeDamage(aqua_ring_heal);
        if(actual_aqua_ring_heal>0)
            event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_aqua_ring_heal)+" HP thanks to Aqua Ring!");
    }
    //apply ingrain heal
    if(active_user->hasVolatileCondition(INGRAINED)){
        unsigned int ingrain_heal = max((active_user->getMaxHP()+15) / 16,1);
        if(active_user->hasHeldItem(BIG_ROOT)){
            ingrain_heal *= 1.3;
        }
        unsigned int actual_ingrain_heal = active_user->removeDamage(ingrain_heal);
        if(actual_ingrain_heal>0)
            event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_ingrain_heal)+" HP thanks to Ingrain!");
    }
    // apply leech seed
    if(!active_user->hasAbility(MAGIC_GUARD)){//magic guard prevents effect damage
        if(active_user->hasVolatileCondition(LEECH_SEED) && !active_opponent->isFainted()){
            unsigned int leech_seed_damage = max(active_user->getMaxHP() / 8,1);
            if(active_opponent->hasHeldItem(BIG_ROOT)){
                leech_seed_damage *= 1.3;
            }
            unsigned int actual_leech_seed_damage = active_user->addDirectDamage(leech_seed_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_leech_seed_damage)+" leech seed damage!");
            event_handler->displayDmgDealt(actual_leech_seed_damage,active_user,"its seeds");
            if(active_user->hasAbility(LIQUID_OOZE)){// if user has liquid ooze, user takes dmg instead of recovering health
                // event_handler->displayMsg(opponent_name+" is hurt by Liquid Ooze!");
                unsigned int liquid_ooze_damage = actual_leech_seed_damage;
                unsigned int actual_liquid_ooze_damage = active_opponent->addDirectDamage(liquid_ooze_damage);
                // event_handler->displayMsg(opponent_name+" took "+std::to_string(actual_liquid_ooze_damage)+" Liquid Ooze damage!");
                event_handler->displayDmgDealt(actual_liquid_ooze_damage,active_opponent,mon_name+"'s Liquid Ooze");
            }
            else{
                unsigned int actual_heal_amount = active_opponent->removeDamage(actual_leech_seed_damage);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(opponent_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
            }
            if(active_user->isFainted())
                return;
        }
        if(active_user->isFainted()){return;}
        //apply WRAP
        if(active_user->hasVolatileCondition(WRAP) && !active_opponent->isFainted()){
            event_handler->displayMsg(mon_name+" is hurt by Wrap!");
            unsigned int wrap_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                wrap_damage = max(active_user->getMaxHP() / 6,1);
            else
                wrap_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_wrap_damage = active_user->addDirectDamage(wrap_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_wrap_damage)+" Wrap damage!");
            event_handler->displayDmgDealt(actual_wrap_damage,active_user,"Wrap");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(WRAP);
        }else{
            active_user->removeVolatileCondition(WRAP);
        }
        if(active_user->isFainted()){return;}
        //apply BIND
        if(active_user->hasVolatileCondition(BIND) && !active_opponent->isFainted()){
            event_handler->displayMsg(mon_name+" is hurt by Bind!");
            unsigned int bind_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                bind_damage = max(active_user->getMaxHP() / 6,1);
            else
                bind_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_bind_damage = active_user->addDirectDamage(bind_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_bind_damage)+" Bind damage!");
            event_handler->displayDmgDealt(actual_bind_damage,active_user,"Bind");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(BIND);
        }else{
            active_user->removeVolatileCondition(BIND);
        }
        if(active_user->isFainted()){return;}
        //apply SNAP_TRAP
        if(active_user->hasVolatileCondition(SNAP_TRAP) && !active_opponent->isFainted()){
            event_handler->displayMsg(mon_name+" is hurt by Snap Trap!");
            unsigned int st_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                st_damage = max(active_user->getMaxHP() / 6,1);
            else
                st_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_st_damage = active_user->addDirectDamage(st_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_st_damage)+" Snap Trap damage!");
            event_handler->displayDmgDealt(actual_st_damage,active_user,"Snap Trap");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(SNAP_TRAP);
        }else{
            active_user->removeVolatileCondition(SNAP_TRAP);
        }
        if(active_user->isFainted()){return;}
        //fire spin
        if(active_user->hasVolatileCondition(FIRESPIN)){
            event_handler->displayMsg(mon_name+" is trapped in a Fire Spin!");
            unsigned int fire_spin_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                fire_spin_damage = max(active_user->getMaxHP() / 6,1);
            else
                fire_spin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_fire_spin_damage = active_user->addDirectDamage(fire_spin_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_fire_spin_damage)+" fire spin damage!");
            event_handler->displayDmgDealt(actual_fire_spin_damage,active_user,"Fire Spin");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(FIRESPIN);
        }
        if(active_user->isFainted()){return;}
        //magma storm
        if(active_user->hasVolatileCondition(MAGMA_STORM)){
            event_handler->displayMsg(mon_name+" is trapped in a mealstorm of fire!");
            unsigned int fire_spin_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                fire_spin_damage = max(active_user->getMaxHP() / 6,1);
            else
                fire_spin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_fire_spin_damage = active_user->addDirectDamage(fire_spin_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_fire_spin_damage)+" damage from the fire!");
            event_handler->displayDmgDealt(actual_fire_spin_damage,active_user,"Magma Storm");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(MAGMA_STORM);
        }
        if(active_user->isFainted()){return;}
        //infested
        if(active_user->hasVolatileCondition(INFESTED)){
            event_handler->displayMsg(mon_name+" is infested!");
            unsigned int infestation_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                infestation_damage = max(active_user->getMaxHP() / 6,1);
            else
                infestation_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_infestation_damage = active_user->addDirectDamage(infestation_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_infestation_damage)+" infestation damage!");
            event_handler->displayDmgDealt(actual_infestation_damage,active_user,"Infestation");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(INFESTED);
        }
        if(active_user->isFainted()){return;}
        //whirlpool
        if(active_user->hasVolatileCondition(WHIRLPOOL)){
            event_handler->displayMsg(mon_name+" is trapped in a Whirlpool!");
            unsigned int whirlpool_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                whirlpool_damage = max(active_user->getMaxHP() / 6,1);
            else
                whirlpool_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_whirlpool_damage = active_user->addDirectDamage(whirlpool_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_whirlpool_damage)+" Whirlpool damage!");
            event_handler->displayDmgDealt(actual_whirlpool_damage,active_user,"Whirlpool");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(WHIRLPOOL);
        }
        if(active_user->isFainted()){return;}
        //sand tomb
        if(active_user->hasVolatileCondition(SANDTOMB)){
            event_handler->displayMsg(mon_name+" is trapped in a vortex of sand!");
            unsigned int sand_damage;
            if(active_opponent->hasHeldItem(BINDING_BAND))
                sand_damage = max(active_user->getMaxHP() / 6,1);
            else
                sand_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_sand_damage = active_user->addDirectDamage(sand_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_sand_damage)+" sand tomb damage!");
            event_handler->displayDmgDealt(actual_sand_damage,active_user,"Sand Tomb");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(SANDTOMB);
        }
        if(active_user->isFainted()){return;}
        //curse
        if(active_user->hasVolatileCondition(CURSED)){
            event_handler->displayMsg(mon_name+" is hurt by the curse!");
            unsigned int curse_damage = max(active_user->getMaxHP() / 4,1);
            unsigned int actual_curse_damage = active_user->addDirectDamage(curse_damage);
            // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_curse_damage)+" curse damage!");
            event_handler->displayDmgDealt(actual_curse_damage,active_user,"the Curse");
            if(active_user->isFainted())
                return;
        }
        if(active_user->isFainted()){return;}
    }
    // perish song
    if(active_user->hasVolatileCondition(PERISH_SONG)){
        active_user->decrementVolatileCondition(PERISH_SONG);
        if(!active_user->hasVolatileCondition(PERISH_SONG)){// perish song counter is at 0!
            active_user->addDirectDamage(active_user->getMaxHP());// user dies from perish song
        }
    }
    if(active_user->isFainted()){return;}
    // remove flinch
    if(active_user->hasVolatileCondition(FLINCH)){
        active_user->removeVolatileCondition(FLINCH);
    }
    // remove Moving first
    if(active_user->hasVolatileCondition(MOVING_FIRST)){
        active_user->removeVolatileCondition(MOVING_FIRST);
    }
    // remove incresed crit
    if(active_user->hasVolatileCondition(INCREASED_CRIT)){
        active_user->removeVolatileCondition(INCREASED_CRIT);
    }
    // remove incresed accuracy
    if(active_user->hasVolatileCondition(INCREASED_ACCURACY)){
        active_user->removeVolatileCondition(INCREASED_ACCURACY);
    }
    // remove protect
    if(active_user->hasVolatileCondition(PROTECT)){
        active_user->removeVolatileCondition(PROTECT);
    }
    // remove crafty shield
    if(active_user->hasVolatileCondition(CRAFTY_SHIELD)){
        active_user->removeVolatileCondition(CRAFTY_SHIELD);
    }
    // remove spiky protect
    if(active_user->hasVolatileCondition(SPIKY_PROTECT)){
        active_user->removeVolatileCondition(SPIKY_PROTECT);
    }
    // remove quick guard
    if(active_user->hasVolatileCondition(QUICK_GUARD)){
        active_user->removeVolatileCondition(QUICK_GUARD);
    }
    //decrement magnet rise
    if(active_user->hasVolatileCondition(MAGNET_RISE)){
        active_user->decrementVolatileCondition(MAGNET_RISE);
    }
    // drowsyness
    if(isUproar() && !active_user->hasAbility(SOUNDPROOF)){
        active_user->removeVolatileCondition(DROWSY);
    }
    if(active_user->hasVolatileCondition(DROWSY)){
        active_user->decrementVolatileCondition(DROWSY);
        if(!active_user->hasVolatileCondition(DROWSY) &&
            active_user->canFallAsleep()){
            unsigned int random_number = RNG::getRandomInteger(1,3);
            if(random_number == 1){
                active_user->setPermanentStatus(SLEEP_2);
            }else if(random_number == 2){
                active_user->setPermanentStatus(SLEEP_3);
            }else{
                active_user->setPermanentStatus(SLEEP_4);
            }
        }
    }
    tryEjectPack(actor);
}

void Battle::applyWeatherPostDamage(BattleActionActor actor){
    Battler* active_user=getActorBattler(actor);
    if(active_user->isFainted())
        return;
    // Battler * enemy_active = getActorBattler(otherBattleActionActor(actor));
    std::string mon_name = getActorBattlerName(actor);
    if(thereIsaCloudNine())
        return;

    // apply weather
    switch(field->getWeather()){
        case SANDSTORM:{
            if(thereIsaCloudNine())
                break;
            if(!(active_user->hasType(ROCK) || active_user->hasType(GROUND) || 
            active_user->hasType(STEEL) || 
            active_user->hasAbility(SAND_RUSH) ||
            active_user->hasAbility(SAND_VEIL) ||
            active_user->hasAbility(MAGIC_GUARD) ||
            active_user->hasAbility(OVERCOAT) ||
            active_user->hasHeldItem(SAFETY_GOGGLES) ||
            active_user->hasAbility(SAND_FORCE))){
                event_handler->displayMsg(mon_name+" is buffeted by the sandstorm!");
                unsigned int sandstorm_damage = max(active_user->getMaxHP() / 16,1);
                unsigned int actual_sandstorm_damage = active_user->addDirectDamage(sandstorm_damage);
                // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_sandstorm_damage)+" sandstorm damage!");
                event_handler->displayDmgDealt(actual_sandstorm_damage,active_user,"the Sandstorm");
                if(active_user->isFainted())
                    return;
            }
            break;
        }
        case HAIL:{
            if(thereIsaCloudNine())
                break;
            if(!(active_user->hasType(ICE)||
                active_user->hasAbility(OVERCOAT) ||
                active_user->hasHeldItem(SAFETY_GOGGLES) ||
                active_user->hasAbility(ICE_BODY) ||
                active_user->hasAbility(SNOW_CLOAK) ||
                active_user->hasAbility(MAGIC_GUARD))){
                event_handler->displayMsg(mon_name+" is buffeted by the hail!");
                unsigned int hail_damage = max(active_user->getMaxHP() / 16,1);
                unsigned int actual_hail_damage = active_user->addDirectDamage(hail_damage);
                // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_hail_damage)+" hail damage!");
                event_handler->displayDmgDealt(actual_hail_damage,active_user,"the Hail");
                if(active_user->isFainted())
                    return;
            }
            if(active_user->hasAbility(ICE_BODY) && 
                !active_user->isAtFullHP()){
                event_handler->displayMsg(mon_name+" is healed by the hail!");
                unsigned int ice_body_heal = max((active_user->getMaxHP()+15) / 16,1);
                unsigned int actual_ice_body_heal = active_user->removeDamage(ice_body_heal);
                if(actual_ice_body_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_ice_body_heal)+" HP!");
            }
            break;
        }
        case SNOWSTORM:{
            if(thereIsaCloudNine())
                break;
            if(active_user->hasAbility(ICE_BODY) && 
                !active_user->isAtFullHP()){
                event_handler->displayMsg(mon_name+" is healed by the snow!");
                unsigned int ice_body_heal = max((active_user->getMaxHP()+15) / 16,1);
                unsigned int actual_ice_body_heal = active_user->removeDamage(ice_body_heal);
                if(actual_ice_body_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_ice_body_heal)+" HP!");
            }
            break;
        }
        case SUN:{
            if(active_user->hasHeldItem(UTILITY_UMBRELLA) || thereIsaCloudNine())
                break;
            if (active_user->hasAbility(SOLAR_POWER) || active_user->hasAbility(DRY_SKIN)){ // SOLAR POWER DAMAGE
                std::string ability_name = abilityToString(active_user->getAbility());
                event_handler->displayMsg(mon_name+" is losing HP due to "+ability_name+"!");
                unsigned int solar_power_damage = max(active_user->getMaxHP()+7 / 8,1);
                unsigned int actual_solar_power_damage = active_user->addDirectDamage(solar_power_damage);
                // event_handler->displayMsg(mon_name+" took "+std::to_string(actual_solar_power_damage)+" "+ability_name+" damage!");
                event_handler->displayDmgDealt(actual_solar_power_damage,active_user,ability_name);
                if(active_user->isFainted())
                    return;
            }
            break;
        }
        case RAIN:
        case HEAVY_RAIN:{
            if(active_user->hasHeldItem(UTILITY_UMBRELLA) || thereIsaCloudNine())
                break;
            if((active_user->hasAbility(RAIN_DISH) || 
                active_user->hasAbility(DRY_SKIN)) &&
                !active_user->isAtFullHP()){ // RAIN DISH HEAL
                event_handler->displayMsg(mon_name+" is healed by the rain!");
                unsigned int rain_heal = 0;
                if(active_user->hasAbility(DRY_SKIN))
                    rain_heal = max((active_user->getMaxHP()+7)/ 8,1);
                else if(active_user->hasAbility(RAIN_DISH))
                    rain_heal = max((active_user->getMaxHP()+15)/ 16,1);
                unsigned int actual_rain_heal = active_user->removeDamage(rain_heal);
                if(actual_rain_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_rain_heal)+" HP!");
            }
            // hydration heals permentent statuses during rain
            if(active_user->hasAbility(HYDRATION) && 
                !active_user->isFainted() &&
                active_user->hasPermanentStatus()){
                event_handler->displayMsg(mon_name+" is cured of its status condition!");
                active_user->clearPermanentStatus();
            }
            break;
        }
        default: break;
    }
    tryEjectPack(actor);
}

void Battle::applyFieldEffectsPostDamage(BattleActionActor actor){
    Battler* active_user=getActorBattler(actor);
    std::string mon_name = getActorBattlerName(actor);
    
    // wish
    if(field->hasFieldEffect(WISH,actor)){
        field->decrementFieldEffect(WISH,actor);
        if(!field->hasFieldEffect(WISH,actor)){
            event_handler->displayMsg("The wish comes true!");
            if(active_user->isFainted()){
                event_handler->displayMsg("But nothing happens!");
            }else{
                unsigned int wish_heal = max((active_user->getMaxHP()+1) / 2,1);
                unsigned int actual_wish_heal = active_user->removeDamage(wish_heal);
                if(actual_wish_heal>0)
                    event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_wish_heal)+" HP thanks to Wish!");
            }
        }
    }
    // std::string mon_name = getActorBattlerName(actor);
    // apply field effects
    // std::set<FieldEffect> field_effects = field->getFieldEffects(actor);
    // for(auto it = field_effects.begin(); it != field_effects.end(); it++){
    //     FieldEffect field_effect = *it;
    //     switch(field_effect){
    //         default:break;
    //     }
    // }   
    tryEjectPack(actor);
}


bool Battle::performEntryHazardCheck(BattleActionActor actor){
    Battler* active_battler = getActorBattler(actor);
    if(active_battler->isFainted())
        return false;
    if((field->hasFieldEffect(TOXIC_SPIKES,actor) || field->hasFieldEffect(BAD_TOXIC_SPIKES,actor))&&
        active_battler->hasType(POISON) && active_battler->isTouchingGround()){
        field->clearFieldEffect(TOXIC_SPIKES,actor);
        field->clearFieldEffect(BAD_TOXIC_SPIKES,actor);
    }
    if(active_battler->hasHeldItem(HEAVY_DUTY_BOOTS)){
        //heavy duty boots avoid all effects of entry hazards
        return false;
    }
    // toxic spikes
    if(field->hasFieldEffect(TOXIC_SPIKES,actor) && active_battler->isTouchingGround()){
        active_battler->setPermanentStatus(POISONED);
    }else if(field->hasFieldEffect(BAD_TOXIC_SPIKES,actor) && active_battler->isTouchingGround()){
        active_battler->setPermanentStatus(BAD_POISON);
    }
    // spikes
    unsigned int spikes_damage = 0;
    unsigned int maxHP = active_battler->getMaxHP();
    if(field->hasFieldEffect(SPIKES,actor) && active_battler->isTouchingGround()){
        spikes_damage = max(maxHP / 8,1);
    }else if(field->hasFieldEffect(SPIKES_2,actor) && active_battler->isTouchingGround()){
        spikes_damage = max(maxHP / 6,1);
    }else if(field->hasFieldEffect(SPIKES_3,actor) && active_battler->isTouchingGround()){
        spikes_damage = max(maxHP / 4,1);
    }
    if(spikes_damage>0){
        unsigned int actual_spikes_damage = active_battler->addDirectDamage(spikes_damage);
        if(actual_spikes_damage > 0){
            std::string battler_name = getActorBattlerName(actor);
            // event_handler->displayMsg(battler_name+" took "+std::to_string(actual_spikes_damage)+" damage from the Spikes!");
            event_handler->displayDmgDealt(actual_spikes_damage,active_battler,"Spikes");
        }
    }
    if(active_battler->isFainted())
        return false;
    // stealth rocks
    if(field->hasFieldEffect(STEALTH_ROCKS,actor)){
        float effectiveness = getTypeEffectiveness(ROCK, active_battler->getTypes(), active_battler->isTouchingGround(), false, false);
        unsigned int stealth_rock_damage = 0;
        if(effectiveness > 2.1){
            stealth_rock_damage = max(1,active_battler->getMaxHP() / 2);
        }else if(effectiveness > 1.1){
            stealth_rock_damage = max(1,active_battler->getMaxHP() / 4);
        }else if(effectiveness > 0.6){
            stealth_rock_damage = max(1,active_battler->getMaxHP() / 8);
        }else if(effectiveness > 0.4){
            stealth_rock_damage = max(1,active_battler->getMaxHP() / 16);
        }else{
            stealth_rock_damage = max(1,active_battler->getMaxHP() / 32);
        }
        unsigned int actual_stealth_rock_damage = active_battler->addDirectDamage(stealth_rock_damage);
        if(actual_stealth_rock_damage > 0){
            std::string battler_name = getActorBattlerName(actor);
            // event_handler->displayMsg(battler_name+" took "+std::to_string(actual_stealth_rock_damage)+" damage from the Stealth Rocks!");
            event_handler->displayDmgDealt(actual_stealth_rock_damage,active_battler,"Stealth Rocks");
        }
    }
    if(active_battler->isFainted())
        return false;
    // sticky web
    if(field->hasFieldEffect(STICKY_WEB,actor)){
        event_handler->displayMsg(getActorBattlerName(actor)+" is slowed by sticky web!");
        // active_battler->changeSpeedModifier(-1);
        StatCV changes = {{5,1}};
        changeStats(actor, changes, false);
    }
    return tryEjectPack(actor);
}

bool Battle::applySwitchInAbilitiesEffects(Battler* user){
    Battler * user_active = user;
    Battler * target_active = getActorBattler(otherBattleActionActor(user->getActor()));
    std::string user_name = getActorBattlerName(user->getActor());
    std::string other_name = getActorBattlerName(otherBattleActionActor(user->getActor()));
    if(user_active->isFainted())
        return false;

    Ability user_ability = user_active->getAbility();
    
    if(user_ability== POWER_OF_ALCHEMY){
        //copy a traceable ability from a dead teammate
        MonsterTeam * user_team = getActorTeam(user->getActor());
        auto monsters = user_team->getMonsters();
        Ability final_choice = NO_ABILITY;
        for(Monster* mon: monsters){
            if(!mon->isFainted())
                continue;
            Ability mon_ability = mon->getAbility();
            if(!abilityCannotBeCopied(mon_ability)){
                final_choice = mon_ability;
                break;
            }
        }
        if(!user_active->hasHeldItem(ABILITY_SHIELD))
            user_active->setAbility(final_choice);
    }
    //TRACE may also activate other abilities once an ability is traced
    if(user_ability == TRACE){
        Ability target_ability = target_active->getAbility();
        if(isAbilityTraceable(target_ability) && !user_active->hasHeldItem(ABILITY_SHIELD)){
            user_active->setAbility(target_ability);
            event_handler->displayMsg(user_name+" traced "+other_name+"'s "+abilityToString(target_ability)+"!");
        }
    }

    user_ability = user_active->getAbility();
    switch(user_ability){
        case INTIMIDATE:{
            if(!target_active->isFainted() && !target_active->hasSubstitute() &&
                !doesAbilityIgnoreIntimidate(target_active->getAbility())){//inner focus, oblivious and scrappy give immunity to intimidate
                event_handler->displayMsg(user_name+" intimidates its opponent!");
                // bool res = target_active->changeAttackModifier(-1);
                // if(res && !target_active->hasAbility(CONTRARY)){
                //     tryEjectPack(otherBattleActionActor(actor));
                // }
                StatCV changes = {{0,-1}};
                changeStats(target_active->getActor(), changes, false);
                if(target_active->hasHeldItem(ADRENALINE_ORB)){
                    int modifier = target_active->getSpeedModifier();
                    if(!((modifier==MAX_MODIFIER && !target_active->hasAbility(CONTRARY))||
                        (modifier==MIN_MODIFIER && target_active->hasAbility(CONTRARY)))){
                        target_active->consumeHeldItem();
                        StatCV changes = {{5,1}};
                        changeStats(target_active->getActor(), changes, false);
                    }
                }
            }
            break;
        }
        case DROUGHT:{
            if(field->getWeather() != SUN && !field->weatherCannotChange() && !thereIsaCloudNine()){
                event_handler->displayMsg(user_name+"'s Drought made the sun shine harshly!");
                field->setWeather(SUN,user_active->hasHeldItem(HEAT_ROCK)?8:5);
            }
            break;
        }
        case DESOLATE_LAND:{
            if(field->getWeather() != EXTREME_SUN){
                event_handler->displayMsg(user_name+"'s Desolate Land made the sun shine harshly!");
                field->setWeather(EXTREME_SUN,-1);
            }
            break;
        }
        case PRIMORDIAL_SEA:{
            if(field->getWeather() != HEAVY_RAIN){
                event_handler->displayMsg(user_name+"'s Primordial Sea made it rain heavily!");
                field->setWeather(HEAVY_RAIN,-1);
            }
            break;
        }
        case DELTA_STREAM:{
            if(field->getWeather() != STRONG_WINDS){
                event_handler->displayMsg(user_name+"'s Delta Stream made the weather change!");
                field->setWeather(STRONG_WINDS,-1);
            }
            break;
        }
        
        case SAND_STREAM:{
            if(field->getWeather()!= SANDSTORM && !field->weatherCannotChange() && !thereIsaCloudNine()){
                event_handler->displayMsg(user_name+"'s Sand Stream started a sandstorm!");
                field->setWeather(SANDSTORM,user_active->hasHeldItem(SMOOTH_ROCK)?8:5);
            }
            break;
        }
        case DRIZZLE:{
            if(field->getWeather() != RAIN && !field->weatherCannotChange() && !thereIsaCloudNine()){
                event_handler->displayMsg(user_name+"'s Drizzle changed the weather to rain!");
                field->setWeather(RAIN,user_active->hasHeldItem(DAMP_ROCK)?8:5);
            }
            break;
        }
        case SNOW_WARNING:{
            if(field->getWeather() != SNOWSTORM && !field->weatherCannotChange() && !thereIsaCloudNine()){
                event_handler->displayMsg(user_name+"'s Snow Warning started a Snow storm!");
                field->setWeather(SNOWSTORM,user_active->hasHeldItem(ICY_ROCK)?8:5);
            }
            break;
        }
        case FOREWARN:{
            if(!target_active->isFainted()){
                auto target_attacks = target_active->getAttacks();
                Attack* most_powerful_attack = nullptr;
                unsigned int most_power = 0;
                for(auto it=target_attacks.begin();it!=target_attacks.end();it++){
                    if(it->first==0 || it->first == STRUGGLE_ID)
                        continue;
                    Attack * current_attack = Attack::getAttack(it->first);
                    if(current_attack==nullptr)
                        continue;
                    if(most_powerful_attack == nullptr || current_attack->getPower()>most_power){
                        most_power = current_attack->getPower();
                        most_powerful_attack = current_attack;
                    }
                } 
                if(most_powerful_attack!=nullptr)
                    event_handler->displayMsg(user_name+"'s Forewarn reveals "+other_name+"'s "+most_powerful_attack->getName()+"!");
            }
            break;
        }
        case ANTICIPATION:{
            if(!target_active->isFainted()){
                auto target_attacks = target_active->getAttacks();
                Attack* most_dangerous_attack = nullptr;
                for(auto it=target_attacks.begin();it!=target_attacks.end();it++){
                    if(it->first==0 || it->first == STRUGGLE_ID)
                        continue;
                    Attack * current_attack = Attack::getAttack(it->first);
                    unsigned int effect = current_attack->getEffectId();
                    if(current_attack==nullptr)
                        continue;
                    if(effect == 103 || effect==156){//OHKO
                        most_dangerous_attack = current_attack;
                    }else if(getTypeEffectiveness(current_attack->getType(), user_active->getTypes(),
                        user_active->isTouchingGround(), false, current_attack->getEffectId() == 196) > 1.1 &&
                        current_attack->getCategory()!=STATUS){//super effective moves
                        most_dangerous_attack = current_attack;
                    }
                    if(most_dangerous_attack != nullptr){
                        break;
                    }
                } 
                if(most_dangerous_attack!=nullptr)
                    event_handler->displayMsg(user_name+"'s Anticipation reveals "+other_name+"'s "+most_dangerous_attack->getName()+"!");
            }
            break;
        }
        case DOWNLOAD:{
            if(!target_active->isFainted()){
                unsigned int target_defense = target_active->getModifiedDefense();
                unsigned int target_special_defense = target_active->getModifiedSpecialDefense();
                event_handler->displayMsg(user_name+"'s Download activates!");
                StatCV changes;
                if(target_special_defense < target_defense){
                    // user_active->changeSpecialAttackModifier(1);
                    changes.push_back({3,1});
                }else{
                    // user_active->changeAttackModifier(1);
                    changes.push_back({1,1});
                }
                changeStats(user->getActor(), changes, false);
            }
            break;
        }
        case FRISK:{
            if(!target_active->isFainted() && target_active->hasHeldItem()){
                ItemType item_type = target_active->getMonster()->getHeldItem();
                ItemData * item = ItemData::getItemData(item_type);
                event_handler->displayMsg(user_name+"'s Frisk reveals "+other_name+"'s "+item->getName()+"!"); 
            }
            break;
        }
        case UNNERVE:{
            if(!target_active->isFainted()){
                event_handler->displayMsg(other_name+" is nervous and won't eat berries!");
                target_active->addVolatileCondition(UNNERVED,-1);
            }
            break;
        }
        case MISTY_SURGE:{
            if(field->getTerrain()!=MISTY_FIELD){
                event_handler->displayMsg(user_name+"'s Misty Surge activates!");
                field->setTerrain(MISTY_FIELD,user_active->hasHeldItem(TERRAIN_EXTENDER)?8:5);
                // consumeSeeds();
            }
            break;
        }
        case SCREEN_CLEANER:{
            // screen cleaner removes all screens active when holder switches in
            field->clearFieldEffect(REFLECT,PLAYER);
            field->clearFieldEffect(LIGHT_SCREEN,PLAYER);
            field->clearFieldEffect(MIST,PLAYER);
            field->clearFieldEffect(REFLECT,OPPONENT);
            field->clearFieldEffect(LIGHT_SCREEN,OPPONENT);
            field->clearFieldEffect(MIST,OPPONENT);
            break;
        }
        // case FORECAST:{
        //     //castform transforms
        //     user_active->onWeatherChange(field->getWeather());
        //     break;
        // }
        default:break;
    }
    // unnerve
    if(opponent_active->hasAbility(UNNERVE) && 
        !user_active->isFainted()){
        user_active->addVolatileCondition(UNNERVED,-1);
        event_handler->displayMsg(user_name+" is nervous and won't eat berries!");
    }
    // check for EJECT PACK
    if(tryEjectPack(user->getActor())){
        return true;
    }
    
    return false;
}

bool Battle::onTerrainChange(BattleActionActor actor){
    Battler * user = getActorBattler(actor);
    if(user->isFainted())
        return false;
    if(user->hasHeldItem(ELECTRIC_SEED) &&
        field->getTerrain() == ELECTRIC_FIELD &&
        user->isTouchingGround()){
        StatCV changes;
        if(user->getDefenseModifier()!=MAX_MODIFIER && !user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Electric Seed boosts its Def!");
            user->consumeHeldItem();
            changes.push_back({2,1});
        }else if(user->getDefenseModifier()!=MIN_MODIFIER && user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Electric Seed lowers its Def!");
            user->consumeHeldItem();
            changes.push_back({2,-1});
        }
        changeStats(user->getActor(), changes, false);
    }
    if(user->hasHeldItem(PSYCHIC_SEED) &&
        field->getTerrain() == PSYCHIC_FIELD &&
        user->isTouchingGround()){
        StatCV changes;
        if(user->getSpecialDefenseModifier()!=MAX_MODIFIER && !user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Psychic Seed boosts its Sp. Def!");
            user->consumeHeldItem();
            changes.push_back({4,1});
        }else if(user->getSpecialDefenseModifier()!=MIN_MODIFIER && user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Psychic Seed lowers its Sp. Def!");
            user->consumeHeldItem();
            changes.push_back({4,-1});
        }
        changeStats(user->getActor(), changes, false);
    }
    if(user->hasHeldItem(GRASSY_SEED) &&
        field->getTerrain() == GRASSY_FIELD &&
        user->isTouchingGround()){
        StatCV changes;
        if(user->getDefenseModifier()!=MAX_MODIFIER && !user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Grassy Seed boosts its Def!");
            user->consumeHeldItem();
            changes.push_back({2,1});
        }else if(user->getDefenseModifier()!=MIN_MODIFIER && user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Grassy Seed lowers its Def!");
            user->consumeHeldItem();
            changes.push_back({2,-1});
        }
        changeStats(user->getActor(), changes, false);
    }
    if(user->hasHeldItem(MISTY_SEED) &&
        field->getTerrain() == MISTY_FIELD &&
        user->isTouchingGround()){
        StatCV changes;
        if(user->getSpecialDefenseModifier()!=MAX_MODIFIER && !user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Misty Seed boosts its Sp. Def!");
            user->consumeHeldItem();
            changes.push_back({4,1});
        }else if(user->getSpecialDefenseModifier()!=MIN_MODIFIER && user->hasAbility(CONTRARY)){
            event_handler->displayMsg(user->getNickname()+"'s Misty Seed lowers its Sp. Def!");
            user->consumeHeldItem();
            changes.push_back({4,-1});
        }
        changeStats(user->getActor(), changes, false);
    }
    if(user->hasAbility(MIMICRY)){
        switch(field->getTerrain()){
            case ELECTRIC_FIELD:{
                user->clearTypes();
                user->addType(ELECTRIC);
                break;
            } 
            case PSYCHIC_FIELD:{
                user->clearTypes();
                user->addType(PSYCHIC);
                break;
            } 
            case MISTY_FIELD:{
                user->clearTypes();
                user->addType(FAIRY);
                break;
            } 
            case GRASSY_FIELD:{
                user->clearTypes();
                user->addType(GRASS);
                break;
            } 
            case NO_TERRAIN:{
                user->resetTypes();
                break;
            }
            default:break;
        }
    }
    return tryEjectPack(user->getActor());
}

void Battle::onTerrainChange(){
    std::vector<Battler*> battlers = getBattlersSortedBySpeed();
    for(Battler* user: battlers){
        onTerrainChange(user->getActor());
    }
}

void Battle::applySwitchInFormChange(BattleActionActor actor){
    Battler * user_active = getActorBattler(actor);
    std::string user_name = getActorBattlerName(actor);
    Battler * target_active = getActorBattler(otherBattleActionActor(actor));
    std::string target_name = getActorBattlerName(otherBattleActionActor(actor));
    if(user_active->isFainted() || target_active->isFainted())
        return;
    if(user_active->hasAbility(IMPOSTOR)){
        user_active->transformInto(target_active->getMonster());
        event_handler->displayMsg(user_name+" transformed into "+target_name+"!");
        // apply effects again since ability may have changed
        // applyImpostorSwitchIn(actor);
        // applySwitchInAbilitiesEffects(actor);
        return;
    }
    user_active->changeFormSwitchIn();
}

Battler* Battle::getActorBattler(BattleActionActor actor){
    if(actor == PLAYER){
        return player_active;
    }else{
        return opponent_active;
    }
}

std::string Battle::getActorBattlerName(BattleActionActor actor){
    if(actor == PLAYER){
        return "Player's "+player_active->getNickname();
    }else{
        if(is_wild_battle)
            return "Wild "+opponent_active->getNickname();
        else
            return "Opponent's "+opponent_active->getNickname();
    }
}

MonsterTeam* Battle::getActorTeam(BattleActionActor actor){
    if(actor == PLAYER){
        return player_team;
    }else{
        return opponent_team;
    }
}

bool Battle::applyContactEffects(Attack * attack, BattleActionActor actor, bool makes_contact){
    Battler * active_user = getActorBattler(actor);
    Battler * active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = active_user->getNickname();
    std::string opponent_mon_name = active_target->getNickname();
    if(!makes_contact){
        return false;
    }

    if(active_user->hasHeldItem(PUNCHING_GLOVE)){
        // punching glove prevents contact
        return false;
    }
    
    // POISON TOUCH ability effect
    if(active_user->hasAbility(POISON_TOUCH) && 
        active_target->canBePoisoned() &&
        RNG::getRandomInteger(0,2)==0){
        event_handler->displayMsg(user_mon_name+"'s Poison Touch triggers!");
        active_target->setPermanentStatus(POISONED);
        if(active_target->hasAbility(SYNCHRONIZE) && 
            !active_user->hasPermanentStatus()){
            event_handler->displayMsg(opponent_mon_name+"'s Synchronize triggers!");
            active_user->setPermanentStatus(POISONED);
        }
    }

    if(active_user->hasHeldItem(PROTECTIVE_PADS)){
        // protective pads prevent contact effects
        return false;
    }

    // rocky helmet
    if(active_target->hasHeldItem(ROCKY_HELMET)){
        event_handler->displayMsg(opponent_mon_name+"'s Rocky Helmet hurts "+user_mon_name+"!");
        unsigned int rocky_helmet_damage = max(active_user->getMaxHP() / 6,1);
        unsigned int actual_rocky_helmet_damage = active_user->addDirectDamage(rocky_helmet_damage);
        // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_rocky_helmet_damage)+" damage from "+opponent_mon_name+"'s Rocky Helmet!");
        event_handler->displayDmgDealt(actual_rocky_helmet_damage,active_user,opponent_mon_name+"'s Rocky Helmet");
    }
    if(active_user->isFainted())
        return false;

    // TANGLING HAIR decreses attacker speed
    

    // apply target ability effects
    switch(active_target->getAbility()){
        case TANGLING_HAIR:{
            StatCV changes = {{5,-1}};
            changeStats(actor, changes, false);
            break;
        }
        case STATIC:{
            if(active_user->canBeParalyzed() &&
                RNG::getRandomInteger(0,2)==0){
                event_handler->displayMsg(opponent_mon_name+"'s Static triggers!");
                active_user->setPermanentStatus(PARALYSIS);
                if(active_user->hasAbility(SYNCHRONIZE) && 
                    !active_target->hasPermanentStatus()){
                    event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
                    active_target->setPermanentStatus(PARALYSIS);
                }
            }
            break;
        }
        case POISON_POINT:{
            if(active_user->canBePoisoned() &&
                RNG::getRandomInteger(0,2)==0){
                event_handler->displayMsg(opponent_mon_name+"'s Poison Point triggers!");
                active_user->setPermanentStatus(POISONED);
                if(active_user->hasAbility(SYNCHRONIZE) && 
                    !active_target->hasPermanentStatus()){
                    event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
                    active_target->setPermanentStatus(POISONED);
                }
            }
            break;
        }
        case FLAME_BODY:{
            if(active_user->canBeBurned() &&
                RNG::getRandomInteger(0,2)==0){
                event_handler->displayMsg(opponent_mon_name+"'s Flame Body triggers!");
                active_user->setPermanentStatus(BURN);
                if(active_user->hasAbility(SYNCHRONIZE) && 
                    !active_target->hasPermanentStatus()){
                    event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
                    active_target->setPermanentStatus(BURN);
                }
            }
            break;
        }
        case EFFECT_SPORE:{
            if(!active_user->hasType(GRASS) &&
                !active_user->hasAbility(OVERCOAT) &&
                !active_user->hasHeldItem(SAFETY_GOGGLES)){
                unsigned int random_number = RNG::getRandomInteger(1,100);
                if(random_number<10){//poison
                    if(active_user->canBePoisoned()){
                        event_handler->displayMsg(opponent_mon_name+"'s Effect Spore triggers!");
                        active_user->setPermanentStatus(POISONED);
                        if(active_user->hasAbility(SYNCHRONIZE) && 
                            !active_target->hasPermanentStatus()){
                            event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
                            active_target->setPermanentStatus(POISONED);
                        }
                    }
                }else if(random_number<20){//paralysis
                    if(active_user->canBeParalyzed()){
                        event_handler->displayMsg(opponent_mon_name+"'s Effect Spore triggers!");
                        active_user->setPermanentStatus(PARALYSIS);
                        if(active_user->hasAbility(SYNCHRONIZE) && 
                            !active_target->hasPermanentStatus()){
                            event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
                            active_target->setPermanentStatus(PARALYSIS);
                        }
                    }
                }else if(random_number<=30){//sleep
                    if(active_user->canFallAsleep() && !isUproar()){
                        event_handler->displayMsg(opponent_mon_name+"'s Effect Spore triggers!");
                        unsigned int random_number = RNG::getRandomInteger(1,3);
                        if(random_number == 1)
                            active_user->setPermanentStatus(SLEEP_3);
                        else if(random_number == 2)
                            active_user->setPermanentStatus(SLEEP_2);
                        else
                            active_user->setPermanentStatus(SLEEP_4);
                    }
                }
            }
            break;
        }
        case CURSED_BODY:{
            if(RNG::getRandomInteger(1,100)<=30){
                unsigned int last_attack_used_id =active_user->getLastAttackUsed();
                Attack * last_attack_used = Attack::getAttack(last_attack_used_id);
                if(last_attack_used_id!=0 && 
                    last_attack_used_id!=STRUGGLE_ID && 
                    !active_user->hasDiabledAttack()){
                    event_handler->displayMsg(user_mon_name+"'s "+last_attack_used->getName()+" was disabled by "+opponent_mon_name+"'s Cursed Body!");
                    active_user->disableAttack(last_attack_used_id,5);
                }
            }
            break;
        }
        case PICKPOCKET:{
            if(!active_target->isFainted() &&
                active_user->hasHeldItem() && 
                active_user->canStealItem() &&
                !active_target->hasHeldItem()){
                ItemType stolen_item = active_user->removeHeldItem();
                active_target->setHeldItem(stolen_item);
                event_handler->displayMsg(opponent_mon_name+" steals "+user_mon_name+"'s "+ItemData::getItemData(stolen_item)->getName()+"!");
            }
            break;
        }
        case CUTE_CHARM:{
            Gender active_user_gender = active_user->getGender();
            Gender active_target_gender = active_target->getGender();
            bool can_attract = areMaleAndFemale(active_user_gender,active_target_gender) && !active_user->hasAbility(OBLIVIOUS);
            if(active_target->hasAbility(CUTE_CHARM) && can_attract
                && RNG::getRandomInteger(0,2)==0){
                active_user->addVolatileCondition(INFATUATION, -1);
                if(active_user->hasHeldItem(DESTINY_KNOT))
                    active_target->addVolatileCondition(INFATUATION, -1);
            }
            break;
        }
        case ROUGH_SKIN:{
            if(active_user->isFainted())
                return false;
            event_handler->displayMsg(opponent_mon_name+"'s Rough Skin hurts "+user_mon_name+"!");
            unsigned int rough_skin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_rough_skin_damage = active_user->addDirectDamage(rough_skin_damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_rough_skin_damage)+" damage from "+opponent_mon_name+"'s Rough Skin!");
            event_handler->displayDmgDealt(actual_rough_skin_damage,active_user,opponent_mon_name+"'s Rough Skin");
            break;
        }
        case IRON_BARBS:{
            if(active_user->isFainted())
                return false;
            event_handler->displayMsg(opponent_mon_name+"'s Iron Barbs hurt "+user_mon_name+"!");
            unsigned int rough_skin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_rough_skin_damage = active_user->addDirectDamage(rough_skin_damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_rough_skin_damage)+" damage from "+opponent_mon_name+"'s Rough Skin!");
            event_handler->displayDmgDealt(actual_rough_skin_damage,active_user,opponent_mon_name+"'s Iron Barbs");
            break;
        }
        case MUMMY:{
            if(abilityCannotBeChanged(active_user->getAbility())){
                break;
            }
            active_user->setAbility(MUMMY);
            event_handler->displayMsg(user_mon_name+"'s ability was changed to Mummy!");
            break;
        }
        case WANDERING_SPIRIT:{
            Ability user_ability = active_user->getAbility();
            if(abilityCannotBeChanged(user_ability) ||
                abilityCannotBeCopied(user_ability) ||
                user_ability==WANDERING_SPIRIT){
                break;
            }
            active_user->setAbility(WANDERING_SPIRIT);
            active_target->setAbility(user_ability);
            event_handler->displayMsg(user_mon_name+"'s ability was changed to "+abilityToString(WANDERING_SPIRIT)+"!");
            event_handler->displayMsg(opponent_mon_name+"'s ability was changed to "+abilityToString(user_ability)+"!");
            applySwitchInAbilitiesEffects(active_target);
            break;
        }
        default: break;
    }
    
    return false;
}

bool Battle::checkIfAttackFails(Attack* attack, 
    BattleAction action,
    BattleAction other_action){
    unsigned int attack_id = attack->getId();
    BattleActionActor actor = action.getActor();
    Battler * active_user = getActorBattler(actor);
    Battler * active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));

    Type attack_type = attack->getType(active_user,field);
    // check if attack is disabled
    if(active_user->isAttackDisabled(attack_id)){
        event_handler->displayMsg(user_mon_name+"'s "+attack->getName()+" is disabled!");
        // active_user->setLastAttackUsed(action.getAttackId());
        // last_attack_used_id = attack_id;
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }

    //check if attack fails due to taunt (or having assault vest)
    if((active_user->hasVolatileCondition(TAUNTED) || active_user->hasHeldItem(ASSULT_VEST)) && 
        attack->getCategory() == STATUS){
        if(active_user->hasHeldItem(ASSULT_VEST))
            event_handler->displayMsg(user_mon_name+"' Assault Vest prevents the use of Status moves!");
        else if(active_user->hasVolatileCondition(TAUNTED))
            event_handler->displayMsg(user_mon_name+" is taunted and cannot use "+attack->getName()+"!");
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }

    //check if attack fails due to torment
    if(active_user->hasVolatileCondition(TORMENTED) && 
        attack->getId() == active_user->getLastAttackUsed()){
        event_handler->displayMsg(user_mon_name+" is tormented and cannot use "+attack->getName()+"!");
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }

    //check if attack fails due to throat chop
    if(active_user->hasVolatileCondition(THROAT_CHOPPED) && 
        attack->isSoundBased()){
        event_handler->displayMsg(user_mon_name+" is cannot use "+attack->getName()+" sound moves!");
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }

    // check if gravity blocks attack
    if(field->hasFullFieldEffect(GRAVITY) && attack_type==FLYING){
        event_handler->displayMsg(user_mon_name+" cannot use "+attack->getName()+" due to gravity!");
        // active_user->setLastAttackUsed(action.getAttackId());
        // last_attack_used_id = attack_id;
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->setLastAttackFailed();
        return true;
    }
    
    // check if target is alive
    AttackTarget target = attack->getTarget();
    if(target == TARGET_OPPONENT){
        if(active_target->isFainted()){
            event_handler->displayMsg("But it failed!");
            decrementVolatiles(active_user);
            active_user->setLastAttackFailed();
            active_user->removeVolatileCondition(LASER_FOCUS);
            if(attack->getEffectId()==100 || attack->getEffectId()==275){
                //user dies
                active_user->addDirectDamage(active_user->getMaxHP());
            }
            if(attack->getEffectId()==187){
                //user dies
                active_user->addVolatileCondition(RECHARGING,-1);
            }
            return true;
        }
    }

    bool attack_failed = false;
    // check if attack fails due to dark types being immune to prankster boosts
    if(active_target->hasType(DARK) && 
        attack->getCategory() == STATUS &&
        attack->getTarget() == TARGET_OPPONENT &&
        attack->getPriorityLevel() < action.getPriority()){
        attack_failed = true;
    }
    // check if move fails due to imprison
    if((active_user->hasVolatileCondition(IMPRISON))&&
        active_target->knowsAttack(action.getAttackId())){ 
        attack_failed = true;
    }
    // check if OHKO fails due to sturdy
    if((attack->getEffectId() == 103 || attack->getEffectId() == 156) && active_target->hasAbility(STURDY)){
        event_handler->displayMsg(opponent_mon_name+" is protected by Sturdy!");
        active_user->setLastAttackUsed(action.getAttackId());
        last_attack_used_id = attack_id;
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }
    switch(attack->getEffectId()){
        case 168:{// check if attack fails due to not having taken any Special damage
            if(active_user->getSpecialDamageTakenThisTurn() == 0){
                attack_failed = true;
            }
            break;
        }
        case 175:{// check if attack fails due to not having taken any Physical damage
            if(active_user->getPhysicalDamageTakenThisTurn() == 0){
                attack_failed = true;
            }
            break;
        }
        case 206:{
            // check if attack fails due to not having taken any damage
            if((active_user->getPhysicalDamageTakenThisTurn() + 
                active_user->getSpecialDamageTakenThisTurn()) == 0){
                attack_failed = true;
            }
            break;
        }
        case 43: {//endeavor
            unsigned int current_hp_user = active_user->getCurrentHP();
            unsigned int current_hp_target = active_target->getCurrentHP();
            if(current_hp_user >= current_hp_target){
                attack_failed = true;
            }
            break;
        }
        case 195:{//snore
            if(!active_user->isAsleep()){
                attack_failed = true;
            }
            break;
        }
        case 48:{//fly
            //fails if user is smacked down
            if(active_user->hasVolatileCondition(SMACKED_DOWN)){
                attack_failed = true;
            }
            break;
        }
        case 49:{//sucker punch
            if(!isAttackingActionType(other_action.getActionType()) || // opponent is not attacking
                action.getOrder() > other_action.getOrder() || // opponent action comes before
                Attack::getAttack(other_action.getAttackId())->getCategory()==STATUS){ // opponent is using status move
                attack_failed = true;
            }
            break;
        }
        case 279:{
            // upper hand fails if the target has not chosen a non status attack with priority between 1 and 3
            // or the opponent act faster
            if(!isAttackingActionType(other_action.getActionType()) || // opponent is not attacking
                action.getOrder() > other_action.getOrder() || // opponent action comes before
                Attack::getAttack(other_action.getAttackId())->getCategory()==STATUS ||// opponent is using status move
                other_action.getPriority() > 3 ||// opponent is using attack with too high priority
                other_action.getPriority() < 1){ //opponent is using attack with too low priority
                attack_failed = true;
            }
        }
        case 105:{//fake out
            if(!active_user->isFirstTurn()){
                active_user->setLastAttackUsed(action.getAttackId());
                last_attack_used_id = attack_id;
                attack_failed = true;
            }
            break;
        }
        case 130:{
            //schedule future sight
            // Attack* future_sight = Attack::getAttack(FUTURE_SIGHT_ID);
            unsigned int attack_stat = attack->getCategory()==PHYSICAL ?
                active_user->getModifiedAttack() : active_user->getModifiedSpecialAttack();
            scheduled_futuresights.push_back(
                ScheduledFutureSight(
                    attack->getId(),
                    3,
                    attack_stat,
                    active_user->getLevel(),
                    otherBattleActionActor(actor),
                    active_user->hasType(attack->getType())
                ));
            if(attack->getId()==FUTURE_SIGHT_ID)
                event_handler->displayMsg(user_mon_name+" has foreseen an attack!");
            else if(attack->getId()==DOOM_DESIRE_ID)
                event_handler->displayMsg(user_mon_name+" has chosen Doom Desire as its destiny!");
            return true;
        }
        case 162:{//dream eater
            if(!active_target->isAsleep()){
                active_user->setLastAttackUsed(action.getAttackId());
                last_attack_used_id = attack_id;
                attack_failed = true;
            }
            break;
        }
        case 176:{//focus punch
            if(!active_user->hasVolatileCondition(FOCUSED)){
                active_user->setLastAttackUsed(action.getAttackId());
                last_attack_used_id = attack_id;
                attack_failed = true;
            }
            break;
        }
        case 178:{//last resort
            auto usable_attacks = active_user->getAttacks();
            bool has_not_used_attack = false;
            for(auto usable_attack: usable_attacks){
                int att_id = usable_attack.first;   
                if(att_id == 0 || att_id==STRUGGLE_ID)
                    continue;
                Attack* other_attack = Attack::getAttack(att_id);
                if(other_attack->getEffectId() == 178)
                    continue;
                if(active_user->hasUsedAttack(att_id)){
                    continue;
                }
                has_not_used_attack = true;
                break;
            }
            if(has_not_used_attack){
                active_user->setLastAttackUsed(action.getAttackId());
                attack_failed = true;
            }
            break;
        }
        case 200:{
            //fails if user has not eaten a berry before
            if(!active_user->hasConsumedBerry()){
                attack_failed = true;
            }
            break;
        }
        case 204:{
            //fling fails if the user has no held item
            if(!active_user->canStealItem() || !active_user->hasHeldItem()){
                attack_failed = true;
            }
            break;
        }
        case 208:{
            //fails if there is no hail or snow
            if((field->getWeather() != HAIL && field->getWeather() != SNOWSTORM) || thereIsaCloudNine()){
                attack_failed = true;
            }
            break;
        }
        case 280:{
            //fails if the target does not hold any item
            if(!active_target->hasHeldItem()){
                attack_failed = true;
            }
            break;
        }
        default:break;
    }
    if(field->getWeather()==HEAVY_RAIN && 
        attack->getType() == FIRE &&
        attack->getCategory() != STATUS &&
        !thereIsaCloudNine()){
        attack_failed = true;
    }
    if(field->getWeather()==EXTREME_SUN && 
        attack->getType() == WATER &&
        attack->getCategory() != STATUS &&
        !thereIsaCloudNine()){
        attack_failed = true;
    }
    if(attack_failed){
        event_handler->displayMsg("But it failed!");
        active_user->setLastAttackUsed(action.getAttackId());
        last_attack_used_id = attack_id;
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }


    //charge solar beam
    if(attack->getEffectId() == 9 && !active_user->hasVolatileCondition(CHARGING_SOLARBEAM)){// Solar beam
        active_user->addVolatileCondition(CHARGING_SOLARBEAM, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Solar Beam immediately!");
            active_user->consumeHeldItem();
        }else if((field->getWeather() != SUN && field->getWeather()!=EXTREME_SUN) 
            || thereIsaCloudNine() || active_user->hasHeldItem(UTILITY_UMBRELLA)){
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_SOLARBEAM)){
        active_user->removeVolatileCondition(CHARGING_SOLARBEAM);
    }
    //charge meteor mash
    if(attack->getEffectId() == 9 && !active_user->hasVolatileCondition(CHARGING_METEORBEAM)){// Solar beam
        active_user->addVolatileCondition(CHARGING_METEORBEAM, 5);
        // boost user Sp. Atk
        StatCV changes = {{3,1}};
        changeStats(actor, changes, false);
        if(tryEjectPack(actor))
            return true;
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Meteor Beam immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_METEORBEAM)){
        active_user->removeVolatileCondition(CHARGING_METEORBEAM);
    }
    //charge solar blade
    if(attack->getEffectId() == 263 && !active_user->hasVolatileCondition(CHARGING_SOLARBLADE)){// Solar blade
        active_user->addVolatileCondition(CHARGING_SOLARBLADE, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Solar Blade immediately!");
            active_user->consumeHeldItem();
        }else if((field->getWeather() != SUN && field->getWeather()!=EXTREME_SUN) || thereIsaCloudNine() || active_user->hasHeldItem(UTILITY_UMBRELLA)){
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_SOLARBLADE)){
        active_user->removeVolatileCondition(CHARGING_SOLARBLADE);
    }
    //charge sky attack
    if(attack->getEffectId() == 197 && !active_user->hasVolatileCondition(CHARGING_SKYATTACK)){// Solar beam
        active_user->addVolatileCondition(CHARGING_SKYATTACK, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Sky Attack immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_SKYATTACK)){
        active_user->removeVolatileCondition(CHARGING_SKYATTACK);
    }
    //charge fly
    if(attack->getEffectId() == 48 && !active_user->hasVolatileCondition(FLYING_HIGH)// Fly
        && !field->hasFullFieldEffect(GRAVITY)){// gravity will cause the attack to fail since fly is flying type
        active_user->addVolatileCondition(FLYING_HIGH, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Fly immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(FLYING_HIGH)){
        active_user->removeVolatileCondition(FLYING_HIGH);
    }
    //charge bounce
    if(attack->getEffectId() == 226 && !active_user->hasVolatileCondition(BOUNCING)// Fly
        && !field->hasFullFieldEffect(GRAVITY)){// gravity will cause the attack to fail since fly is flying type
        active_user->addVolatileCondition(BOUNCING, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Bounce immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(BOUNCING)){
        active_user->removeVolatileCondition(BOUNCING);
    }
    //charge dig
    if(attack->getEffectId() == 70 && !active_user->hasVolatileCondition(UNDERGROUND)){// Dig
        active_user->addVolatileCondition(UNDERGROUND, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Dig immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(UNDERGROUND)){
        active_user->removeVolatileCondition(UNDERGROUND);
    }
    //charge phantom force
    if(attack->getEffectId() == 264 && !active_user->hasVolatileCondition(VANISHED)){// Phantom force
        active_user->addVolatileCondition(VANISHED, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Phantom Force immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(VANISHED)){
        active_user->removeVolatileCondition(VANISHED);
    }
    //charge shadow force
    if(attack->getEffectId() == 288 && !active_user->hasVolatileCondition(VANISHED_2)){// Shadow force
        active_user->addVolatileCondition(VANISHED_2, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Shadow Force immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(VANISHED_2)){
        active_user->removeVolatileCondition(VANISHED_2);
    }
    //charge ice burn
    if(attack->getEffectId() == 308 && !active_user->hasVolatileCondition(CHARGING_ICEBURN)){// Ice Burn
        active_user->addVolatileCondition(CHARGING_ICEBURN, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Ice Burn immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_ICEBURN)){
        active_user->removeVolatileCondition(CHARGING_ICEBURN);
    }
    //charge ice burn
    if(attack->getEffectId() == 309 && !active_user->hasVolatileCondition(CHARGING_FREEZESHOCK)){// Freeze Shock
        active_user->addVolatileCondition(CHARGING_FREEZESHOCK, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Freeze Shock immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_FREEZESHOCK)){
        active_user->removeVolatileCondition(CHARGING_FREEZESHOCK);
    }
    //charge dive
    if(attack->getEffectId() == 136 && !active_user->hasVolatileCondition(UNDERWATER)){// Dive
        active_user->addVolatileCondition(UNDERWATER, 5);
        if(active_user->hasHeldItem(POWER_HERB)){
            event_handler->displayMsg(user_mon_name+"'s Power Herb allows "+user_mon_name+" to use Dive immediately!");
            active_user->consumeHeldItem();
        }else{
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(UNDERWATER)){
        active_user->removeVolatileCondition(UNDERWATER);
    }
    // moves that lock user in using them for a while
    if(attack->getEffectId() == 10 && !active_user->hasVolatileCondition(PETAL_DANCING)){
        active_user->addVolatileCondition(PETAL_DANCING, RNG::getRandomInteger(2,3));
    }
    if(attack->getEffectId() == 209 && !active_user->hasVolatileCondition(RAGING_FURING)){
        active_user->addVolatileCondition(RAGING_FURING, RNG::getRandomInteger(2,3));
    }
    if(attack->getEffectId() == 151 && !active_user->hasVolatileCondition(UPROARING)){
        active_user->addVolatileCondition(UPROARING, 3);
    }
    if(attack->getEffectId() == 12 && !active_user->hasVolatileCondition(OUTRAGING)){
        active_user->addVolatileCondition(OUTRAGING, RNG::getRandomInteger(2,3));
    }
    if(attack->getEffectId() == 114 && !active_user->hasVolatileCondition(THRASHING)){
        active_user->addVolatileCondition(THRASHING, RNG::getRandomInteger(2,3));
    }
    if(attack->getEffectId() == 68 && !active_user->hasVolatileCondition(ROLLINGOUT)){
        // reset consecutive counter
        active_user->setLastAttackUsed(0);
        active_target->setLastAttackUsed(ROLLOUT_ID);
        active_user->addVolatileCondition(ROLLINGOUT, 5);
    }

    // moves that use the power from stockpile
    if((attack->getEffectId() == 56 || attack->getEffectId() == 57) && active_user->getStockpiles()==0){//moves that use the power from stockpile
        event_handler->displayMsg("But it failed!");
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->setLastAttackFailed();
        return true;
    }
    // protect
    if(attack->getEffectId()==65 || attack->getEffectId()==264 || attack->getEffectId()==288){
        active_target->removeVolatileCondition(PROTECT);
        active_target->removeVolatileCondition(SPIKY_PROTECT);
        active_target->removeVolatileCondition(QUICK_GUARD);
        active_target->removeVolatileCondition(CRAFTY_SHIELD);
    }
    if((active_target->hasVolatileCondition(PROTECT) || 
        active_target->hasVolatileCondition(SPIKY_PROTECT))&& 
        attack->getTarget()==TARGET_OPPONENT &&
        attack->getEffectId()!=239){// SOME ATTACKS GO THROUGH PROTECT
        event_handler->displayMsg(opponent_mon_name+" protected itself!");
        // active_target->removeVolatileCondition(PROTECT);
        if(active_target->hasVolatileCondition(SPIKY_PROTECT) && 
            !active_user->hasAbility(WONDER_GUARD) &&
            attack->makesContact()){
            event_handler->displayMsg(opponent_mon_name+"'s Spiky Shield hurt "+user_mon_name+"!");
            unsigned int spiky_protect_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_spiky_protect_damage = active_user->addDirectDamage(spiky_protect_damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_spiky_protect_damage)+" damage from "+opponent_mon_name+"'s Spiky Shield!");
            event_handler->displayDmgDealt(actual_spiky_protect_damage,active_user,opponent_mon_name+"'s Spiky Shield");
        }
        decrementVolatiles(active_user);
        active_user->setLastAttackHit();
        if(attack->getEffectId()==100 || attack->getEffectId()==275){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            //user dies
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        if((attack->getEffectId() == 171 || attack->getEffectId() == 174 || attack->getEffectId() == 277)&&!active_user->hasAbility(MAGIC_GUARD)){
            // user takes 50% maxHP recoil damage
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int damage = max_hp / 2;
            unsigned int actual_damage = active_user->addDirectDamage(damage);
            // event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage from recoil!"); 
            event_handler->displayDmgDealt(actual_damage,active_user,"recoil");
        }
        return true;
    }
    if(active_target->hasVolatileCondition(CRAFTY_SHIELD) && 
        attack->getTarget()==TARGET_OPPONENT &&
        attack->getCategory() == STATUS){
        event_handler->displayMsg(opponent_mon_name+" protected itself!");
        decrementVolatiles(active_user);
        active_user->setLastAttackHit();
        if(attack->getEffectId()==100 || attack->getEffectId()==275){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            //user dies
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        return true;
    }
    
    // quick guard
    if(active_target->hasVolatileCondition(QUICK_GUARD) && 
        attack->getTarget()==TARGET_OPPONENT && 
        attack->getPriorityLevel()>0){
        event_handler->displayMsg(opponent_mon_name+" protected itself!");
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(ROLLINGOUT);
        active_user->setLastAttackHit();
        if(attack->getEffectId()==100 || attack->getEffectId()==275){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            //user dies
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        return true;
    }
    // psychic terrain prevents priority moves from hitting
    if(field->getTerrain() == PSYCHIC_FIELD && 
        attack->getTarget()==TARGET_OPPONENT && 
        attack->getPriorityLevel()>0){
        event_handler->displayMsg("But it failed!");
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(ROLLINGOUT);
        active_user->setLastAttackFailed();
        return true;
    }

    // damp prevents explosion and self-destruct
    if(active_target->hasAbility(DAMP) && 
        (attack->getEffectId() == 100 || attack->getEffectId()==275)&&
        attack->getTarget()==TARGET_OPPONENT){
        event_handler->displayMsg(opponent_mon_name+" prevented "+user_mon_name+"'s self-destructing move!");
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->setLastAttackHit();
        return true;
    }
    bool attack_absorbed = false;
    // lightning rod -> drew in electric type attack and increase SPATT
    if(active_target->hasAbility(LIGHTNING_ROD) && 
        attack_type==ELECTRIC &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        // active_target->changeSpecialAttackModifier(1);
        StatCV changes = {{3,1}};
        changeStats(active_target->getActor(), changes, false);
        attack_absorbed = true;
    }
    // motor drive -> drew in electric type attack and increase SPEED
    if(active_target->hasAbility(MOTOR_DRIVE) && 
        attack_type==ELECTRIC &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" absorbed the attack!");
        // active_target->changeSpecialAttackModifier(1);
        StatCV changes = {{5,1}};
        changeStats(active_target->getActor(), changes, false);
        attack_absorbed = true;
    }
    // storm drain -> drew in water type attack and increase SPATT
    if(active_target->hasAbility(STORM_DRAIN) && 
        attack_type==WATER &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        StatCV changes = {{3,1}};
        changeStats(active_target->getActor(), changes, false);
        attack_absorbed = true;
    }
    // wind rider -> wind based attack does nothing and increase speed
    if(active_target->hasAbility(LIGHTNING_ROD) && 
        attack->isWind() &&
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
        StatCV changes = {{5,1}};
        changeStats(active_target->getActor(), changes, false);
        attack_absorbed = true;
    }
    // sap sipper -> drew in grass type attack and increase ATT
    if(active_target->hasAbility(SAP_SIPPER) && 
        attack_type==GRASS &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        // active_target->changeAttackModifier(1);
        StatCV changes = {{1,1}};
        changeStats(active_target->getActor(), changes, false);
        attack_absorbed = true;
    }
    // flash fire -> drew in fire type attack and get flash fired volatile
    if(active_target->hasAbility(FLASH_FIRE) && 
        attack_type==FIRE &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        active_target->addVolatileCondition(FLASH_FIRED, -1);
        attack_absorbed = true;
    }
    // dry skin and water absorb prevent water type moves from hitting and restores 25% of max HP
    unsigned int heal_amount = max((active_target->getMaxHP() + 3)/ 4,1);
    unsigned int actual_heal_amount = 0;
    if((active_target->hasAbility(DRY_SKIN) ||
        active_target->hasAbility(WATER_ABSORB)) && 
        attack_type==WATER &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" absorbed the water attack!");
        actual_heal_amount = active_target->removeDamage(heal_amount);
        event_handler->displayMsg(opponent_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
        attack_absorbed = true;
    }
    // volt absorb prevents electric type moves from hitting and restores 25% of max HP
    if(active_target->hasAbility(VOLT_ABSORB) && 
        attack_type==ELECTRIC &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT &&
        !active_target->hasSubstitute()){
        event_handler->displayMsg(opponent_mon_name+" absorbed the electric attack!");
        actual_heal_amount = active_target->removeDamage(heal_amount);
        attack_absorbed = true;
    }
    if(actual_heal_amount>0)
        event_handler->displayMsg(opponent_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");

    if(attack_absorbed){
        decrementVolatiles(active_user);
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->setLastAttackHit();
        if(attack->getEffectId()==100 || attack->getEffectId()==275){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            //user recharges next turn
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        return true;
    }

    return false;
}

bool Battle::thereIsaCloudNine(){
    if(player_active->hasAbility(CLOUD_NINE) && !player_active->isFainted()){
        return true;
    }
    if(opponent_active->hasAbility(CLOUD_NINE) && !opponent_active->isFainted()){
        return true;
    }
    if(player_active->hasAbility(AIR_LOCK) && !player_active->isFainted()){
        return true;
    }
    if(opponent_active->hasAbility(AIR_LOCK) && !opponent_active->isFainted()){
        return true;
    }
    return false;
}

void Battle::removeVolatilesFromOpponentOfMonsterLeavingField(BattleActionActor actor_switching_out){
    Battler* opponent_active_monster = getActorBattler(otherBattleActionActor(actor_switching_out));
    opponent_active_monster->removeVolatileCondition(INFATUATION);
    opponent_active_monster->removeVolatileCondition(WRAP);
    opponent_active_monster->removeVolatileCondition(BIND);
    opponent_active_monster->removeVolatileCondition(SNAP_TRAP);
    opponent_active_monster->removeVolatileCondition(IMPRISON);
    opponent_active_monster->removeVolatileCondition(UNNERVED);
}

void Battle::forceSwitch(BattleActionActor actor_switching_out){
    Battler* old_active = getActorBattler(actor_switching_out);
    Battler* new_active;
    unsigned int old_active_maxHP = old_active->getMaxHP();
    field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,actor_switching_out);
    if(old_active->hasAbility(REGENERATOR) && !old_active->isFainted()){
        //regenerate 1/3 of user HP
        old_active->removeDamage(max((old_active_maxHP+2) / 3,1));
    }
    if(old_active->hasAbility(NATURAL_CURE) && !old_active->isFainted()){
        //remove status
        old_active->clearPermanentStatus();
    }
    if(actor_switching_out == PLAYER){
        unsigned int new_active_index = event_handler->chooseSwitchForced(player_team);
        player_team->swapActiveMonster(new_active_index);
        delete player_active;
        player_active = new Battler(player_team->getActiveMonster(),player_team,field,PLAYER,event_handler);
        new_active = player_active;
        event_handler->displayMsg("Player switched in "+player_active->getNickname());
    }else{
        unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
        opponent_team->swapActiveMonster(new_active_index);
        delete opponent_active;
        opponent_active = new Battler(opponent_team->getActiveMonster(),opponent_team,field,OPPONENT,event_handler);
        new_active = opponent_active;
        event_handler->displayMsg("Opponent switched in "+opponent_active->getNickname());
    }
    checkMonsterLeavingAbilities(actor_switching_out);
    resetOpponents();
    player_active->addSeenOpponent(opponent_active->getMonster());
    removeVolatilesFromOpponentOfMonsterLeavingField(actor_switching_out);
    applySwitchInFormChange(actor_switching_out);
    if(onTerrainChange(actor_switching_out))
        return;
    if(applySwitchInAbilitiesEffects(new_active))
        return;
    if(applySwitchInItemsEffects(actor_switching_out))
        return;
    if(performEntryHazardCheck(actor_switching_out))
        return;
    checkUproars();
    checkForExp();
}

void Battle::applyScheduledFutureSights(){
    std::vector<unsigned int> to_remove;
    for(unsigned int i = 0; i < scheduled_futuresights.size(); i++){
        ScheduledFutureSight& it = scheduled_futuresights[i];
        it.turns_left--;
        if(it.turns_left == 0){
            to_remove.push_back(i);
            unsigned int attack_id = it.attack_id;
            Battler* target_active = getActorBattler(it.target);
            std::string target_name = getActorBattlerName(it.target);
            if(attack_id==FUTURE_SIGHT_ID)
                event_handler->displayMsg("The foreseen attack hits "+target_name+"!");
            else if(attack_id==DOOM_DESIRE_ID)
                event_handler->displayMsg("The chosen Doom Desire fulfills on "+target_name+"!");
            if(target_active->isFainted()){
                event_handler->displayMsg("But it failed!");
                continue;
            }
            Attack* attack = Attack::getAttack(attack_id);
            float effectiveness = 1;
            for(auto type:target_active->getTypes()){
                float curr_effectiveness = getTypeEffectiveness(attack->getType(), 
                    type, 
                    target_active->isTouchingGround(),
                    false,
                    false);
                if(curr_effectiveness==0 && target_active->hasHeldItem(RING_TARGET))
                    curr_effectiveness = 1;
                effectiveness *= curr_effectiveness;
            }
            if(effectiveness == 0){
                event_handler->displayMsg("But it failed!");
                continue;
            }
            
            unsigned int base_dmg = baseDamage(it.user_level,
                attack->getPower(),
                it.user_special_attack,
                target_active->getModifiedSpecialDefense());
            bool critical_hit = RNG::getRandomInteger(0,23) == 0;
            if(target_active->hasAbility(SHELL_ARMOR)||
                target_active->hasAbility(BATTLE_ARMOR)){
                // shell armor and battle armor prevent critical hits
                critical_hit = false;
            }
            float crit_multiplier = 1;
            if(critical_hit){
                event_handler->displayMsg("Its a critical Hit!");
                crit_multiplier = 1.5;
            }
            float stab_multiplier = 1;
            if(it.stab){
                stab_multiplier = 1.5;
            }
            if(!target_active->hasSubstitute() && target_active->tryEatSuperEffectiveBerry(attack->getType(),effectiveness>1.1))
                base_dmg /= 2;
            unsigned int final_damage = max(base_dmg * effectiveness * crit_multiplier * stab_multiplier,1);
            auto actual_damage = target_active->addDamage(final_damage, attack->getCategory(), effectiveness, false);
            if(actual_damage.first > 0 && effectiveness > 1.1){
                event_handler->displayMsg(target_name+" was hit super effectively!");
            }
            if(actual_damage.first > 0 && effectiveness < 0.9){
                event_handler->displayMsg(target_name+" was hit not very effectively!");
            }
            if(!actual_damage.second){
                // event_handler->displayMsg(target_name+" took "+std::to_string(actual_damage.first)+" damage!");
                event_handler->displayDmgDealt(actual_damage.first,target_active);
            }
        }

    }
    // clear the scheduled future sights that were executed this turn from the end
    for(int i = to_remove.size()-1; i >= 0; i--){
        scheduled_futuresights.erase(scheduled_futuresights.begin()+to_remove[i]);
    }
}

void Battle::applyAbilityPostDamage(BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    std::string user_name = getActorBattlerName(actor);
    if(active_user->isFainted())
        return;
    switch(active_user->getAbility()){
        case SPEED_BOOST:{
            // user gets +1 speed
            event_handler->displayMsg(user_name+"' Speed Boost activates!");
            // active_user->changeSpeedModifier(1);
            StatCV changes = {{5,1}};
            changeStats(actor,changes,false);
            break;
        }
        case MOODY:{
            //user gets +2 in a random stat and -1 in another
            event_handler->displayMsg(user_name+"'s Moody activates!");
            unsigned int random_stat_high = RNG::getRandomInteger(0,6);
            unsigned int random_stat_low =random_stat_high;
            int count = 0;
            while(random_stat_high == random_stat_low){
                random_stat_low = RNG::getRandomInteger(0,6);
                if(++count>20){//avoid infinite looping
                    random_stat_low = (random_stat_high + 1)%7;
                    break;
                }
            }
            StatCV changes = {{random_stat_high,2},{random_stat_low,-1}};
            changeStats(actor,changes,true);
            break;
        }
        case BAD_DREAMS:{
            Battler* active_target = getActorBattler(otherBattleActionActor(actor));
            if(active_target->isFainted())
                break;
            if(active_target->isAsleep()){
                unsigned int bad_dreams_dmg = max(1,active_target->getMaxHP()/8);
                unsigned int actual_bad_dreams_dmg = active_target->addDirectDamage(bad_dreams_dmg);
                // event_handler->displayMsg(active_target->getNickname()+" took "+std::to_string(actual_bad_dreams_dmg)+" damage from Bad Dreams!");
                event_handler->displayDmgDealt(actual_bad_dreams_dmg,active_target,user_name+"'s Bad Dreams");
            }
            break;
        }
        default:break;
    }
    tryEjectPack(actor);
}

bool Battle::thereIsAbility(Ability ability){
    return (player_active->hasAbility(ability) && !player_active->isFainted()) || 
        (opponent_active->hasAbility(ability) && !opponent_active->isFainted());
}

unsigned int Battle::getMoney()const{
    return money;
}
void Battle::addMoney(unsigned int money){
    this->money += money;
}

void Battle::setWild(){
    if(opponent_team->getSize()>1){
        //error: cannot have wild battles with teams of size > 1
        event_handler->displayMsg("Error: cannot have wild battles against teams of size > 1");
        exit(1);
    }
    is_wild_battle = true;
}
void Battle::setBattleGivesExp(){
    battle_gives_exp = true;
}

void Battle::givePlayerExperience(Monster* defeated_mon){
    // gain EXPERIENCE
    Species * defeated_spec = Species::getSpecies(defeated_mon->getSpeciesId());
    unsigned int defeated_form_id = defeated_mon->getFormId();
    unsigned int base_exp = defeated_spec->getExpYield(defeated_form_id);
    unsigned int level = defeated_mon->getLevel();
    unsigned int level_player_active = player_active->getLevel();  
    std::vector<Monster*> player_experience_gainers;
    for(unsigned int i=0;i<player_team->getSize();i++){
        Monster* player_mon = player_team->getMonster(i);
        if(player_mon==nullptr)
            continue;
        if(player_mon->isFainted())
            continue;
        if(player_mon->hasSeenOpponent(defeated_mon))
            player_experience_gainers.push_back(player_mon);
    }
    for(Monster* mon: player_experience_gainers){
        //gain EXP
        unsigned int exp_gained = computeExperience(base_exp,level,true,level_player_active,
            false,(mon->getHeldItem()==LUCKY_EGG),mon->isPastEvoLevel());
        event_handler->displayMsg(mon->getNickname()+" gained "+std::to_string(exp_gained)+" EXP!");
        mon->gainExperience(exp_gained,event_handler);
        // gain EVs
        Stats ev_gained = defeated_spec->getEVYield(defeated_form_id);
        unsigned int multiplier = 1;
        if(mon->getHeldItem()==MACHO_BRACE)
            multiplier *= 2;
        mon->changeEffortHp(ev_gained.getHp()*multiplier);
        mon->changeEffortAtk(ev_gained.getAtk()*multiplier);
        mon->changeEffortDef(ev_gained.getDef()*multiplier);
        mon->changeEffortSpatk(ev_gained.getSpatk()*multiplier);
        mon->changeEffortSpdef(ev_gained.getSpdef()*multiplier);
        mon->changeEffortSpd(ev_gained.getSpd()*multiplier);
        //gain EVs from held items
        if(mon->getHeldItem()==POWER_WEIGHT)
            mon->changeEffortHp(8);
        if(mon->getHeldItem()==POWER_BRACER)
            mon->changeEffortAtk(8);
        if(mon->getHeldItem()==POWER_BELT)
            mon->changeEffortDef(8);
        if(mon->getHeldItem()==POWER_LENS)
            mon->changeEffortSpatk(8);
        if(mon->getHeldItem()==POWER_BAND)
            mon->changeEffortSpdef(8);
        if(mon->getHeldItem()==POWER_ANKLET)
            mon->changeEffortSpd(8);
    }
}

void Battle::checkForExp(){
    if(player_active->isFainted()){
        checkMonsterLeavingAbilities(PLAYER);
        if(monsters_defeated_by_opponent.find(player_active->getMonster()) == monsters_defeated_by_opponent.end())
            event_handler->displayMsg(player_active->getNickname()+" fainted!");
        monsters_defeated_by_opponent.insert(player_active->getMonster());
    }
    if(opponent_active->isFainted()){
        checkMonsterLeavingAbilities(OPPONENT);
        if(monsters_defeated_by_player.find(opponent_active->getMonster()) == monsters_defeated_by_player.end())
            event_handler->displayMsg(opponent_active->getNickname()+" fainted!");
        monsters_defeated_by_player.insert(opponent_active->getMonster());
        if(battle_gives_exp)
            givePlayerExperience(opponent_active->getMonster());
    }
}

void Battle::tryToCatchWildMonster(ItemType ball_used){
    ItemData* ball_data = ItemData::getItemData(ball_used);
    if(ball_data==nullptr || ball_data->getCategory()!=BALL)
        return;
    event_handler->displayMsg("Player throws a "+ball_data->getName()+"!");
    player_bag->removeItem(ball_used);
    Battler* active_user = getActorBattler(PLAYER);
    Battler* active_target = getActorBattler(OPPONENT);
    unsigned int form = active_target->getMonster()->getFormId();
    Species * spec = Species::getSpecies(form);
    unsigned int base_catch_rate = spec->getCatchRate(form);
    if(ball_used==MASTER_BALL){
        caught_wild_monster = true;
    }else{
        //try with other balls
        double ball_bonus=1;
        switch(ball_used){
            case GREAT_BALL:{
                ball_bonus = 1.5;
                break;
            }
            case ULTRA_BALL:{
                ball_bonus = 2;
                break;
            }
            case LEVEL_BALL:{
                unsigned int player_level = player_active->getLevel();
                unsigned int wild_level = active_target->getLevel();
                if(player_level >= 4 * wild_level){
                    ball_bonus = 8;
                }else if(player_level >= 2 * wild_level){
                    ball_bonus = 4;
                }else if(player_level > wild_level){
                    ball_bonus = 2;
                }
                break;
            }
            case MOON_BALL:{
                auto evos = spec->getEvolutions(form);
                bool evolves_with_moon_stone = false;
                for(auto evo:evos){
                    EvolutionMethod method = evo.getEvolutionMethod();
                    if(method!=USE_EVO_ITEM &&
                        method!=USE_EVO_ITEM_DAY &&
                        method!=USE_EVO_ITEM_NIGHT)
                        continue;
                    ItemType necessary_item = static_cast<ItemType>(evo.getMethodCondition());
                    if(necessary_item == MOON_STONE){
                        evolves_with_moon_stone = true;
                        break;
                    }
                }
                if(evolves_with_moon_stone){
                    ball_bonus = 4;
                }
                break;
            }
            case LOVE_BALL:{
                Gender wild_gender = active_target->getGender();
                Gender player_mon_gender = active_user->getGender();
                if(areMaleAndFemale(wild_gender,player_mon_gender) && 
                    active_user->getMonster()->getSpeciesId() == spec->getId()){
                    ball_bonus = 8;
                }
                break;
            }
            case HEAVY_BALL:{
                unsigned int weight = active_target->getWeight();
                if(weight>4096){
                    base_catch_rate = max(255,40+base_catch_rate);
                }else if(weight>3072){
                    base_catch_rate = max(255,30+base_catch_rate);
                }else if(weight>2048){
                    base_catch_rate = max(255,20+base_catch_rate);
                }else if(weight<1024){
                    base_catch_rate = max(1,base_catch_rate-20);
                }
                break;
            }
            case FAST_BALL:{
                unsigned int base_speed = spec->getBaseStats(form).getSpd();
                if(base_speed>=100)
                    ball_bonus=4;
                break;
            }
            case NET_BALL:{
                if(active_target->hasType(BUG) || active_target->hasType(WATER)){
                    ball_bonus = 3.5;
                }
                break;
            }
            case NEST_BALL:{
                unsigned int wild_level = active_target->getLevel();
                if(wild_level <= 29){
                    ball_bonus = (int)((41-wild_level)/10.0)/4096.0;
                }
                break;
            }
            case REPEAT_BALL:{
                //IF user had previously caught the same species in the past
                // boost to 3.5x
                break;
            }
            case TIMER_BALL:{
                ball_bonus = min(4.0,1 + turn*1229.0/4096.0);
                break;
            }
            case LURE_BALL:{
                //bonus_ball = 4 if battle happened in water
                break;
            }
            case DIVE_BALL:{
                //bonus_ball = 3.5 if battle happened in water
                break;
            }
            case DUSK_BALL:{
                if(isNight())
                    ball_bonus = 3.5;
                //bonus_ball = 3 if battle happened in cave
                break;
            }
            case QUICK_BALL:{
                if(turn == 0)
                    ball_bonus = 5;
                break;
            }
            case DREAM_BALL:{
                if(active_target->isAsleep())
                    ball_bonus = 4;
                break;
            }
            case BEAST_BALL:{
                // bonus_ball = 5 if used on ultra beast
                ball_bonus=410/4096.0;
                break;
            }
            default:{
                ball_bonus = 1;
                break;
            }
        }
        double status_bonus;
        switch(active_target->getPermanentStatus()){
            case SLEEP_1:
            case SLEEP_2:
            case SLEEP_3:
            case SLEEP_4:
            case FREEZE:{
                status_bonus = 2;
                break;
            }
            case PARALYSIS:
            case POISONED:
            case BAD_POISON:
            case BURN:{
                status_bonus = 1.5;
                break;
            }
            default:{
                status_bonus = 1;
                break;
            }
        }
        unsigned int catching_probability=max(1,
            (1.0 - (2.0 * active_target->getCurrentHP())/(3.0 * active_target->getMaxHP()))*ball_bonus*status_bonus*base_catch_rate);
        if(catching_probability > 255){
            caught_wild_monster = true;
        }else{
            unsigned int random_number = RNG::getRandomInteger(0,255);
            if(random_number < catching_probability)
                caught_wild_monster = true;
        }
    }
    if(caught_wild_monster){
        event_handler->displayMsg(active_target->getNickname()+" was caught!");
        Monster * caught_monster = new Monster(active_target->getMonster());
        caught_monster->setBall(ball_used);
        if(ball_used == FRIEND_BALL){
            unsigned int old_friendship = caught_monster->getFriendship();
            int friendship_gain = 200 - old_friendship;
            if(friendship_gain > 0){
                caught_monster->changeFriendship(friendship_gain);
            }
        }else if(ball_used == HEAL_BALL){
            //recover damage
            caught_monster->removeDamage(caught_monster->getMaxHP());
            // recover status
            caught_monster->setPermanentStatus(NO_PERMANENT_CONDITION);
            //recover all PP
            auto attacks = caught_monster->getAttacks();
            for(auto attack:attacks){
                caught_monster->recoverPP(attack.first,1000);
            }
        }
        if(!player_team->isFull())
            player_team->addMonster(caught_monster);//add a clone of the caught monster to the player team
        if(battle_gives_exp)
            givePlayerExperience(active_target->getMonster());
    }else{
        event_handler->displayMsg(active_target->getNickname()+" broke free!");
    }
}

void Battle::performEscape(BattleAction action){
    if(!is_wild_battle){
        event_handler->displayMsg("You cannot escape from trainer battles!");
        return;
    }
    runaway_attempts++;
    Battler* active_user = getActorBattler(PLAYER);
    Battler* active_target = getActorBattler(OPPONENT);
    active_user->removeVolatileCondition(LASER_FOCUS);
    std::string user_name = getActorBattlerName(PLAYER);
    unsigned int user_speed = active_user->getModifiedSpeed();
    unsigned int target_speed = max(1,active_target->getModifiedSpeed());
    bool escape_successful = false;
    if(!active_user->canSwitchOut(active_target)){
        event_handler->displayMsg(user_name+" is trapped and cannot escape!");
        return;
    }else if(user_speed > target_speed){
        escape_successful = true;
    }else{
        unsigned int escape_odds = ((user_speed*32)/(target_speed/4))+30*runaway_attempts;
        unsigned int random_number = RNG::getRandomInteger(0,255);
        if(random_number < escape_odds){
            escape_successful = true;
        }
    }
    if(escape_successful){
        event_handler->displayMsg("Player got away safely!");
        is_wild_battle_over = true;
    }else{
        event_handler->displayMsg("Player couldn't get away!");
    }
}

bool Battle::applySwitchInItemsEffects(BattleActionActor actor){
    // room service
    checkRoomService();
    // seeds
    return onTerrainChange(actor);
}

void Battle::applyItemPostDamage(BattleActionActor actor){
    // items
    Battler* active_user = getActorBattler(actor);
    switch(active_user->getHeldItem()){
        case FLAME_ORB:{
            // burn user
            if(active_user->hasPermanentStatus())
                break;
            active_user->setPermanentStatus(BURN);
            event_handler->displayMsg(active_user->getNickname()+" is burned by the Flame Orb!");
            break;
        }
        case TOXIC_ORB:{
            // poison user
            if(active_user->hasPermanentStatus())
                break;
            active_user->setPermanentStatus(BAD_POISON);
            event_handler->displayMsg(active_user->getNickname()+" is badly poisoned by the Toxic Orb!");
            break;
        }
        case STICKY_BARB:{
            // user takes 1/8 max HP damage
            if(active_user->hasAbility(MAGIC_GUARD) || active_user->hasSubstitute())
                break;
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int damage = max(max_hp / 8,1);
            unsigned int actual_damage = active_user->addDirectDamage(damage);
            // event_handler->displayMsg(active_user->getNickname()+" took "+std::to_string(actual_damage)+" damage from the Sticky Barb!");
            event_handler->displayDmgDealt(actual_damage,active_user,"its Sticky Barb");
            break;
        }
        case LEFTOVERS:{
            // user heals 1/16 max HP
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int heal_amount = max(max_hp / 16,1);
            unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
            if(actual_heal_amount>0)
                event_handler->displayMsg(active_user->getNickname()+" healed "+std::to_string(actual_heal_amount)+" HP!");
            break;
        }
        case BLACK_SLUDGE:{
            //black sludge behaves like leftovers for poison types
            // and like sticky barb for non-poison types
            if(!active_user->hasType(POISON)){
                //same as sticky barb
                if(active_user->hasAbility(MAGIC_GUARD))
                    break;
                unsigned int max_hp = active_user->getMaxHP();
                unsigned int damage = max(max_hp / 8,1);
                unsigned int actual_damage = active_user->addDirectDamage(damage);
                // event_handler->displayMsg(active_user->getNickname()+" took "+std::to_string(actual_damage)+" damage from the Sticky Barb!");
                event_handler->displayDmgDealt(actual_damage,active_user,"its Black Sludge");
            }else{
                //same as leftovers
                unsigned int max_hp = active_user->getMaxHP();
                unsigned int heal_amount = max(max_hp / 16,1);
                unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                if(actual_heal_amount>0)
                    event_handler->displayMsg(active_user->getNickname()+" healed "+std::to_string(actual_heal_amount)+" HP!");
            }
            break;
        }
        default:break;
    }
    tryEjectPack(actor);
}

bool Battle::tryEjectPack(BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    MonsterTeam* active_team = getActorTeam(actor);
    if(active_user->hasHeldItem(EJECT_PACK) && !active_user->isFainted() && 
        active_team->hasAliveBackup() && active_user->hasVolatileCondition(STAT_JUST_DROPPED)){
        // event_handler->displayMsg(active_user->getNickname()+" is switched out by the Eject Pack!");
        active_user->consumeHeldItem();
        forceSwitch(actor);
        return true;
    }
    // active_user->removeVolatileCondition(STAT_JUST_DROPPED);
    return false;
}

void Battle::changeStats(BattleActionActor actor,StatCV& changes,bool forced){
    Battler* active_battler = getActorBattler(actor);
    std::map<unsigned int,int> stats_successfully_increased;
    std::map<unsigned int,int> stats_successfully_decreased;
    for(auto change:changes){
        if(change.second==0)
            continue;
        bool res;
        switch(change.first){
            case 1:{//ATT
                if(forced)
                    res = active_battler->changeAttackModifierForced(change.second);
                else
                    res = active_battler->changeAttackModifier(change.second);
                break;
            }
            case 2:{//DEF
                if(forced)
                    res = active_battler->changeDefenseModifierForced(change.second);
                else
                    res = active_battler->changeDefenseModifier(change.second);
                break;
            }
            case 3:{//SPATT
                if(forced)
                    res = active_battler->changeSpecialAttackModifierForced(change.second);
                else
                    res = active_battler->changeSpecialAttackModifier(change.second);
                break;
            }
            case 4:{//SPDEF
                if(forced)
                    res = active_battler->changeSpecialDefenseModifierForced(change.second);
                else
                    res = active_battler->changeSpecialDefenseModifier(change.second);
                break;
            }
            case 5:{//SPEED
                if(forced)
                    res = active_battler->changeSpeedModifierForced(change.second);
                else
                    res = active_battler->changeSpeedModifier(change.second);
                break;
            }
            case 6:{//ACC
                if(forced)
                    res = active_battler->changeAccuracyModifierForced(change.second);
                else
                    res = active_battler->changeAccuracyModifier(change.second);
                break;
            }
            case 7:{//EVA
                if(forced)
                    res = active_battler->changeEvasionModifierForced(change.second);
                else
                    res = active_battler->changeEvasionModifier(change.second);
                break;
            }
            default:break;
        }
        if(!res)
            continue;
        if((change.second>0 && !active_battler->hasAbility(CONTRARY)) ||
            (change.second<0 && active_battler->hasAbility(CONTRARY))){
            //stat was increased
            stats_successfully_increased.insert({change.first,change.second>0?change.second:-change.second});
        }else{
            stats_successfully_decreased.insert({change.first,change.second>0?change.second:-change.second});
        }
    }
    if(!stats_successfully_increased.empty() && active_battler->hasHeldItem(MIRROR_HERB)){
        //mirror herb
        active_battler->consumeHeldItem();
        for(auto change: stats_successfully_increased){
            switch(change.first){
                case 1:{//ATT
                    active_battler->changeAttackModifier(change.second);
                    break;
                }
                case 2:{//DEF
                    active_battler->changeDefenseModifier(change.second);
                    break;
                }
                case 3:{//SPATT
                    active_battler->changeSpecialAttackModifier(change.second);
                    break;
                }
                case 4:{//SPDEF
                    active_battler->changeSpecialDefenseModifier(change.second);
                    break;
                }
                case 5:{//SPEED
                    active_battler->changeSpeedModifier(change.second);
                    break;
                }
                case 6:{//ACC
                    active_battler->changeAccuracyModifier(change.second);
                    break;
                }
                case 7:{//EVA
                    active_battler->changeEvasionModifier(change.second);
                    break;
                }
                default:break;
            }
        }
    }
    // if(!stats_successfully_decreased.empty())
    //     return tryEjectPack(actor);
    // return false;
}

void Battle::resetOpponents(){
    player_active->setOpponent(opponent_active);
    opponent_active->setOpponent(player_active);
}

void Battle::checkRoomService(){
    if(!field->hasFullFieldEffect(TRICK_ROOM)){
        return;
    }
    Battler* users[2] = {player_active,opponent_active};
    for(int j=0;j<2;j++){
        Battler* active_user = users[j];
        if(active_user->hasHeldItem(ROOM_SERVICE) && !active_user->isFainted()){
            // room service decreases speed under trick room by one stage and is consumed
            event_handler->displayMsg(active_user->getNickname()+"'s Room Service activates!");
            active_user->consumeHeldItem();
            StatCV changes = {{5,-1}};
            changeStats(active_user->getActor(),changes,false);
        }
    }
}

bool Battle::isCriticalHit(Attack* attack, BattleActionActor user_actor, BattleActionActor target_actor){
    Battler* active_user = getActorBattler(user_actor);
    Battler* active_target = getActorBattler(target_actor);
    unsigned int effect_id = attack->getEffectId();
    
    bool is_critical_hit = false;
    int crit_level = 0;
    if(active_user->hasVolatileCondition(LASER_FOCUS)){//laser focus makes crits more likely
        crit_level+=3;
        active_user->removeVolatileCondition(LASER_FOCUS);
    }
    if(active_user->hasHeldItem(RAZOR_CLAW)){//razor claw makes crits more likely for the user
        crit_level+=1;
    }
    if(active_user->hasHeldItem(SCOPE_LENS)){//scope lens makes crits more likely for the user
        crit_level+=1;
    }
    unsigned int species_id = active_user->getMonster()->getSpeciesId();
    if(active_user->hasHeldItem(LEEK) && (species_id == 83)){
        // TODO!() add sirfetchd once implemented here
        // leek makes crits more likely for farfetchd and Sirfetchd
        crit_level+=2;
    }
    if(active_user->hasHeldItem(LUCKY_PUNCH) && species_id == 113){
        // lucky punch makes crits more likely for chansey
        crit_level+=2;
    }
    if((effect_id == 17 || effect_id==197 || 
        effect_id==232 || effect_id == 305) 
        && !active_user->hasAbility(SHEER_FORCE)){//moves with increased crit ratio, whose effect is denied by sheer force
        crit_level+=1;
    }
    if(active_user->hasAbility(SUPER_LUCK)){
        crit_level+=1;
    }
    if(active_user->hasVolatileCondition(FOCUS_ENERGY)){//focus energy
        crit_level+=2;
    }
    if(active_user->hasVolatileCondition(INCREASED_CRIT)){//crit incresed by berry
        crit_level += 1;
        active_user->removeVolatileCondition(INCREASED_CRIT);
    }
    if(crit_level == 0 && RNG::getRandomInteger(0,23)==0){
        is_critical_hit = true;
    }else if(crit_level == 1 && RNG::oneEight()){
        is_critical_hit = true;
    }else if(crit_level == 2 && RNG::coinFlip()){
        is_critical_hit = true;
    }else if(crit_level>=3){
        is_critical_hit = true;
    }
    if(effect_id==189)//always crits unless abilities deny it
        is_critical_hit = true;
    // check for crit immunity
    if(active_target->hasAbility(SHELL_ARMOR) || //shell armor blocks crits
        active_target->hasAbility(BATTLE_ARMOR)){//battle armor blocks crits
        is_critical_hit = false;
    }
    return is_critical_hit;
}



void Battle::applyBattleActionModifiers(){
    for(Battler* user_active: getBattlersSortedBySpeed()){
        BattleAction& action = actions[user_active];
        BattleAction other_action = actions[getActorBattler(otherBattleActionActor(action.getActor()))];
        // Battler* opponent_active = getActorBattler(other_action.getActor());
        if(action.getActionType()==SWITCH){
            user_active->addVolatileCondition(SWITCHING_OUT,5);
            if(other_action.getAttackId()==PURSUIT_ID)
                other_action.setPriority(11);
        }
        // check prankster modifications
        if(user_active->hasAbility(PRANKSTER) && isAttackingActionType(action.getActionType())){
            Attack* attack = Attack::getAttack(action.getAttackId());
            if(attack->getCategory() == STATUS){
                action.setPriority(action.getPriority() + 1);
            }
        }
        // check grassy glide priority
        if(isAttackingActionType(action.getActionType())){
            Attack* attack = Attack::getAttack(action.getAttackId());
            if(attack->getEffectId()==261 && field->getTerrain()==GRASSY_FIELD && 
                user_active->isTouchingGround()){
                action.setPriority(action.getPriority() + 1);
            }
        }
        // apply trick room
        if(field->hasFullFieldEffect(TRICK_ROOM)){
            action.setSpeed(MAX_UNSIGNED - action.getSpeed());
        }
        // apply action speed mods
        user_active->tryEatStartOfTurnBerry();
        if(user_active->hasAbility(QUICK_DRAW) && RNG::getRandomInteger(1,10)<4){
            //quick draw gives 30% chance to the user to act first in its priority bracket
            user_active->addVolatileCondition(MOVING_FIRST,1);
        }
        if(user_active->hasHeldItem(QUICK_CLAW) && RNG::getRandomInteger(1,10)<3){
            //quick claw gives 20% chance to the user to act first in its priority bracket
            user_active->addVolatileCondition(MOVING_FIRST,1);
        }
        //apply moving first condition
        if(user_active->hasVolatileCondition(MOVING_FIRST)){
            user_active->removeVolatileCondition(MOVING_FIRST);
            action.setSpeed(MAX_UNSIGNED);
        }
        // check for lagging tail and full incense
        if(user_active->hasHeldItem(LAGGING_TAIL) || user_active->hasHeldItem(FULL_INCENSE))
            user_active->addVolatileCondition(MOVING_LAST,1);
        if(user_active->hasAbility(STALL))
            user_active->addVolatileCondition(MOVING_LAST,1);
        // apply moving last condition
        if(user_active->hasVolatileCondition(MOVING_LAST)){
            user_active->removeVolatileCondition(MOVING_LAST);
            action.setSpeed(0);
        }
    }
    
}


double Battle::computeEffectiveness(Attack* attack, BattleActionActor user_actor, BattleActionActor target_actor){
    Battler* active_user = getActorBattler(user_actor);
    Battler* active_target = getActorBattler(target_actor);
    bool can_hit_ghosts = active_user->hasAbility(SCRAPPY);
    // Type attack_type = attack->getType(active_user,field);
    unsigned int effect_id = attack->getEffectId();
    Type attack_type = attack->getType(active_user,field);
    double effectiveness = 1;
    for(auto type:active_target->getTypes()){
        double type_effectiveness = getTypeEffectiveness(
            attack_type,
            type,
            active_target->isTouchingGround(),
            can_hit_ghosts,
            effect_id == 196
        );
        if(active_user->hasHeldItem(RING_TARGET) && type_effectiveness==0)
            type_effectiveness = 1;
        effectiveness *= type_effectiveness;
    }
    // check for immunity for sheer cold
    if(effect_id == 156 && active_target->hasType(ICE)){
        effectiveness = 0;
    }
    // check for immunity for soundproof
    if(attack->isSoundBased() && active_target->hasAbility(SOUNDPROOF)){
        effectiveness = 0;
    }
    // check for immunity for overcoat
    if(attack->isPowder() && (active_target->hasAbility(OVERCOAT) || active_target->hasHeldItem(SAFETY_GOGGLES))){
        effectiveness = 0;
    }
    // TINTED LENS doubles effectiveness of not very effective moves
    if(effectiveness < 0.9 && active_user->hasAbility(TINTED_LENS)){
        effectiveness *= 2;
    }
    // check for wonder guard immunity
    if(effectiveness < 1.1 && active_target->hasAbility(WONDER_GUARD)){
        effectiveness = 0;
    }
    // check for strong winds 
    if(field->getWeather() == STRONG_WINDS && !thereIsaCloudNine() &&
        active_target->hasType(FLYING) && effectiveness > 1.1){
        effectiveness = 1;
    }
    return effectiveness;
}

void Battle::onWeatherChange(Weather new_weather){
    auto battlers = getBattlersSortedBySpeed();
    for(auto battler:battlers){
        battler->onWeatherChange(new_weather);
    }
}

void Battle::checkMonsterLeavingAbilities(BattleActionActor actor){
    Battler* user_active = getActorBattler(actor);
    Battler* target_active = getActorBattler(otherBattleActionActor(actor));
    std::string user_name = getActorBattlerName(actor);
    std::string target_name = getActorBattlerName(otherBattleActionActor(actor));
    if(thereIsAbility(NEUTRALIZING_GAS)){
        player_active->neutralizeAbility();
        opponent_active->neutralizeAbility();
    }else{
        player_active->cancelAbilityNeutralization();
        opponent_active->cancelAbilityNeutralization();
    }
    if(field->getWeather() == EXTREME_SUN && !thereIsAbility(DESOLATE_LAND)){
        if(!(thereIsAbility(PRIMORDIAL_SEA) || thereIsAbility(DELTA_STREAM))){
            field->clearWeather();
        }
    }
    if(field->getWeather() == HEAVY_RAIN && !thereIsAbility(PRIMORDIAL_SEA)){
        if(!(thereIsAbility(DESOLATE_LAND) || thereIsAbility(DELTA_STREAM))){
            field->clearWeather();
        }
    }
    if(field->getWeather() == STRONG_WINDS && !thereIsAbility(DELTA_STREAM)){
        if(!(thereIsAbility(DESOLATE_LAND) || thereIsAbility(PRIMORDIAL_SEA))){
            field->clearWeather();
        }
    }
    // check if user has no weather ability
    if(!user_active->hasAbility(DELTA_STREAM) && 
        !user_active->hasAbility(PRIMORDIAL_SEA) &&
        !user_active->hasAbility(DESOLATE_LAND) &&
        !target_active->isFainted()){
        // opponent's ability may trigger
        Ability target_ability = target_active->getAbility();
        switch(target_ability){
            case DESOLATE_LAND:{
                if(field->getWeather() != EXTREME_SUN){
                    event_handler->displayMsg(target_name+"'s Desolate Land made the sun shine harshly!");
                    field->setWeather(EXTREME_SUN,-1);
                }
                break;
            }
            case PRIMORDIAL_SEA:{
                if(field->getWeather() != HEAVY_RAIN){
                    event_handler->displayMsg(target_name+"'s Primordial Sea made it rain heavily!");
                    field->setWeather(HEAVY_RAIN,-1);
                }
                break;
            }
            case DELTA_STREAM:{
                if(field->getWeather() != STRONG_WINDS){
                    event_handler->displayMsg(target_name+"'s Delta Stream made the weather change!");
                    field->setWeather(STRONG_WINDS,-1);
                }
                break;
            }
            default: break;
        }
    }
}

void Battle::performMegaEvolutions(){
    std::set<Battler*> battler_megas;
    for(Battler* battler: getBattlersSortedBySpeed()){
        if(battler->isFainted())
            continue;
        MonsterTeam * team = getActorTeam(battler->getActor());
        if(actions[battler].isMega() && battler->canMegaEvolve() && !team->hasMega()){
            //check if battler megas already contains a Monster on the same team that megas
            // if so, do not allow the other one to mega evolve
            bool already_other_mega = false;
            for(Battler* battler2: battler_megas){
                if(battler2->getActor() == battler->getActor()){
                    already_other_mega = true;
                    break;
                }
            }
            if(!already_other_mega)
                battler_megas.insert(battler);
        }
    }
    for(Battler* battler: battler_megas){
        battler->megaEvolve();
        actions[battler].setSpeed(battler->getModifiedSpeed());
    }
    for(Battler* battler: battler_megas){
        applySwitchInAbilitiesEffects(battler);
    }
}

void Battle::checkZenModes(){
    for(auto battler: getBattlersSortedBySpeed())
        battler->checkZenMode();
}

std::vector<Battler*> Battle::getBattlersSortedBySpeed(){
    unsigned int player_speed = player_active->getModifiedSpeed();
    unsigned int opponent_speed = opponent_active->getModifiedSpeed();
    std::vector<Battler*> sorted_battlers;
    if(player_speed > opponent_speed || (player_speed == opponent_speed && RNG::coinFlip())){
        sorted_battlers.push_back(player_active);
        sorted_battlers.push_back(opponent_active);
    }else{
        sorted_battlers.push_back(opponent_active);
        sorted_battlers.push_back(player_active);
    }
    return sorted_battlers;
}

std::vector<Battler*> Battle::getBattlersSortedByAction(){
    // priority - speed
    struct PrioritySpeedCmp{
        bool operator()(const std::pair<int,unsigned int>& a,const std::pair<int,unsigned int>& b)const{
            if(a.first == b.first)
                return a.second < b.second;
            return a.first < b.first;
        }
    };
    std::map<std::pair<int,unsigned int>,std::vector<Battler*>,PrioritySpeedCmp> buckets_actions;
    for(auto action: actions){
        Battler* battler = action.first;
        if(battler->isFainted())
            continue;
        int priority = action.second.getPriority();
        unsigned int speed = action.second.getSpeed();
        std::pair<int,unsigned int> key = {priority,speed};
        if(buckets_actions.find(key) == buckets_actions.end()){
            buckets_actions[key] = std::vector<Battler*>();
            buckets_actions[key].push_back(action.first);
        }else{
            buckets_actions[key].push_back(action.first);
        }
    }
    std::vector<Battler*> sorted_battlers;
    for(auto it = buckets_actions.rbegin(); it != buckets_actions.rend(); ++it){
        std::vector<Battler*> bucket = it->second;
        while(!bucket.empty()){
            unsigned int random_index = RNG::getRandomInteger(0,bucket.size()-1);
            Battler* acting_battler = bucket[random_index];
            bucket.erase(bucket.begin() + random_index);
            if(acting_battler->isFainted())
                continue;
            sorted_battlers.push_back(acting_battler);
        }
    }
    #ifdef DEBUG
    for(auto battler: sorted_battlers){
        BattleAction action = actions[battler];
        std::cout<< "Battler: "<<battler->getNickname()<<" Speed: "<<action.getSpeed()<<" Priority: "<<action.getPriority()<<std::endl;
    }
    std::cout.flush();
    #endif
    return sorted_battlers;
}