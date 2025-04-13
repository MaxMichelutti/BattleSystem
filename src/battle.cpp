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
        case OUTRAGE:
        case THRASH:
        case SOLAR_BEAM:
        case FLY:
        case ROLLOUT:
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
ScheduledFutureSight::ScheduledFutureSight(unsigned int turns_left, 
        unsigned int user_special_attack, 
        unsigned int level,
        BattleActionActor target,
        bool stab) {
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
}

BattleAction::BattleAction(BattleActionActor actor,BattleActionType type, unsigned int attack_id, 
    int priority, unsigned int speed, unsigned int switch_id, ItemType item_to_use) {
    this->item_to_use = item_to_use;
    this->action_type = type;
    this->actor = actor;
    this->attack_id = attack_id;
    this->priority = priority;
    this->speed = speed;
    this->switch_id = switch_id;
    if(action_type == SWITCH)
        this->priority = 10;
    else if(action_type == USE_ITEM)
        this->priority = 20;
}
BattleAction::~BattleAction() {}

bool BattleAction::operator==(const BattleAction& other) const {
    return (speed == other.speed) && (priority == other.priority);
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
    this->event_handler = handler;
    this->cpu_ai = new CPUAI(cpu_skill);
    this->player_team = new MonsterTeam(*player_team);
    this->opponent_team = new MonsterTeam(*opponent_team);
    field = new Field(handler);
    player_active = new Battler(player_team->getActiveMonster(),field,PLAYER,event_handler);
    opponent_active = new Battler(opponent_team->getActiveMonster(),field,OPPONENT,event_handler);
    turn = 0;
    last_attack_used_id = 0;
    rounds_used_this_turn = 0;
    turn_of_opponent_last_kill = -2;
    turn_of_player_last_kill = -2;
    this->player_bag = user_bag;
    this->opponent_bag = opponent_bag;
}

Battle::Battle(unsigned int cpu_skill, EventHandler* handler, 
        MonsterTeam* player_team, MonsterTeam* opponent_team, 
        Weather weather, Terrain terrain,
        Bag * user_bag, Bag* opponent_bag) {
    this->event_handler = handler;
    this->cpu_ai = new CPUAI(cpu_skill);
    this->player_team = new MonsterTeam(*player_team);
    this->opponent_team = new MonsterTeam(*opponent_team);
    field = new Field(handler);
    player_active = new Battler(player_team->getActiveMonster(),field,PLAYER,handler);
    opponent_active = new Battler(opponent_team->getActiveMonster(),field,OPPONENT,handler);
    field->setWeather(weather);
    field->setTerrain(terrain);
    turn = 0;
    last_attack_used_id = 0;
    rounds_used_this_turn = 0;
    turn_of_opponent_last_kill = -2;
    turn_of_player_last_kill = -2;
    this->player_bag = user_bag;
    this->opponent_bag = opponent_bag;
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
    unsigned int speed_player = player_active->getModifiedSpeed();
    unsigned int speed_opponent = opponent_active->getModifiedSpeed();
    if(speed_player == speed_opponent){
        if(RNG::coinFlip()){
            applyImpostorSwitchIn(PLAYER);
            applyImpostorSwitchIn(OPPONENT);
            applySwitchInAbilitiesEffects(PLAYER);
            applySwitchInAbilitiesEffects(OPPONENT);
        }else{
            applyImpostorSwitchIn(OPPONENT);
            applyImpostorSwitchIn(PLAYER);
            applySwitchInAbilitiesEffects(OPPONENT);
            applySwitchInAbilitiesEffects(PLAYER);
        }
    }else if(speed_player > speed_opponent){
        applyImpostorSwitchIn(PLAYER);
        applyImpostorSwitchIn(OPPONENT);
        applySwitchInAbilitiesEffects(PLAYER);
        applySwitchInAbilitiesEffects(OPPONENT);
    }else{
        applyImpostorSwitchIn(OPPONENT);
        applyImpostorSwitchIn(PLAYER);
        applySwitchInAbilitiesEffects(OPPONENT);
        applySwitchInAbilitiesEffects(PLAYER);
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
    player_active->decrementDisabledTurns();
    opponent_active->decrementDisabledTurns();
    #ifdef DEBUG
    std::cout<<"Leaving ground"<<std::endl;
    std::cout.flush();
    #endif
    // remove roost grounding effect
    player_active->leaveGround();
    opponent_active->leaveGround();
    // reset turn damage counters for battlers
    player_active->resetDamageTakenThisTurn();
    opponent_active->resetDamageTakenThisTurn();
    // remobe charged 2 effect
    player_active->removeVolatileCondition(CHARGED_2);
    opponent_active->removeVolatileCondition(CHARGED_2);
    // remove endure effects
    player_active->removeVolatileCondition(ENDURE);
    opponent_active->removeVolatileCondition(ENDURE);
    // remove focus
    player_active->removeVolatileCondition(FOCUSED);
    opponent_active->removeVolatileCondition(FOCUSED);
    checkUproars();
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
    player_active->nextTurn();
    opponent_active->nextTurn();
    // 1: choose actions
    #ifdef DEBUG
    std::cout<<"Choosing action player "<<std::endl;
    std::cout.flush();
    #endif
    BattleAction player_action = event_handler->chooseAction(player_active,player_team,opponent_active,field,player_bag);
    #ifdef DEBUG
    std::cout<<"Choosing action CPU "<<std::endl;
    std::cout.flush();
    #endif
    BattleAction opponent_action = cpu_ai->chooseAction(opponent_active,opponent_team,player_active,field,opponent_bag);
    // 2: focus monsters
    if(player_action.getAttackId() == FOCUS_PUNCH_ID){
        player_active->addVolatileCondition(FOCUSED,5);
    }
    if(opponent_action.getAttackId() == FOCUS_PUNCH_ID){
        opponent_active->addVolatileCondition(FOCUSED,5);
    }
    // 3: sort actions by priority
    #ifdef DEBUG
    std::cout<<"Computing action order "<<std::endl;
    std::cout.flush();
    #endif
    std::vector<BattleAction> actions;
    if (player_action == opponent_action){// they are equal in speed and priority
        if(RNG::coinFlip()){
            opponent_action.setOrder(1);
            actions.push_back(opponent_action);
            player_action.setOrder(2);
            actions.push_back(player_action);
        }else{
            player_action.setOrder(1);
            actions.push_back(player_action);
            opponent_action.setOrder(2);
            actions.push_back(opponent_action);
        }
    }else if(player_action > opponent_action){
        player_action.setOrder(1);
        actions.push_back(player_action);
        opponent_action.setOrder(2);
        actions.push_back(opponent_action);
    }else{
        opponent_action.setOrder(1);
        actions.push_back(opponent_action);
        player_action.setOrder(2);
        actions.push_back(player_action);
    }
    #ifdef DEBUG
    std::cout<<"Perfrming actions "<<std::endl;
    std::cout.flush();
    #endif
    // 4: perform actions
    for(auto action: actions){
        performAction(action, actions);
        if(isOver()){
            return;
        }
        if(thereIsNeutralizingGas()){
            player_active->neutralizeAbility();
            opponent_active->neutralizeAbility();
        }else{
            player_active->cancelAbilityNeutralization();
            opponent_active->cancelAbilityNeutralization();
        }
    }
    // std::cout<<"ATTACKS DONE!"<<std::endl;std::cout.flush();
    #ifdef DEBUG
    std::cout<<"Applying scheduled future sights "<<std::endl;
    std::cout.flush();
    #endif
    applyScheduledFutureSights();
    if(isOver()){return;}
    // 4.5 check if something is dead and in case remove some volatiles
    if(thereIsNeutralizingGas()){
        player_active->neutralizeAbility();
        opponent_active->neutralizeAbility();
    }else{
        player_active->cancelAbilityNeutralization();
        opponent_active->cancelAbilityNeutralization();
    }
    if(player_active->isFainted()){
        removeVolatilesFromOpponentOfMonsterLeavingField(PLAYER);
    }
    if(opponent_active->isFainted()){
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

void Battle::performAction(BattleAction action, std::vector<BattleAction>& all_actions){
    Battler* active_user = getActorBattler(action.getActor());
    Battler* other_active = getActorBattler(otherBattleActionActor(action.getActor()));
    // MonsterTeam* user_team = getActorTeam(action.getActor());
    // MonsterTeam* target_team = getActorTeam(otherBattleActionActor(action.getActor()));
    // check if user is alive
    if(active_user->isFainted()){
        return;
    }
    // perform action
    BattleActionType atype = action.getActionType();
    if(isAttackingActionType(atype)){
        performAttack(action,all_actions);
        // deactivate mold breaker
        active_user->deactivateMoldBreaker();
        other_active->deactivateMoldBreaker();
    }else if(atype == SWITCH){
        performSwitch(action);
    }else if(atype==RECHARGE_TURN){
        performRechargeTurn(action);
    }else if(atype == USE_ITEM){
        performUseItem(action);
    }
}

void Battle::performUseItem(BattleAction action){
    Battler* active_user = getActorBattler(action.getActor());
    if(active_user->isFainted())
        return;
    std::string user_mon_name = getActorBattlerName(action.getActor());
    ItemType item_to_use = action.getItemToUse();
    ItemData* item_data = ItemData::getItemData(item_to_use);
    std::string player_name;
    Bag * bag_used;
    if(action.getActor() == PLAYER){
        player_name = "Player";
        bag_used = player_bag;
    }else{
        player_name = "Opponent";
        bag_used = opponent_bag;
    }
    event_handler->displayMsg(player_name+" used "+item_data->getName()+" on "+user_mon_name+"!");
    bag_used->removeItem(item_to_use,1);
    active_user->useItem(item_to_use);
}

void Battle::performRechargeTurn(BattleAction action){
    Battler* active_user = getActorBattler(action.getActor());
    if(active_user->isFainted())
        return;
    std::string user_mon_name = getActorBattlerName(action.getActor());
    active_user->removeVolatileCondition(RECHARGING);
    event_handler->displayMsg(user_mon_name+" is recharging!");
}

void Battle::performAttack(BattleAction action, std::vector<BattleAction>& all_actions){
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
        for(auto action: all_actions){
            if(action.getActor() == OPPONENT){
                other_action = action;
                break;
            }
        }
        user_player_name = "Player";
        opponents_player_name = "Opponent";
    }else{
        for(auto action: all_actions){
            if(action.getActor() == PLAYER){
                other_action = action;
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
    active_target->activateMoldBreaker();

    // reset Destiny Bond
    if(active_user->hasVolatileCondition(DESTINY_BOND))
        active_user->removeVolatileCondition(DESTINY_BOND);
    // remove protects if move is not protect
    if(attack->getEffectId() != 26){
        active_user->resetConsecutiveProtect();
    }
    //reset charged if move is electric type
    if(attack->getType() == ELECTRIC && active_user->hasVolatileCondition(CHARGED)){
        active_user->removeVolatileCondition(CHARGED);
        active_user->addVolatileCondition(CHARGED_2,-1);
    }
    // sleep
    // remove sleep during uproars
    if(active_user->isAsleep() && isUproar()){
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
        if(active_user->hasAbility(STEADFAST))
            active_user->changeSpeedModifierForced(1);
        forgetMoveVolatiles(active_user);
        active_user->setLastAttackFailed();
        return;
    }
    // apply freeze
    if(active_user->isFrozen()){
        unsigned int random_number = RNG::getRandomInteger(1,100);
        if(random_number > 20){
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
                forgetMoveVolatiles(active_user);
                unsigned int level = active_user->getLevel();
                unsigned int attack_stat = active_user->getModifiedAttack();
                unsigned int defense_stat = active_user->getModifiedDefense();
                unsigned int damage = max(int(baseDamage(
                    level, 40, attack_stat, defense_stat)),1);
                unsigned int actual_damage = active_user->addDirectDamage(damage);
                event_handler->displayMsg(user_mon_name+" dealt "+std::to_string(actual_damage)+" damage to itself!");
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
    event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
    active_user->setLastAttackUsed(attack_id);
    if(attack->getEffectId() != 80){// I need to preserve this value if copycat is being ised
        last_attack_used_id = attack_id;
    }
    if(action.getActionType() == ATTACK && attack_id != STRUGGLE_ID){
        // cannot remove PP to struggle
        unsigned int PP_amount = 1;
        if(active_target->hasAbility(PRESSURE))
            PP_amount = 2;
        active_user->usePP(attack_id, PP_amount);
    }
    // copycat changes move
    if(attack->getEffectId() == 80){
        // event_handler->displayMsg(user_mon_name+" uses "+attack->getName()+"!");
        if(last_attack_used_id == 0){
            event_handler->displayMsg("But it failed!");
            decrementVolatiles(active_user);
            active_user->removeVolatileCondition(LASER_FOCUS);
            return;
        }else{
            attack_id = last_attack_used_id;
            attack = Attack::getAttack(last_attack_used_id);
            event_handler->displayMsg(user_mon_name+" copies "+attack->getName()+"!");
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
    // sleep talk changes attack
    if(attack->getEffectId()==194){
        if(!active_user->isAsleep()){
            event_handler->displayMsg(user_mon_name+"But it failed!");
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
    //increment rounds counter
    if(attack->getEffectId() == 94){
        rounds_used_this_turn++;
    }

    if(checkIfAttackFails(attack,action,other_action)){
        return;
    }
    active_user->setLastAttackHit();

    
    // check if attack hits
    if(checkIfMoveMisses(attack,action.getActor())){// IF MISS
        event_handler->displayMsg(opponent_mon_name+" avoided the attack!");
        decrementVolatiles(active_user);
        active_user->setLastAttackFailed();
        active_user->removeVolatileCondition(LASER_FOCUS);
        active_user->removeVolatileCondition(ROLLINGOUT);
        if(attack->getEffectId()==100){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        if(attack->getEffectId() == 171 || attack->getEffectId() == 174){
            // user takes 50% maxHP recoil damage
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int damage = max_hp / 2;
            unsigned int actual_damage = active_user->addDirectDamage(damage);
            event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage from recoil!"); 
        }
        return;
    }
    
    unsigned int actual_damage = applyDamage(attack,action.getActor(),attack_after_target);
    if(active_user->isFainted())
        return;
    if(actual_damage==0 && !(attack->getCategory()==STATUS)) // Immunity
        return;

    // apply effects
    applyAttackEffect(attack,action.getActor());
    // these pointers may have changed due to move effects!!!
    active_user = getActorBattler(action.getActor());
    active_target = getActorBattler(otherBattleActionActor(action.getActor()));
    if(active_user->isFainted())
        return;

    // apply recoil damage
    applyRecoil(attack,actual_damage,action.getActor());
    if(active_user->isFainted() || active_target->isFainted())
        return;

    // decrement volatile
    decrementVolatiles(active_user);
}

bool Battle::checkIfMoveMisses(Attack* attack, BattleActionActor actor){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    AttackTarget target = attack->getTarget();
    //no guard bypasses accuracy check and moves never miss
    if(active_user->hasAbility(NO_GUARD))
        return false;
    // if the user is locked onto thew target, it cannot miss
    // (even in case of semi-invulnerable moves)
    if(active_user->hasLockOn(active_target->getMonster())){
        return false;
    }
    // fly (target is flying high and cannot be hit)
    if(active_target->hasVolatileCondition(FLYING_HIGH) && target == TARGET_OPPONENT){
        if(attack->getEffectId()!=45)//twister also hits monsters that are flying
            return true;   
    }
    // dig (target is underground and cannot be hit)
    if(active_target->hasVolatileCondition(UNDERGROUND) && target == TARGET_OPPONENT){
        if(attack->getEffectId()!=73)//earthquake also hits monsters that are underground
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
    if(attack->getEffectId()==141 && active_target->hasUsedAttack(MINIMIZE_ID)){
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
        if(active_target->hasAbility(WONDER_SKIN) && attack->getCategory() == STATUS){
            // WONDER SKIN drops accuracy of status moves that apply permanent conditions
            switch(attack->getEffectId()){
                case 4:case 5:case 14:
                case 16:case 61:case 67:
                case 76:{
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
            if(field->getWeather() == RAIN)
                modified_evasion = 0;
            else if(field->getWeather() == SUN && !thereIsaCloudNine())
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
        // TANGLED FEET effect
        if(active_target->hasAbility(TANGLED_FEET) && active_target->hasVolatileCondition(CONFUSION)){
            modified_accuracy *= 0.5;
        }
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

unsigned int Battle::applyDamage(Attack* attack,BattleActionActor actor, bool target_attack_first){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    // check if move is multi hit
    unsigned int number_of_hits = 1;
    if(attack->getEffectId() == 36){//moves hitting from 2 to 5 times
        int random_number = RNG::getRandomInteger(1,8);
        if(random_number==8)
            number_of_hits = 5;
        else if(random_number==7)
            number_of_hits = 4;
        else if(random_number>3)
            number_of_hits = 3;
        else
            number_of_hits = 2;
        if(active_user->hasAbility(SKILL_LINK))//skill link always guarantees 5 shots
            number_of_hits = 5;
    }else if(attack->getEffectId()==75){//double hitting moves
        number_of_hits = 2;
    }
    // perform damage calc and apply damage
    unsigned int actual_damage = 0;
    unsigned int total_actual_damage = 0;
    unsigned int actual_hits = 0;
    if(attack->getCategory() != STATUS){
        bool can_hit_ghosts = active_user->hasAbility(SCRAPPY);
        float effectiveness = getTypeEffectiveness(
            attack->getType(), 
            active_target->getTypes(),
            active_target->isTouchingGround(),
            can_hit_ghosts,
            attack->getEffectId() == 196
        );
        // check for immunity for sheer cold
        if(attack->getEffectId() == 156 && active_target->hasType(ICE)){
            effectiveness = 0;
        }
        // check for immunity for soundproof
        if(attack->isSoundBased() && active_target->hasAbility(SOUNDPROOF)){
            effectiveness = 0;
        }
        // check for immunity for overcoat
        if(attack->isPowder() && active_target->hasAbility(OVERCOAT)){
            effectiveness = 0;
        }
        // check immunity
        if(effectiveness == 0){
            event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
            decrementVolatiles(active_user);
            if(attack->getEffectId() == 171 || attack->getEffectId() == 174){
                // user takes 50% maxHP recoil damage
                unsigned int max_hp = active_user->getMaxHP();
                unsigned int damage = max_hp / 2;
                unsigned int actual_damage = active_user->addDirectDamage(damage);
                event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage from recoil!"); 
            }
            return 0;
        }
        //cycle in order to deal with multi hit moves
        for(unsigned int i=0;i<number_of_hits;i++){
            actual_hits++;
            // TINTED LENS doubles effectiveness of not very effective moves
            if(effectiveness < 0.9 && active_user->hasAbility(TINTED_LENS)){
                effectiveness *= 2;
            }
            //check for critical hit;
            bool is_critical_hit = false;
            if(active_user->hasVolatileCondition(LASER_FOCUS)){// laser focus guarantees crit
                is_critical_hit = true;
                active_user->removeVolatileCondition(LASER_FOCUS);
            }else if((attack->getEffectId() == 17 || attack->getEffectId()==197) && !active_user->hasAbility(SHEER_FORCE)){ 
                // Moves with increased crit chance
                if(active_user->hasVolatileCondition(FOCUS_ENERGY))//focus energy + increse crit chance = guaranteed crit
                    is_critical_hit = true;
                else if(RNG::oneEight())
                    is_critical_hit = true;
            }else if(active_user->hasVolatileCondition(FOCUS_ENERGY)){// focus energy crit ratio
                is_critical_hit = RNG::coinFlip();
            }else if(0==RNG::getRandomInteger(0,23))//normal crit ratio
                is_critical_hit = true;
            if(attack->getEffectId()==189)//always crits unless abilities deny it
                is_critical_hit = true;
            // check for crit immunity
            if(active_target->hasAbility(SHELL_ARMOR) || //shell armor blocks crits
                active_target->hasAbility(BATTLE_ARMOR)){//battle armor blocks crits
                is_critical_hit = false;
            }
            if(is_critical_hit && attack->getCategory() != STATUS){
                event_handler->displayMsg("It's a critical hit!");
            }
            // apply damage
            unsigned int damage = max(computeDamage(attack->getId(), actor, is_critical_hit, target_attack_first),1);
            if(attack->getEffectId() == 150 && damage >= active_target->getCurrentHP()){// user cannot be killed by this move
                damage = active_target->getCurrentHP()-1;
            }
            if(attack->getEffectId() == 103 || attack->getEffectId()==156){//one hit KO
                event_handler->displayMsg("It's a one hit KO!");//notice that this may only OHKO the substitute!
                damage = active_target->getMaxHP();
            }
            actual_damage = active_target->addDamage(damage,attack->getCategory());
            active_target->hitOnceMore(attack->getType());
            
            total_actual_damage += actual_damage;
            if(number_of_hits>1)
                event_handler->displayMsg(opponent_mon_name+" took "+std::to_string(actual_damage)+" damage!");

            
            // contact effects
            applyContactEffects(attack,actor);
            if(active_target->isFainted()){
                if(actor == PLAYER)
                    turn_of_player_last_kill = turn;
                else
                    turn_of_opponent_last_kill = turn;
                break;
            }
            // justified increase attack by 1 when hit by a dark move
            if(attack->getType() == DARK && active_user->hasAbility(JUSTIFIED)){
                event_handler->displayMsg(user_mon_name+"'s Justified triggers!");
                active_user->changeAttackModifierForced(1);
            }

            //anger point raises attack of taget when hit by a crit
            if(is_critical_hit && active_target->hasAbility(ANGER_POINT)){
                event_handler->displayMsg(opponent_mon_name+"'s Anger Point triggers!");
                active_target->changeAttackModifierForced(12);
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
        else // number_of hits == 1
            event_handler->displayMsg(opponent_mon_name+" took "+std::to_string(actual_damage)+" damage!");
        //active_user->setLastAttackUsed(attack->getId());
        last_attack_used_id = attack->getId();
        if(active_target->isFrozen() && attack->getType()==FIRE)
            active_target->clearPermanentStatus();
        // check if target is dead
        if(active_target->isFainted()){
            if(attack->getEffectId()==44){// if the damage of the attack kills the target, the user gains +2 ATT
                active_user->changeAttackModifier(+2);
            }
            if(active_target->hasVolatileCondition(DESTINY_BOND)){
                event_handler->displayMsg(opponent_mon_name+" takes "+user_mon_name+" with it!");
                active_user->addDirectDamage(active_user->getMaxHP());
            }
            if(active_target->hasAbility(AFTERMATH) &&
                !active_user->hasAbility(DAMP) &&
                !active_user->isFainted()){
                event_handler->displayMsg(opponent_mon_name+"'s Aftermath triggers!");
                unsigned int aftermath_damage = active_user->getMaxHP() / 4;
                unsigned int actual_aftermath_damage = active_user->addDirectDamage(aftermath_damage);
                event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_aftermath_damage)+" damage!");
            }
            if(!active_user->isFainted() && active_user->hasAbility(MOXIE) && active_user->getAttackModifier()!=MAX_MODIFIER){
                // moxie increases attack by 1 when a target is KOed by a user's attack
                event_handler->displayMsg(user_mon_name+"'s Moxie triggers!");
                active_user->changeAttackModifier(1);
            }
            return total_actual_damage;
        }
    }else{
        //active_user->setLastAttackUsed(attack->getId());
        last_attack_used_id = attack->getId();
        active_user->removeVolatileCondition(LASER_FOCUS);
    }
    return total_actual_damage;
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
        effect != 117) // death after use
        return;
    if(actual_damage>0){
        switch(effect){
            case 6:{//recoil 1/4 damage dealt
                unsigned int recoil_damage = max(actual_damage / 4,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                if(active_user->isFainted()){
                    return;
                }
                break;
            }
            case 11:{ // Struggle
                unsigned int recoil_damage = max(active_user->getMaxHP() / 4,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                if(active_user->isFainted())
                    return;
                break;
            }
            case 20:case 30:case 51:{//recoil 1/3 damage dealt
                unsigned int recoil_damage = max(actual_damage / 3,1);
                unsigned int actual_recoil_damage = active_user->addDirectDamage(recoil_damage);
                event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_recoil_damage)+" recoil damage!");
                if(active_user->isFainted())
                    return;
                break;
            }
            case 97:case 162:{//heal 50% of damage dealt (negative recoil)
                unsigned int heal_amount = max((actual_damage+1) / 2,1);
                if(!active_target->hasAbility(LIQUID_OOZE)){
                    unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                    if(actual_heal_amount>0)
                        event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
                }else{
                    unsigned int actual_damage_amount = active_user->addDirectDamage(heal_amount);
                    event_handler->displayMsg(opponent_mon_name+"'s Liquid Ooze triggers!");
                    if(actual_damage_amount>0)
                        event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage_amount)+" damage!");
                    if(active_user->isFainted())
                        return;
                }
                break;
            }
            case 100:case 117:{//user faints
                unsigned int max_hp = active_user->getMaxHP();
                active_user->addDirectDamage(max_hp);
                break;
            }
            default: break;
        }
    }
}

void Battle::applyAttackEffect(Attack* attack,BattleActionActor actor){
    // apply move effects
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = getActorBattlerName(actor);
    std::string opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
    MonsterTeam* user_team = getActorTeam(actor);
    MonsterTeam* target_team = getActorTeam(otherBattleActionActor(actor));
    unsigned int effect = attack->getEffectId();
    unsigned int effect_chance = attack->getEffectChance();
    if(active_user->isFainted() || active_target->isFainted())
        return;
    bool effect_is_applied=true;
    if (effect_chance!=ALWAYS_HITS){
        if(active_user->hasAbility(SERENE_GRACE))//serene grace doubles up ,ikelihood of applying effects
            effect_chance*= 2;
        unsigned int random = RNG::getRandomInteger(1,100);
        if(random > effect_chance){
            effect_is_applied = false;
        }
    }
    // SHIELD DUST PROTECTS OPPONENT FROM SECONDARY EFFECTS
    if(active_target->hasAbility(SHIELD_DUST) && 
        attack->getCategory() != STATUS &&
        attack->getEffectTarget() == TARGET_OPPONENT){
        effect_is_applied = false;
    }
    // SHEER FORCE PREVENTS INFLICTING SECONDARY EFFECTS TO OPPONENT
    if(active_user->hasAbility(SHEER_FORCE) &&
        attack->getCategory() != STATUS &&
        attack->getEffectTarget() == TARGET_OPPONENT){
        effect_is_applied = false;
    }
    // SOUNDPROOF PREVENTS SOUND BASED MOVES EFFECTS
    if(active_target->hasAbility(SOUNDPROOF) && 
        attack->isSoundBased() &&
        attack->getEffectTarget() == TARGET_OPPONENT){
        effect_is_applied = false;
    }
    // OVERCOAT PREVENTS POWDER BASED MOVES EFFECTS
    if(active_target->hasAbility(OVERCOAT) && 
        attack->isPowder() &&
        attack->getEffectTarget() == TARGET_OPPONENT){
        effect_is_applied = false;
    }
    if(effect_is_applied){
        switch(effect){
            case 1: { // lower attack by 1 opponent
                active_target->changeAttackModifier(-1);
                break;
            }
            case 46:{ // lower attack by 2 opponent
                active_target->changeAttackModifier(-2);
                break;
            }
            case 2:{// increase Att and SpAtk by 1 user
                active_user->changeAttackModifier(+1);
                active_user->changeSpecialAttackModifier(+1);
                break;
            }
            case 3:{ // Leech Seed
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
            case 4:{//poison opponent
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being poisoned!");
                    if(attack->getCategory() == STATUS)
                        active_user->setLastAttackFailed();
                    break;
                }
                bool result = active_target->setPermanentStatus(POISONED);
                if(!result){
                    if(attack->getCategory() == STATUS){
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
            case 76:{//bad poison opponent
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being badly poisoned!");
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
                if(attack->getType()==GRASS && active_target->hasType(GRASS)){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    if(attack->getCategory() == STATUS)
                        active_user->setLastAttackFailed();
                    break;
                }
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from falling asleep!");
                    if(attack->getCategory() == STATUS)
                        active_user->setLastAttackFailed();
                    break;
                }
                if(isUproar()){
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
                active_target->changeEvasionModifier(-2);
                break;
            }
            case 8:{ // Synthesis
                if(active_user->isAtFullHP()){
                    event_handler->displayMsg(user_mon_name+" is already at full health!");
                }else{
                    unsigned int maxHP = active_user->getMaxHP();
                    unsigned int heal_amount;
                    if(field->getWeather() == CLEAR || thereIsaCloudNine()){
                        heal_amount = max((maxHP+1) / 2,1);
                    }else if(field->getWeather() == RAIN || field->getWeather() == SUN){
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
            case 13:{ // Worry Seed
                if(active_target->hasAbility(INSOMNIA)){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->setAbility(INSOMNIA);
                    event_handler->displayMsg(opponent_mon_name+"'s ability was changed to Insomnia!");
                }
                // neutralizing gas ability may have been changed
                if(thereIsNeutralizingGas()){
                    player_active->neutralizeAbility();
                    opponent_active->neutralizeAbility();
                }else{
                    player_active->cancelAbilityNeutralization();
                    opponent_active->cancelAbilityNeutralization();
                }
                break;
            }
            case 14:case 20:{//burn opponent
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being burned!");
                    if(attack->getCategory() == STATUS)
                        active_user->setLastAttackFailed();
                    break;
                }
                if(active_target->hasType(FIRE) && attack->getType() == FIRE){
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
                active_target->changeAccuracyModifier(-1);
                break;
            }
            case 16:case 67:{ // paralyze opponent - paralysis
                if(active_target->hasType(GRASS) && attack->getType() == GRASS && attack->getCategory() == STATUS){
                    event_handler->displayMsg("It does not affect "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                    break;
                }
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from the paralysis!");
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
                active_target->changeSpeedModifier(-2);
                break;
            }
            case 19:{ // Fire Spin
                if(!active_target->hasVolatileCondition(FIRESPIN)){
                    active_target->addVolatileCondition(FIRESPIN, 5);
                // }else{
                //     event_handler->displayMsg(opponent_mon_name+" is already trapped in a Fire Spin!");
                }
                break;
            }
            case 158:{ // Whirlpool
                if(!active_target->hasVolatileCondition(WHIRLPOOL)){
                    active_target->addVolatileCondition(WHIRLPOOL, 5);
                }
                break;
            }
            case 74:{ // Sand Tomb
                if(!active_target->hasVolatileCondition(SANDTOMB)){
                    active_target->addVolatileCondition(SANDTOMB, 5);
                }
                break;
            }
            case 21:case 45:case 105:case 141:case 195:case 197:{//flinch
                active_target->addVolatileCondition(FLINCH, 2);
                break;
            }
            case 22:{ //-1 opponent Def
                active_target->changeDefenseModifier(-1);
                break;
            }
            case 23:{ //+1 user Def
                active_user->changeDefenseModifier(+1);
                break;
            }
            case 29:{//+2 Def user
                active_user->changeDefenseModifier(+2);
                break;
            }
            case 24:{// rapid spin
                active_user->changeSpeedModifier(+1);
                field->clearFieldEffectsSuchThat(&isFieldEffectClearedByRapidSpin,PLAYER);
                active_user->clearVolatilesSuchThat(&isVolatileConditionClearedByRapidSpin);
                break;
            }
            case 25:case 77:case 113:case 174:{ // confuse target
                if(active_target->hasVolatileCondition(CONFUSION)){
                    if(attack->getCategory()==STATUS){
                        event_handler->displayMsg(opponent_mon_name+" is already confused!");
                        active_user->setLastAttackFailed();
                    }
                }else if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                        !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being confused!");
                    if(attack->getCategory() == STATUS)
                        active_user->setLastAttackFailed();
                }else if(field->getTerrain() == MISTY_FIELD){
                    if(attack->getCategory() == STATUS && effect==25){
                        event_handler->displayMsg("But it failed!");
                        active_user->setLastAttackFailed(); 
                    }
                }else{  
                    active_target->addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
                }
                if(effect==72)//flatter
                    active_target->changeSpecialAttackModifier(1);
                if(effect==113)//swagger
                    active_target->changeAttackModifier(2);
                break;
            }
            case 26:{//PROTECT
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
            case 27:{ // start rain
                if(field->getWeather()==RAIN || thereIsaCloudNine()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();   
                }else
                    field->setWeather(RAIN,5);
                break;
            }
            case 28:{ // +2 ATT SPATT SPD, -1 DEF SPDEF
                active_user->changeAttackModifier(+2);
                active_user->changeSpecialAttackModifier(+2);
                active_user->changeSpeedModifier(+2);
                active_user->changeDefenseModifierForced(-1);
                active_user->changeSpecialDefenseModifierForced(-1);
                break;
            }
            case 30:{// + 1 SPEED USER
                active_user->changeSpeedModifier(+1);
                break;
            }
            case 31:{// -1 SPDEF opponent
                active_target->changeSpecialDefenseModifier(-1);
                break;
            }
            case 54:{//-2 SPDEF opponent
                active_target->changeSpecialDefenseModifier(-2);
                break;
            }
            case 32:{// whirlwind/roar/dragontail
                if(! target_team->hasAliveBackup()){
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
                    active_target = new Battler(target_team->getActiveMonster(),field,otherBattleActionActor(actor),event_handler);
                    if(actor == PLAYER){
                        opponent_active = active_target;
                    }else{
                        player_active = active_target;
                    }
                    opponent_mon_name = getActorBattlerName(otherBattleActionActor(actor));
                    event_handler->displayMsg(opponent_mon_name+" was forced in!");
                    if(thereIsNeutralizingGas()){
                        player_active->neutralizeAbility();
                        opponent_active->neutralizeAbility();
                    }else{
                        player_active->cancelAbilityNeutralization();
                        opponent_active->cancelAbilityNeutralization();
                    }
                    removeVolatilesFromOpponentOfMonsterLeavingField(otherBattleActionActor(actor));
                    applyImpostorSwitchIn(otherBattleActionActor(actor));
                    applySwitchInAbilitiesEffects(otherBattleActionActor(actor));
                    performEntryHazardCheck(otherBattleActionActor(actor));
                    checkUproars();
                    if(active_target->isFainted())
                        return;
                }
                break;
            }
            case 33:{// SAFEGUARD
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
                if(field->hasFieldEffect(TAILWIND,actor)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    field->setFieldEffect(TAILWIND, 5, actor);
                    event_handler->displayMsg("A strong wind starts to blow on "+user_mon_name+"'s side!");
                }
                break;
            }
            case 35:{ // user + 1 spatt spdef speed
                active_user->changeSpecialAttackModifier(+1);
                active_user->changeSpecialDefenseModifier(+1);
                active_user->changeSpeedModifier(+1);
                break;
            }
            case 38:{// laser focus effect (crit next turn)
                active_user->addVolatileCondition(LASER_FOCUS, 5);
                break;
            }
            case 39:{//focus energy
                if(active_user->hasVolatileCondition(FOCUS_ENERGY)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_user->addVolatileCondition(FOCUS_ENERGY, -1);
                }
                break;
            }
            case 41:{//Toxic spikes
                if(field->hasFieldEffect(BAD_TOXIC_SPIKES,otherBattleActionActor(actor))){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else if(field->hasFieldEffect(TOXIC_SPIKES,otherBattleActionActor(actor))){
                    field->setFieldEffect(BAD_TOXIC_SPIKES, -1, otherBattleActionActor(actor));
                    field->clearFieldEffect(TOXIC_SPIKES,otherBattleActionActor(actor));
                    event_handler->displayMsg(user_mon_name+" scattered toxic spikes towards its opponent!");
                }else{
                    field->setFieldEffect(TOXIC_SPIKES, -1, otherBattleActionActor(actor));
                    event_handler->displayMsg(user_mon_name+" scattered toxic spikes towards its opponent!");
                }
                break;
            }
            case 159:{
                //spikes
                if(field->hasFieldEffect(SPIKES_3,otherBattleActionActor(actor))){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else if(field->hasFieldEffect(SPIKES_2,otherBattleActionActor(actor))){
                    field->setFieldEffect(SPIKES_3, -1, otherBattleActionActor(actor));
                    field->clearFieldEffect(SPIKES_2,otherBattleActionActor(actor));
                    event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
                }else if(field->hasFieldEffect(SPIKES,otherBattleActionActor(actor))){
                    field->setFieldEffect(SPIKES_2, -1, otherBattleActionActor(actor));
                    field->clearFieldEffect(SPIKES,otherBattleActionActor(actor));
                    event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
                }else{
                    field->setFieldEffect(SPIKES, -1, otherBattleActionActor(actor));
                    event_handler->displayMsg(user_mon_name+" scattered spikes towards its opponent!");
                }
                break;
            }
            case 42:{// +2 speed user
                active_user->changeSpeedModifier(+2);
                break;
            }
            case 47:{//roost
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
                active_user->changeAttackModifier(+2);
                break;
            }
            case 53:{ // Wrap
                if(!active_target->hasVolatileCondition(WRAP)){
                    active_target->addVolatileCondition(WRAP, 5);
                }
                break;
            }
            case 164:{ // Bind
                if(!active_target->hasVolatileCondition(BIND)){
                    active_target->addVolatileCondition(BIND, (RNG::coinFlip()?4:5));
                }
                break;
            }
            case 55:{ // stockpile
                if(active_user->getStockpiles() == 3){
                    event_handler->displayMsg(user_mon_name+" cannot stockpile anymore!");
                    active_user->setLastAttackFailed();
                }else{
                    event_handler->displayMsg(user_mon_name+" is stockpiling!");
                    active_user->incrementStockpiles();
                    // active_user->changeDefenseModifier(+1);
                    // active_user->changeSpecialDefenseModifier(+1);
                }
                break;
            }
            case 56:{//56 and 57 are moves related to stockpile, 
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
                active_user->resetStockpiles();
                break;
            }
            case 58:{ // suppress ability
                if(active_target->isAbilitySuppressed()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    event_handler->displayMsg(opponent_mon_name+"'s ability was suppressed!");
                    active_target->suppressAbility();
                }
                //neutralizing gas may have been suppressed
                if(thereIsNeutralizingGas()){
                    player_active->neutralizeAbility();
                    opponent_active->neutralizeAbility();
                }else{
                    player_active->cancelAbilityNeutralization();
                    opponent_active->cancelAbilityNeutralization();
                }
                break;
            }
            case 59:{// clear stat changes
                active_user->resetAllStatChanges();
                active_target->resetAllStatChanges();
                event_handler->displayMsg(user_mon_name+"'s haze cleared all stat changes!");
                break;
            }
            case 60:{// +1 ATT DEF ACC user
                active_user->changeAttackModifier(+1);
                active_user->changeDefenseModifier(+1);
                active_user->changeAccuracyModifier(+1);
                break;
            }
            case 61:{//freeze opponent
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+" from being frozen!");
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
                active_user->changeSpecialAttackModifier(+2);
                break;
            }
            case 63:{//+1 evasiveness user
                active_user->changeEvasionModifier(+1);
                break;
            }
            case 66:{//light screen
                if(field->hasFieldEffect(LIGHT_SCREEN,actor)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    field->setFieldEffect(LIGHT_SCREEN, 5, actor);
                    event_handler->displayMsg(user_mon_name+" set up a light screen that reduces damage coming from special attacks!");
                }
                break;
            }
            case 69:{//-1 speed opponent
                active_target->changeSpeedModifier(-1);
                break;
            }
            case 72:{//start sandstorm
                if(field->getWeather()==SANDSTORM || thereIsaCloudNine()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else
                    field->setWeather(SANDSTORM,5);
                break;
            }
            case 78:{// -1 ATT DEF user
                active_user->changeAttackModifierForced(-1);
                active_user->changeDefenseModifierForced(-1);
                break;
            }
            case 79:{//nothing happens
                event_handler->displayMsg("But nothing happens!");
                break;
            }
            case 82:{//encore
                if(active_target->hasVolatileCondition(ENCORE) || 
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
            case 83:{//heal 25%
                if(active_user->isAtFullHP()){
                    event_handler->displayMsg(user_mon_name+" is already at full health!");
                }else{
                    unsigned int maxHP = active_user->getMaxHP();
                    unsigned int heal_amount = max((maxHP+3) / 4,1);
                    unsigned int actual_heal_amount = active_user->removeDamage(heal_amount);
                    if(actual_heal_amount>0)
                        event_handler->displayMsg(user_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP to itself!");
                }
                break;
            }
            case 85:{//GRAVITY
                if(!field->setFullFieldEffect(GRAVITY,5)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            case 86:{// +1 ATT user
                active_user->changeAttackModifier(+1);
                break;
            }
            case 87:{// +1 DEF SPDEF user
                active_user->changeDefenseModifier(+1);
                active_user->changeSpecialDefenseModifier(+1);
                break;
            }
            case 88:{// -1 SPATT user
                active_user->changeSpecialAttackModifierForced(-1);
                break;
            }
            case 89:{// user dies entry Monster is fully healed
                if(!  user_team->hasAliveBackup()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    // faint active user
                    unsigned int maxHP_old = active_user->getMaxHP();
                    unsigned int actual_damage = active_user->addDirectDamage(maxHP_old);
                    event_handler->displayMsg(user_mon_name+" inflicted to itself "+std::to_string(actual_damage)+" damage!");
                    delete active_user;
                    field->clearFieldEffectsSuchThat(&isFieldEffectTrapping,actor);
                    if(actor == PLAYER){
                        unsigned int new_active_index = event_handler->chooseSwitchForced(player_team);
                        player_team->swapActiveMonster(new_active_index);
                        player_active = new Battler(player_team->getActiveMonster(),field,actor,event_handler);
                        active_user = player_active;
                        event_handler->displayMsg("Player switched in "+player_active->getNickname());
                    }else{
                        unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
                        opponent_team->swapActiveMonster(new_active_index);
                        opponent_active = new Battler(player_team->getActiveMonster(),field,actor,event_handler);
                        active_user = opponent_active;
                        event_handler->displayMsg("Opponent switched in "+opponent_active->getNickname());
                    }
                    if(thereIsNeutralizingGas()){
                        player_active->neutralizeAbility();
                        opponent_active->neutralizeAbility();
                    }else{
                        player_active->cancelAbilityNeutralization();
                        opponent_active->cancelAbilityNeutralization();
                    }
                    removeVolatilesFromOpponentOfMonsterLeavingField(actor);
                    // heal active user
                    user_mon_name = getActorBattlerName(actor);
                    unsigned int maxHP = active_user->getMaxHP();
                    active_user->removeDamage(maxHP);
                    active_user->clearPermanentStatus();
                    event_handler->displayMsg(user_mon_name+" was fully healed!");
                    applyImpostorSwitchIn(actor);
                    applySwitchInAbilitiesEffects(actor);
                    performEntryHazardCheck(actor);
                    checkUproars();
                    if(active_user->isFainted())
                        return;
                }
                break;
            }
            case 90:{// disable last move used by opponenet
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
            case 91:{// remove 4 PP from opponents last move used
                Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
                if(last_attack == nullptr || ! active_target->hasPP(last_attack->getId())){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->usePP(last_attack->getId(),4);
                    event_handler->displayMsg(opponent_mon_name+"'s "+last_attack->getName()+" lost some PP!");
                }
                break;
            }
            case 92:{//imprison
                if(active_user->hasVolatileCondition(IMPRISON)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_user->addVolatileCondition(IMPRISON, -1);
                }
                break;
            }
            case 93:{//Mimic
                if(!active_user->setMimickedAttack(active_target->getLastAttackUsed())){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    Attack* last_attack = Attack::getAttack(active_target->getLastAttackUsed());
                    event_handler->displayMsg(user_mon_name+" mimicked "+opponent_mon_name+"'s "+last_attack->getName()+"!");
                }
                break;
            }
            case 95:{//rest
                if(isUproar()){
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
                if(active_target->hasVolatileCondition(MEAN_LOOK) || active_target->hasType(GHOST)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->addVolatileCondition(MEAN_LOOK, -1);
                }
                break;
            }
            case 98:{//quick guard
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
                if(field->getTerrain() == GRASSY_FIELD)
                    event_handler->displayMsg("But it failed!");
                else
                    field->setTerrain(GRASSY_FIELD);
                break;
            }
            case 102:{//transform
                if(active_target->isFainted()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    break;
                }
                active_user->transformInto(active_target->getMonster());
                event_handler->displayMsg(user_mon_name+" transformed into "+opponent_mon_name+"!");
                break;
            }
            case 104:{//tri attack
                if(!active_target->hasPermanentStatus()){
                    if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
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
                if(!active_target->hasVolatileCondition(TAUNTED))
                    event_handler->displayMsg(opponent_mon_name+" was taunted!");
                else{
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            case 107:{//turn opponent into water type
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
                active_user->changeSpecialDefenseModifier(+2);
                break;
            }
            case 110:{//wonder room
                if(field->hasFullFieldEffect(WONDER_ROOM)){
                    field->clearFullFieldEffect(WONDER_ROOM);
                }else{
                    field->setFullFieldEffect(WONDER_ROOM, 5);
                }
                break;
            }
            case 115:{
                //-1 def spdef user
                active_user->changeDefenseModifierForced(-1);
                active_user->changeSpecialDefenseModifierForced(-1);
                break;
            }
            case 121:{
                //user halves its HP and then maximizes its attack
                unsigned int maxHP = active_user->getMaxHP();
                unsigned int currentHP = active_user->getCurrentHP();
                unsigned int half_hp = max(1,maxHP/2);
                if(currentHP <= half_hp){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_user->addDirectDamage(half_hp);
                    event_handler->displayMsg(user_mon_name+" halved its HP!");
                    active_user->changeAttackModifier(12);
                }
                break;
            }
            case 122:{
                //-1 speed opponent
                active_target->changeSpeedModifier(-1);
                break;
            }
            case 123:{
                // target is forced to switch
                if(!target_team->hasAliveBackup() && attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    break;
                }
                //force opponent to switch
                forceSwitch(otherBattleActionActor(actor));
                active_target = getActorBattler(otherBattleActionActor(actor));
                checkUproars();
                if(active_target->isFainted())
                    return;
                break;
            }
            case 124:{
                //user switches out, fails if trapped
                if((!user_team->hasAliveBackup() || !active_user->canSwitchOut(active_target)) && 
                    attack->getCategory() == STATUS){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    break;
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
                    field->setFieldEffect(REFLECT, 5, actor);
                    event_handler->displayMsg(user_mon_name+" sets up a barrier that reduces damage coming from physical attacks!");
                }
                break;
            }
            case 126:{
                //heal 50% HP
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
                if(active_user->getAbility() == active_target->getAbility() || active_target->hasAbility(NO_ABILITY)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    event_handler->displayMsg(user_mon_name+" copied "+opponent_mon_name+"'s ability!");
                    active_user->setAbility(active_target->getAbility());
                }
                break;
            }
            case 129:{
                //+1 def spdef user
                active_user->changeDefenseModifier(+1);
                active_user->changeSpecialDefenseModifier(+1);
                break;
            }
            case 130:{
                //schedule future sight
                Attack* future_sight = Attack::getAttack(FUTURE_SIGHT_ID);
                unsigned int attack_stat = future_sight->getCategory()==PHYSICAL ?
                    active_user->getModifiedAttack() : active_user->getModifiedSpecialAttack();
                scheduled_futuresights.push_back(
                    ScheduledFutureSight(
                        3,
                        attack_stat,
                        active_user->getLevel(),
                        otherBattleActionActor(actor),
                        active_user->hasType(future_sight->getType())
                    ));
                event_handler->displayMsg(user_mon_name+" has foreseen an attack!");
                break;
            }
            case 132:{
                //+1 att def user
                active_user->changeAttackModifier(+1);
                active_user->changeDefenseModifier(+1);
                break;
            }
            case 133:{
                // -2 SPATT user
                active_user->changeSpecialAttackModifierForced(-2);
                break;
            }
            case 137:{
                //user becomes same type as target
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
                if(field->hasFieldEffect(STEALTH_ROCKS,otherBattleActionActor(actor))){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    field->setFieldEffect(STEALTH_ROCKS, -1, otherBattleActionActor(actor));
                    event_handler->displayMsg("Pointed rocks float in the air at "+opponent_mon_name+"'s side!");
                }
            }
            case 142:{
                //+1 spd user
                active_user->changeSpeedModifier(+1);
                break;
            }
            case 143:{
                //opponent drowsy
                if(field->hasFieldEffect(SAFEGUARD,otherBattleActionActor(actor)) &&
                    !active_user->hasAbility(INFILTRATOR)){
                    event_handler->displayMsg("Safeguard protects "+opponent_mon_name+"!");
                    active_user->setLastAttackFailed();
                }else if(!active_target->canFallAsleep() ||
                    active_target->hasVolatileCondition(DROWSY)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->addVolatileCondition(DROWSY, 2);
                }
                break;
            }
            case 144:{
                //curse
                if(active_user->hasType(GHOST)){
                    unsigned int maxHP = active_user->getMaxHP();
                    unsigned int actual_damage = active_user->addDirectDamage(maxHP / 2);
                    event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage to send a curse to its opponent!");
                    if(active_target->hasVolatileCondition(CURSED)){
                        event_handler->displayMsg("But it failed!");
                        active_user->setLastAttackFailed();
                    }else{
                        active_target->addVolatileCondition(CURSED, -1);
                    }
                }else{
                    active_user->changeSpeedModifierForced(-1);
                    active_user->changeAttackModifier(+1);
                    active_user->changeDefenseModifier(+1);
                }
                break;
            }
            case 145:{
                //heal 50% target
                if(active_target->isAtFullHP()){
                    event_handler->displayMsg(opponent_mon_name+" is already at full health!");
                    active_user->setLastAttackFailed();
                }else{
                    unsigned int maxHP = active_target->getMaxHP();
                    unsigned int heal_amount = max((maxHP+1) / 2,1);
                    unsigned int actual_heal_amount = active_target->removeDamage(heal_amount);
                    if(actual_heal_amount>0)
                        event_handler->displayMsg(opponent_mon_name+" was healed by "+std::to_string(actual_heal_amount)+" HP!");
                }
                break;
            }
            case 146:{
                //infatuation opponent
                Gender user_gender = active_user->getGender();
                Gender target_gender = active_target->getGender();
                if(active_target->hasAbility(OBLIVIOUS) ||
                    !areMaleAndFemale(user_gender,target_gender) ||
                    active_target->hasVolatileCondition(INFATUATION)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->addVolatileCondition(INFATUATION, -1);
                }
                break;
            }
            case 147:{
                //lock on target
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
                if(field->getTerrain() == ELECTRIC_FIELD)
                    event_handler->displayMsg("But it failed!");
                else
                    field->setTerrain(ELECTRIC_FIELD);
                break;
            }
            case 149:{
                //magnet rise
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
            }
            case 151:{
                //uproar
                event_handler->displayMsg(user_mon_name+" is making an uproar!");
                uproaring_monsters.insert(active_user->getMonster());
                break;
            }
            case 152:{
                //raise a random stat by +2
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
                switch(chosen_option){
                    case 1:{
                        active_user->changeAttackModifier(+2);
                        break;
                    }
                    case 2:{
                        active_user->changeSpecialAttackModifier(+2);
                        break;
                    }
                    case 3:{
                        active_user->changeDefenseModifier(+2);
                        break;
                    }
                    case 4:{
                        active_user->changeSpecialDefenseModifier(+2);
                        break;
                    }
                    case 5:{
                        active_user->changeSpeedModifier(+2);
                        break;
                    }
                    case 6:{
                        active_user->changeAccuracyModifier(+2);
                        break;
                    }
                    case 7:{
                        active_user->changeEvasionModifier(+2);
                        break;
                    }
                    default:break;
                }
                break;
            }
            case 153:{
                //start snowstorm
                if(field->getWeather() == SNOWSTORM || thereIsaCloudNine()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else
                    field->setWeather(SNOWSTORM,5);
                break;
            }
            case 154:{
                //aqua ring
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
                if(active_target->hasVolatileCondition(PROTECT)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                    break;
                }
                //user faints
                active_user->addDirectDamage(active_user->getMaxHP());
                // opponent stats are lowered
                active_target->changeAttackModifier(-12);
                active_target->changeSpecialAttackModifier(-12);
                break;
            }
            case 161:{
                //destiny bond
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
                active_user->changeSpeedModifierForced(-1);
                break;
            }
            case 167:{
                // charge user, +1 spder user
                if(active_user->hasVolatileCondition(CHARGED)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_user->addVolatileCondition(CHARGED, -1);
                    active_user->changeSpecialDefenseModifier(+1);
                }
            }
            case 169:{
                // +1 SPATT user
                active_user->changeSpecialAttackModifier(+1);
                break;
            }
            case 170:{
                // +1 DEF SPDEF user iff ability is PLUS or MINUS
                if(active_user->hasAbility(PLUS) || 
                    active_user->hasAbility(MINUS)){
                    active_user->changeDefenseModifier(+1);
                    active_user->changeSpecialDefenseModifier(+1);
                }else{
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }
                break;
            }
            case 173:{
                //user endures all hits this turn
                active_user->addVolatileCondition(ENDURE,5);
                break;
            }
            case 177:{
                //reset target stat changes
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
                active_user->changeAttackModifier(+1);
                active_user->changeDefenseModifier(+1);
                active_user->changeSpecialAttackModifier(1);
                active_user->changeSpecialDefenseModifier(1);
                active_user->changeSpeedModifier(1);
                break;
            }
            case 181:{
                //-1 att def target
                active_target->changeAttackModifier(-1);
                active_target->changeDefenseModifier(-1);
                break;
            }
            case 182:{
                //+1 att speed user
                active_user->changeAttackModifier(+1);
                active_user->changeSpeedModifier(+1);
                break;
            }
            case 183:{
                //switch att spatt with opponent
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
                    player_active->setMonster(player_team->getActiveMonster());
                }else{
                    unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
                    opponent_team->swapActiveMonster(new_active_index);
                    opponent_active->setMonster(opponent_team->getActiveMonster());
                }
                if(thereIsNeutralizingGas()){
                    player_active->neutralizeAbility();
                    opponent_active->neutralizeAbility();
                }else{
                    player_active->cancelAbilityNeutralization();
                    opponent_active->cancelAbilityNeutralization();
                }
                removeVolatilesFromOpponentOfMonsterLeavingField(actor);
                applyImpostorSwitchIn(actor);
                applySwitchInAbilitiesEffects(actor);
                performEntryHazardCheck(actor);
                checkUproars();
                break;
            }
            case 187:{
                //user needs to recharge next turn
                active_user->addVolatileCondition(RECHARGING, -1);
                break;
            }
            case 188:{
                //set sun
                if(field->getWeather() == SUN || thereIsaCloudNine()){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    field->setWeather(SUN,5);
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
            }
            case 191:{
                //user change type to first attack type
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
                Type attack_type = opponent_last_attack->getType();
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
                if(active_target->hasVolatileCondition(BLOCKED)){
                    event_handler->displayMsg("But it failed!");
                    active_user->setLastAttackFailed();
                }else{
                    active_target->addVolatileCondition(BLOCKED, 5);
                }
            }
            default:break;
        }
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
    active_user->removeVolatileCondition(FLINCH);
}

void Battle::forgetMoveVolatiles(Battler* active_user){
    active_user->removeVolatileCondition(PETAL_DANCING);
    active_user->removeVolatileCondition(UPROARING);
    uproaring_monsters.erase(active_user->getMonster());
    active_user->removeVolatileCondition(THRASHING);
    active_user->removeVolatileCondition(OUTRAGING);
    active_user->removeVolatileCondition(CHARGING_SOLARBEAM);
    active_user->removeVolatileCondition(FLYING_HIGH);
    active_user->removeVolatileCondition(UNDERGROUND);
    active_user->removeVolatileCondition(ROLLINGOUT);
    active_user->removeVolatileCondition(FLINCH);
    active_user->removeVolatileCondition(LASER_FOCUS);
}

void Battle::performSwitch(BattleAction action){
    std::string old_active_monster_name;
    std::string new_active_monster_name;
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
        player_active = new Battler(player_team->getActiveMonster(),field,PLAYER,event_handler);
        new_active_monster_name = player_active->getNickname();
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
        opponent_active = new Battler(opponent_team->getActiveMonster(),field,OPPONENT,event_handler);
        new_active_monster_name = opponent_active->getNickname();
        event_handler->displayMsg("Opponent switched out " + old_active_monster_name + " and sent " + new_active_monster_name);
    }
    if(thereIsNeutralizingGas()){
        player_active->neutralizeAbility();
        opponent_active->neutralizeAbility();
    }else{
        player_active->cancelAbilityNeutralization();
        opponent_active->cancelAbilityNeutralization();
    }
    removeVolatilesFromOpponentOfMonsterLeavingField(action.getActor());
    applyImpostorSwitchIn(action.getActor());
    applySwitchInAbilitiesEffects(action.getActor());
    performEntryHazardCheck(action.getActor());
    checkUproars();
}

unsigned int Battle::computeDamage(unsigned int attack_id, BattleActionActor user, bool critical_hit, bool attack_after_target){
    Attack* attack = Attack::getAttack(attack_id);
    if(attack->getCategory() == STATUS){
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
    //check immunity
    bool can_hit_ghosts = (user_monster->hasAbility(SCRAPPY));
    float effectiveness = getTypeEffectiveness(attack->getType(), 
        enemy_monster->getTypes(),
        enemy_monster->isTouchingGround(),
        can_hit_ghosts,
        attack->getEffectId() == 196
    );
    if(effectiveness == 0)
        return 0;//immunities
    // check for sheer cold immunity
    if(effect == 156 && enemy_monster->hasType(ICE)){
        return 0;
    }
    // check for soundproof immunity
    if(attack->isSoundBased() && 
        attack->getCategory() != STATUS &&
        enemy_monster->hasAbility(SOUNDPROOF)){
        return 0;
    }
    // check for overcoat immunity
    if(attack->isPowder() && 
        attack->getCategory() != STATUS &&
        enemy_monster->hasAbility(OVERCOAT)){
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
    if(user_monster->hasAbility(UNAWARE)){//UNAWARE uses base stats instead of modified
        physical_defense_stat = enemy_monster->getStats().getDef();
        special_defense_stat = enemy_monster->getStats().getSpdef();
    }else{
        physical_defense_stat = enemy_monster->getModifiedDefense();
        special_defense_stat = enemy_monster->getModifiedSpecialDefense();
    }

    if(field->getWeather() == SNOWSTORM && enemy_monster->hasType(ICE)){
        //ice types increase their defense stat under snowstorm
        physical_defense_stat *= 1.5;
    }
        
    if(attack->getCategory() == PHYSICAL){
        attack_stat = physical_attack_stat;
        if(field->hasFullFieldEffect(WONDER_ROOM))
            defense_stat = special_defense_stat;
        else
            defense_stat = physical_defense_stat;   
        if(user_monster->isBurned() && !user_monster->hasAbility(GUTS)){
            burn_multiplier *= 0.5;
        }
    }else{
        attack_stat = special_attack_stat;
        if((field->hasFullFieldEffect(WONDER_ROOM) && (effect != 128)) ||
            (!field->hasFullFieldEffect(WONDER_ROOM) && (effect == 128)))
            defense_stat = physical_defense_stat;
        else
            defense_stat = special_defense_stat; 
        if(user_monster->isBurned() && !user_monster->hasAbility(GUTS)){
            burn_multiplier *= 0.5;
        }
    }
    if(attack->getEffectId()==100){
        //self destructing moves halve enemy defense stat
        defense_stat *= 0.5;
    }
    if((attack->getType()==FIRE || attack->getType()==ICE) &&
        enemy_monster->hasAbility(THICK_FAT)){
        //THICK FAT reduces damage from fire and ice moves by 50%
        attack_stat *= 0.5;
    }
    
    double base_power = computePower(attack,user, attack_after_target);
    // compute modifiers
    unsigned int level = user_monster->getLevel();
    float stab_multiplier = 1;
    if(user_monster->hasType(attack->getType())){
        if(user_monster->hasAbility(ADAPTABILITY))//adaptability boosts STAB
            stab_multiplier = 2;
        else
            stab_multiplier = 1.5;
    }
    float weather_multiplier = 1;
    if(field->getWeather() == RAIN && !thereIsaCloudNine()){
        if(attack->getType() == WATER)
            weather_multiplier = 1.5;
        else if(attack->getType() == FIRE)
            weather_multiplier = 0.5;
    }else if(field->getWeather() == SUN && !thereIsaCloudNine()){
        if(attack->getType() == FIRE)
            weather_multiplier = 1.5;
        else if(attack->getType() == WATER)
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
    if(effect==172){
        field->clearFieldEffect(REFLECT,otherBattleActionActor(user));
        field->clearFieldEffect(LIGHT_SCREEN,otherBattleActionActor(user));
    }
    //apply field effects
    if(field->hasFieldEffect(LIGHT_SCREEN, otherBattleActionActor(user)) && 
        attack->getCategory()==SPECIAL &&
        !user_monster->hasAbility(INFILTRATOR)){
        damage -= damage * 2732.0 / 4096;
    }
    if(field->hasFieldEffect(REFLECT, otherBattleActionActor(user)) && 
        attack->getCategory()==PHYSICAL  &&
        !user_monster->hasAbility(INFILTRATOR)){
        damage -= damage * 2732.0 / 4096;
    }
    // apply solar beam modifiers
    if((effect == 9)&&
        !thereIsaCloudNine() &&
        (field->getWeather() == HAIL || 
        field->getWeather() == SANDSTORM || field->getWeather()==RAIN)){
            damage *= 0.5;
    }
    // some moves have damage halved under Grassy Terrain
    if(field->getTerrain() == GRASSY_FIELD && (effect == 69 || effect == 73)){
        damage *= 0.5;
    }
    // dry skin opponents take 25% more damage
    if(enemy_monster->hasAbility(DRY_SKIN) && attack->getType() == FIRE){
        damage *= 1.25;
    }
    // filter reduces damage from supereffective attacks
    if(effectiveness>1.1 && enemy_monster->hasAbility(FILTER)){
        damage *= 3.0 / 4.0;
    }

    if(attack->getEffectId() == 112){
        // damage equal to level
        unsigned int level = user_monster->getLevel();
        damage = level;
    }
    if(attack->getEffectId() == 117){
        // damage equal to currentHP user
        unsigned int current_hp = user_monster->getCurrentHP();
        damage = current_hp;
    }

    int integer_damage = (int)damage;
    if(integer_damage < 1)
        integer_damage = 1;
    return integer_damage;
}

double Battle::computePower(Attack*attack,BattleActionActor actor,bool attack_after_target){
    Battler* active_user = getActorBattler(actor);
    Battler* active_target = getActorBattler(otherBattleActionActor(actor));
    unsigned int effect = attack->getEffectId();
    unsigned int attack_id = attack->getId();
    // get base power
    double base_power = attack->getPower();

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
        case 45:{
            if(active_target->hasVolatileCondition(FLYING_HIGH)){
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
        case 134:{
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
        default: break;
    }

    // ability modifiers
    switch(active_user->getAbility()){
        case OVERGROW:{
            if(attack->getType() == GRASS && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case BLAZE:{
            if(attack->getType() == FIRE && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case TORRENT:{
            if(attack->getType() == WATER && 
                active_user->getCurrentHP() <= active_user->getMaxHP() / 3){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case SWARM:{
            if(attack->getType() == BUG && 
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
            if(attack->getType() == FIRE && active_user->hasVolatileCondition(FLASH_FIRED)){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case SAND_FORCE:{
            if(field->getWeather() == SANDSTORM &&
                !thereIsaCloudNine() && (
                attack->getType()==STEEL ||
                attack->getType()==GROUND ||
                attack->getType()==ROCK))
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
                effect==51 || effect==6 || effect==174)
                base_power *= 1.2;
            break;
        }
        case IRON_FIST:{
            //boost power of puncihng attacks
            if(attack->isPunching())
                base_power*=1.2;
            break;
        }
        default: break;
    }

    //terrain modifiers
    switch(field->getTerrain()){
        case GRASSY_FIELD:{
            if(attack->getType() == GRASS && active_user->isTouchingGround()){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case MISTY_FIELD:{
            if(attack->getType() == DRAGON){
                base_power *= 0.5;
            }
            break;
        }
        case ELECTRIC_FIELD:{
            if(attack->getType() == ELECTRIC && active_user->isTouchingGround()){
                base_power *= 3.0 / 2;
            }
            break;
        }
        case PSYCHIC_FIELD:{
            if(attack->getType() == PSYCHIC && active_user->isTouchingGround()){
                base_power *= 3.0 / 2;
            }
            break;
        }
        default: break;
    }

    //volatile conditions modifiers
    if(active_user->hasVolatileCondition(CHARGED_2)){
        if(attack->getType() == ELECTRIC){
            base_power *= 2;
        }
    }

    return base_power;
}

void Battle::applyPostDamage(){
    unsigned int player_active_speed = player_active->getModifiedSpeed();
    unsigned int opponent_active_speed = opponent_active->getModifiedSpeed();
    std::vector<BattleActionActor> actors;
    if(player_active_speed > opponent_active_speed){
        actors.push_back(PLAYER);
        actors.push_back(OPPONENT);
    }else if(player_active_speed < opponent_active_speed){
        actors.push_back(OPPONENT);
        actors.push_back(PLAYER);
    }else{
        if(RNG::coinFlip()){
            actors.push_back(PLAYER);
            actors.push_back(OPPONENT);
        }else{
            actors.push_back(OPPONENT);
            actors.push_back(PLAYER);
        }
    }
    // update field
    field->nextTurnWeather();
    // apply weather
    for(auto actor: actors){
        applyWeatherPostDamage(actor);
        if(isOver()){
            return;
        }
    }
    field->nextTurnTerrain();
    // apply terrain effects
    for(auto actor: actors){
        applyTerrainPostDamage(actor);
        if(isOver()){
            return;
        }
    }
    // apply permanent
    for(auto actor: actors){
        applyPermanentStatusPostDamage(actor);
        if(isOver()){
            return;
        }
    }
    // apply volatile
    for(auto actor: actors){
        applyVolatileStatusPostDamage(actor);
        if(isOver()){
            return;
        }
    }

    
    for(auto actor: actors){
        applyFieldEffectsPostDamage(actor);
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
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_burn_damage)+" burn damage!");
            if(active_user->isFainted())
                return;
            break;
        }
        case POISONED:{
            unsigned int psn_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_psn_damage = active_user->addDirectDamage(psn_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_psn_damage)+" poison damage!");
            if(active_user->isFainted())
                return;
            break;
        }
        case BAD_POISON:{
            unsigned int badpsn_damage = max(active_user->getMaxHP() * active_user->getBadPoisonCounter() / 16,1);
            unsigned int actual_badpsn_damage = active_user->addDirectDamage(badpsn_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_badpsn_damage)+" poison damage!");
            if(active_user->isFainted())
                return;
            active_user->increaseBadPoisonCounter();
            break;
        }
        default: break;
    }
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
        unsigned int actual_aqua_ring_heal = active_user->removeDamage(aqua_ring_heal);
        if(actual_aqua_ring_heal>0)
            event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_aqua_ring_heal)+" HP thanks to Aqua Ring!");
    }
    //apply ingrain heal
    if(active_user->hasVolatileCondition(INGRAINED)){
        unsigned int ingrain_heal = max((active_user->getMaxHP()+15) / 16,1);
        unsigned int actual_ingrain_heal = active_user->removeDamage(ingrain_heal);
        if(actual_ingrain_heal>0)
            event_handler->displayMsg(mon_name+" healed "+std::to_string(actual_ingrain_heal)+" HP thanks to Ingrain!");
    }
    // apply leech seed
    if(!active_user->hasAbility(MAGIC_GUARD)){//magic guard prevents effect damage
        if(active_user->hasVolatileCondition(LEECH_SEED) && !active_opponent->isFainted()){
            unsigned int leech_seed_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_leech_seed_damage = active_user->addDirectDamage(leech_seed_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_leech_seed_damage)+" leech seed damage!");
            if(active_user->hasAbility(LIQUID_OOZE)){// if user has liquid ooze, user takes dmg instead of recovering health
                event_handler->displayMsg(opponent_name+" is hurt by Liquid Ooze!");
                unsigned int liquid_ooze_damage = actual_leech_seed_damage;
                unsigned int actual_liquid_ooze_damage = active_opponent->addDirectDamage(liquid_ooze_damage);
                event_handler->displayMsg(opponent_name+" took "+std::to_string(actual_liquid_ooze_damage)+" Liquid Ooze damage!");
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
            unsigned int wrap_damage = max(active_user->getMaxHP() / 16,1);
            unsigned int actual_wrap_damage = active_user->addDirectDamage(wrap_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_wrap_damage)+" Wrap damage!");
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
            unsigned int bind_damage = max(active_user->getMaxHP() / 16,1);
            unsigned int actual_bind_damage = active_user->addDirectDamage(bind_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_bind_damage)+" Bind damage!");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(BIND);
        }else{
            active_user->removeVolatileCondition(BIND);
        }
        if(active_user->isFainted()){return;}
        //fire spin
        if(active_user->hasVolatileCondition(FIRESPIN)){
            event_handler->displayMsg(mon_name+" is trapped in a Fire Spin!");
            unsigned int fire_spin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_fire_spin_damage = active_user->addDirectDamage(fire_spin_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_fire_spin_damage)+" fire spin damage!");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(FIRESPIN);
        }
        if(active_user->isFainted()){return;}
        //whirlpool
        if(active_user->hasVolatileCondition(WHIRLPOOL)){
            event_handler->displayMsg(mon_name+" is trapped in a Whirlpool!");
            unsigned int fire_spin_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_fire_spin_damage = active_user->addDirectDamage(fire_spin_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_fire_spin_damage)+" Whirlpool damage!");
            if(active_user->isFainted())
                return;
            active_user->decrementVolatileCondition(WHIRLPOOL);
        }
        if(active_user->isFainted()){return;}
        //sand tomb
        if(active_user->hasVolatileCondition(SANDTOMB)){
            event_handler->displayMsg(mon_name+" is trapped in a vortex of sand!");
            unsigned int sand_damage = max(active_user->getMaxHP() / 8,1);
            unsigned int actual_sand_damage = active_user->addDirectDamage(sand_damage);
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_sand_damage)+" sand tomb damage!");
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
            event_handler->displayMsg(mon_name+" took "+std::to_string(actual_curse_damage)+" curse damage!");
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
    // remove protect
    if(active_user->hasVolatileCondition(PROTECT)){
        active_user->removeVolatileCondition(PROTECT);
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
    if(isUproar()){
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
            if(!(active_user->hasType(ROCK) || active_user->hasType(GROUND) || 
            active_user->hasType(STEEL) || 
            active_user->hasAbility(SAND_RUSH) ||
            active_user->hasAbility(SAND_VEIL) ||
            active_user->hasAbility(MAGIC_GUARD) ||
            active_user->hasAbility(OVERCOAT) ||
            active_user->hasAbility(SAND_FORCE))){
                event_handler->displayMsg(mon_name+" is buffeted by the sandstorm!");
                unsigned int sandstorm_damage = max(active_user->getMaxHP() / 16,1);
                unsigned int actual_sandstorm_damage = active_user->addDirectDamage(sandstorm_damage);
                event_handler->displayMsg(mon_name+" took "+std::to_string(actual_sandstorm_damage)+" sandstorm damage!");
                if(active_user->isFainted())
                    return;
            }
            break;
        }
        case HAIL:{
            if(!(active_user->hasType(ICE)||
                active_user->hasAbility(OVERCOAT) ||
                active_user->hasAbility(ICE_BODY) ||
                active_user->hasAbility(SNOW_CLOAK) ||
                active_user->hasAbility(MAGIC_GUARD))){
                event_handler->displayMsg(mon_name+" is buffeted by the hail!");
                unsigned int hail_damage = max(active_user->getMaxHP() / 16,1);
                unsigned int actual_hail_damage = active_user->addDirectDamage(hail_damage);
                event_handler->displayMsg(mon_name+" took "+std::to_string(actual_hail_damage)+" hail damage!");
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
            if (active_user->hasAbility(SOLAR_POWER) || active_user->hasAbility(DRY_SKIN)){ // SOLAR POWER DAMAGE
                std::string ability_name = abilityToString(active_user->getAbility());
                event_handler->displayMsg(mon_name+" is losing HP due to "+ability_name+"!");
                unsigned int solar_power_damage = max(active_user->getMaxHP()+7 / 8,1);
                unsigned int actual_solar_power_damage = active_user->addDirectDamage(solar_power_damage);
                event_handler->displayMsg(mon_name+" took "+std::to_string(actual_solar_power_damage)+" "+ability_name+" damage!");
                if(active_user->isFainted())
                    return;
            }
            break;
        }
        case RAIN:{
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
}

void Battle::applyFieldEffectsPostDamage(BattleActionActor actor){
    Battler* active_user=getActorBattler(actor);
    if(active_user->isFainted())
        return;
    // std::string mon_name = getActorBattlerName(actor);
    // apply field effects
    // std::set<FieldEffect> field_effects = field->getFieldEffects(actor);
    // for(auto it = field_effects.begin(); it != field_effects.end(); it++){
    //     FieldEffect field_effect = *it;
    //     switch(field_effect){
    //         default:break;
    //     }
    // }   
}


void Battle::performEntryHazardCheck(BattleActionActor actor){
    Battler* active_battler = getActorBattler(actor);
    if(active_battler->isFainted())
        return;
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
            event_handler->displayMsg(battler_name+" took "+std::to_string(actual_spikes_damage)+" damage from the Spikes!");
        }
    }
    if(active_battler->isFainted())
        return;
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
            event_handler->displayMsg(battler_name+" took "+std::to_string(actual_stealth_rock_damage)+" damage from the Stealth Rocks!");
        }
    }
}

void Battle::applySwitchInAbilitiesEffects(BattleActionActor actor){
    Battler * user_active = getActorBattler(actor);
    Battler * target_active = getActorBattler(otherBattleActionActor(actor));
    std::string user_name = getActorBattlerName(actor);
    std::string other_name = getActorBattlerName(otherBattleActionActor(actor));
    if(user_active->isFainted())
        return;

    Ability user_ability = user_active->getAbility();
    //TRACE may also activbate other abilities once an ability is traced
    if(user_ability == TRACE){
        Ability target_ability = target_active->getAbility();
        if(isAbilityTraceable(target_ability)){
            user_active->setAbility(target_ability);
            event_handler->displayMsg(user_name+" traced "+other_name+"'s "+abilityToString(target_ability)+"!");
        }
    }

    user_ability = user_active->getAbility();
    switch(user_ability){
        case INTIMIDATE:{
            if(!target_active->isFainted() && 
                !doesAbilityIgnoreIntimidate(target_active->getAbility())){//inner focus, oblivious and scrappy give immunity to intimidate
                event_handler->displayMsg(user_name+" intimidates its opponent!");
                target_active->changeAttackModifier(-1);
            }
            break;
        }
        // case UNNERVE:{
        //     event_handler->displayMsg(user_name+" is nervous and won't eat berries!");
        //     break;
        // }
        case DROUGHT:{
            if(field->getWeather() != SUN && !thereIsaCloudNine()){
                event_handler->displayMsg(user_name+"'s Drought made the sun shine harshly!");
                field->setWeather(SUN,5);
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
                if(target_special_defense < target_defense){
                    user_active->changeSpecialAttackModifier(1);
                }else{
                    user_active->changeAttackModifier(1);
                }
            }
            break;
        }
        
        default:break;
    }
}

void Battle::applyImpostorSwitchIn(BattleActionActor actor){
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
        applyImpostorSwitchIn(actor);
        applySwitchInAbilitiesEffects(actor);
        return;
    }
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

void Battle::applyContactEffects(Attack * attack, BattleActionActor actor){
    Battler * active_user = getActorBattler(actor);
    Battler * active_target = getActorBattler(otherBattleActionActor(actor));
    std::string user_mon_name = active_user->getNickname();
    std::string opponent_mon_name = active_target->getNickname();
    if(!attack->makesContact()){
        return;
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

    // STATIC ability effect
    if(active_target->hasAbility(STATIC) && 
        active_user->canBeParalyzed() &&
        RNG::getRandomInteger(0,2)==0){
        event_handler->displayMsg(opponent_mon_name+"'s Static triggers!");
        active_user->setPermanentStatus(PARALYSIS);
        if(active_user->hasAbility(SYNCHRONIZE) && 
            !active_target->hasPermanentStatus()){
            event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
            active_target->setPermanentStatus(PARALYSIS);
        }
    }
    // POISON POINT ability effect
    if(active_target->hasAbility(POISON_POINT) && 
        active_user->canBePoisoned() &&
        RNG::getRandomInteger(0,2)==0){
        event_handler->displayMsg(opponent_mon_name+"'s Poison Point triggers!");
        active_user->setPermanentStatus(POISONED);
        if(active_user->hasAbility(SYNCHRONIZE) && 
            !active_target->hasPermanentStatus()){
            event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
            active_target->setPermanentStatus(POISONED);
        }
    }
    // FLAME BODY ability effect
    if(active_target->hasAbility(FLAME_BODY) && 
        active_user->canBeBurned() &&
        RNG::getRandomInteger(0,2)==0){
        event_handler->displayMsg(opponent_mon_name+"'s Flame Body triggers!");
        active_user->setPermanentStatus(BURN);
        if(active_user->hasAbility(SYNCHRONIZE) && 
            !active_target->hasPermanentStatus()){
            event_handler->displayMsg(user_mon_name+"'s Synchronize triggers!");
            active_target->setPermanentStatus(BURN);
        }
    }
    // EFFECT SPORE ability effect
    if(active_target->hasAbility(EFFECT_SPORE) && 
        !active_user->hasType(GRASS) &&
        !active_user->hasAbility(OVERCOAT)){
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
    // CURSED BODY
    if(active_target->hasAbility(CURSED_BODY) && RNG::getRandomInteger(1,100)<=30){
        unsigned int last_attack_used_id =active_user->getLastAttackUsed();
        Attack * last_attack_used = Attack::getAttack(last_attack_used_id);
        if(last_attack_used_id!=0 && 
            last_attack_used_id!=STRUGGLE_ID && 
            !active_user->hasDiabledAttack()){
            event_handler->displayMsg(user_mon_name+"'s "+last_attack_used->getName()+" was disabled by "+opponent_mon_name+"'s Cursed Body!");
            active_user->disableAttack(last_attack_used_id,5);
        }
    }
    // check if target is dead
    if(active_target->isFainted()){
        return;
    }
    // CUTE CHARM ability effect
    Gender active_user_gender = active_user->getGender();
    Gender active_target_gender = active_target->getGender();
    bool can_attract = areMaleAndFemale(active_user_gender,active_target_gender) && !active_user->hasAbility(OBLIVIOUS);
    if(active_target->hasAbility(CUTE_CHARM) && can_attract
        && RNG::getRandomInteger(0,2)==0){
        active_user->addVolatileCondition(INFATUATION, 5);
    }
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
    // check if gravity blocks attack
    if(field->hasFullFieldEffect(GRAVITY) && attack->getType()==FLYING){
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
            if(attack->getEffectId()==100){
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
        attack->getPriorityLevel() < action.getPriority()){
        attack_failed = true;
    }
    // check if move fails due to imprison
    if((active_user->hasVolatileCondition(IMPRISON))&&
        active_target->knowsAttack(action.getAttackId())){ 
        attack_failed = true;
    }
    // check if OHKO fails
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
        case 49:{//sucker punch
            if(!isAttackingActionType(other_action.getActionType()) || // opponent is not attacking
                action.getOrder() > other_action.getOrder() || // opponent action comes before
                Attack::getAttack(other_action.getAttackId())->getCategory()==STATUS){ // opponent is using status move
                attack_failed = true;
            }
            break;
        }
        case 105:{//fake out
            if(!active_user->isFirstTurn()){
                active_user->setLastAttackUsed(action.getAttackId());
                last_attack_used_id = attack_id;
                attack_failed = true;
            }
            break;
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
        }
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
        if(field->getWeather() != SUN || thereIsaCloudNine()){
            active_user->setLastAttackUsed(action.getAttackId());
            last_attack_used_id = attack_id;
            active_user->removeVolatileCondition(LASER_FOCUS);
            return true;
        }
    }
    if(active_user->hasVolatileCondition(CHARGING_SOLARBEAM)){
        active_user->removeVolatileCondition(CHARGING_SOLARBEAM);
    }
    //charge sky attack
    if(attack->getEffectId() == 9 && !active_user->hasVolatileCondition(CHARGING_SKYATTACK)){// Solar beam
        active_user->addVolatileCondition(CHARGING_SKYATTACK, 5);
        active_user->setLastAttackUsed(action.getAttackId());
        last_attack_used_id = attack_id;
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }
    if(active_user->hasVolatileCondition(CHARGING_SKYATTACK)){
        active_user->removeVolatileCondition(CHARGING_SKYATTACK);
    }
    //charge fly
    if(attack->getEffectId() == 48 && !active_user->hasVolatileCondition(FLYING_HIGH)// Fly
        && !field->hasFullFieldEffect(GRAVITY)){// gravity will cause the attack to fail since fly is flying type
        active_user->addVolatileCondition(FLYING_HIGH, 5);
        active_user->removeVolatileCondition(LASER_FOCUS);
        return true;
    }
    if(active_user->hasVolatileCondition(FLYING_HIGH)){
        active_user->removeVolatileCondition(FLYING_HIGH);
    }
    //charge dig
    if(attack->getEffectId() == 48 && !active_user->hasVolatileCondition(UNDERGROUND)){// Dig
        active_user->addVolatileCondition(UNDERGROUND, 5);
        active_user->removeVolatileCondition(UNDERGROUND);
        return true;
    }
    if(active_user->hasVolatileCondition(UNDERGROUND)){
        active_user->removeVolatileCondition(UNDERGROUND);
    }
    //charge dive
    if(attack->getEffectId() == 136 && !active_user->hasVolatileCondition(UNDERWATER)){// Dive
        active_user->addVolatileCondition(UNDERWATER, 5);
        active_user->removeVolatileCondition(UNDERWATER);
        return true;
    }
    if(active_user->hasVolatileCondition(UNDERWATER)){
        active_user->removeVolatileCondition(UNDERWATER);
    }
    // moves that lock user in using them for a while
    if(attack->getEffectId() == 10 && !active_user->hasVolatileCondition(PETAL_DANCING)){
        active_user->addVolatileCondition(PETAL_DANCING, RNG::getRandomInteger(2,3));
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
    if(attack->getEffectId()==65){
        active_target->removeVolatileCondition(PROTECT);
    }
    if(active_target->hasVolatileCondition(PROTECT) && 
        attack->getTarget()==TARGET_OPPONENT){
        event_handler->displayMsg(opponent_mon_name+" protected itself!");
        // active_target->removeVolatileCondition(PROTECT);
        decrementVolatiles(active_user);
        active_user->setLastAttackHit();
        if(attack->getEffectId()==100){
            //user dies
            active_user->addDirectDamage(active_user->getMaxHP());
        }
        if(attack->getEffectId()==187){
            //user dies
            active_user->addVolatileCondition(RECHARGING,-1);
        }
        if(attack->getEffectId() == 171 || attack->getEffectId() == 174){
            // user takes 50% maxHP recoil damage
            unsigned int max_hp = active_user->getMaxHP();
            unsigned int damage = max_hp / 2;
            unsigned int actual_damage = active_user->addDirectDamage(damage);
            event_handler->displayMsg(user_mon_name+" took "+std::to_string(actual_damage)+" damage from recoil!"); 
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
        if(attack->getEffectId()==100){
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
        attack->getEffectId() == 100 &&
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
        attack->getType()==ELECTRIC &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        active_target->changeSpecialAttackModifier(1);
        attack_absorbed = true;
    }
    // flash fire -> drew in fire type attack and get flash fired volatile
    if(active_target->hasAbility(FLASH_FIRE) && 
        attack->getType()==FIRE &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT){
        event_handler->displayMsg(opponent_mon_name+" drew in the attack!");
        active_target->addVolatileCondition(FLASH_FIRED, -1);
        attack_absorbed = true;
    }
    // dry skin and water absorb prevent water type moves from hitting and restores 25% of max HP
    unsigned int heal_amount = max((active_target->getMaxHP() + 3)/ 4,1);
    unsigned int actual_heal_amount = 0;
    if((active_target->hasAbility(DRY_SKIN) ||
        active_target->hasAbility(WATER_ABSORB)) && 
        attack->getType()==WATER &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT){
        event_handler->displayMsg(opponent_mon_name+" absorbed the water attack!");
        actual_heal_amount = active_target->removeDamage(heal_amount);
        event_handler->displayMsg(opponent_mon_name+" healed "+std::to_string(actual_heal_amount)+" HP!");
        attack_absorbed = true;
    }
    // volt absorb prevents electric type moves from hitting and restores 25% of max HP
    if(active_target->hasAbility(VOLT_ABSORB) && 
        attack->getType()==ELECTRIC &&
        attack->getCategory()!=STATUS && 
        attack->getTarget()==TARGET_OPPONENT){
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
        if(attack->getEffectId()==100){
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
    return false;
}

void Battle::removeVolatilesFromOpponentOfMonsterLeavingField(BattleActionActor actor_switching_out){
    Battler* opponent_active_monster = getActorBattler(otherBattleActionActor(actor_switching_out));
    opponent_active_monster->removeVolatileCondition(INFATUATION);
    opponent_active_monster->removeVolatileCondition(WRAP);
    opponent_active_monster->removeVolatileCondition(BIND);
    opponent_active_monster->removeVolatileCondition(IMPRISON);
}

void Battle::forceSwitch(BattleActionActor actor_switching_out){
    Battler* old_active = getActorBattler(actor_switching_out);
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
        player_active = new Battler(player_team->getActiveMonster(),field,PLAYER,event_handler);
        event_handler->displayMsg("Player switched in "+player_active->getNickname());
    }else{
        unsigned int new_active_index = cpu_ai->chooseSwitch(opponent_active,opponent_team,player_active,field);
        opponent_team->swapActiveMonster(new_active_index);
        delete opponent_active;
        opponent_active = new Battler(opponent_team->getActiveMonster(),field,OPPONENT,event_handler);
        event_handler->displayMsg("Opponent switched in "+opponent_active->getNickname());
    }
    if(thereIsNeutralizingGas()){
        player_active->neutralizeAbility();
        opponent_active->neutralizeAbility();
    }else{
        player_active->cancelAbilityNeutralization();
        opponent_active->cancelAbilityNeutralization();
    }
    removeVolatilesFromOpponentOfMonsterLeavingField(actor_switching_out);
    applyImpostorSwitchIn(actor_switching_out);
    applySwitchInAbilitiesEffects(actor_switching_out);
    performEntryHazardCheck(actor_switching_out);
    checkUproars();
}

void Battle::applyScheduledFutureSights(){
    std::vector<unsigned int> to_remove;
    for(unsigned int i = 0; i < scheduled_futuresights.size(); i++){
        ScheduledFutureSight& it = scheduled_futuresights[i];
        it.turns_left--;
        if(it.turns_left == 0){
            to_remove.push_back(i);
            Battler* target_active = getActorBattler(it.target);
            std::string target_name = getActorBattlerName(it.target);
            event_handler->displayMsg("Future Sight hits "+target_name+"!");
            if(target_active->isFainted()){
                event_handler->displayMsg("But it failed!");
                continue;
            }
            float effectiveness = getTypeEffectiveness(PSYCHIC, 
                target_active->getTypes(), 
                target_active->isTouchingGround(),
                false,
                false);
            if(effectiveness == 0){
                event_handler->displayMsg("But it failed!");
                continue;
            }
            Attack* attack = Attack::getAttack(FUTURE_SIGHT_ID);
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
            unsigned int final_damage = max(base_dmg * effectiveness * crit_multiplier * stab_multiplier,1);
            unsigned int actual_damage = target_active->addDamage(final_damage, attack->getCategory());
            event_handler->displayMsg(target_name+" took "+std::to_string(actual_damage)+" damage!");
        }

    }
    // clear the scheduled future sights that were executed this turn from the end
    for(int i = to_remove.size()-1; i >= 0; i--){
        scheduled_futuresights.erase(scheduled_futuresights.begin()+to_remove[i]);
    }
}

bool Battle::thereIsNeutralizingGas(){
    return (player_active->hasAbility(NEUTRALIZING_GAS) && !player_active->isFainted()) || 
        (opponent_active->hasAbility(NEUTRALIZING_GAS) && !opponent_active->isFainted());
}