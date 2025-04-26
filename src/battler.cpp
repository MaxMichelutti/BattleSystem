#include "battler.h"

// Messages ------------------------------------------------------------------------------------------

std::string cannotFallMsg(std::string nickname, std::string stat_name){
    return nickname+"'s "+stat_name+" won't go any lower!";
}
std::string cannotRaiseMsg(std::string nickname, std::string stat_name){
    return nickname+"'s "+stat_name+" won't go any higher!";
}
std::string statRaiseMsg(std::string nickname, std::string stat_name, unsigned int severity){
    if(severity==1){
        return nickname+"'s "+stat_name+" rose!";
    }else if(severity==2){
        return nickname+"'s "+stat_name+" rose sharply!";
    }else{
        return nickname+"'s "+stat_name+" drastically rose!";
    }
}
std::string statFallMsg(std::string nickname, std::string stat_name, unsigned int severity){
    if(severity==1){
        return nickname+"'s "+stat_name+" fell!";
    }else if(severity==2){
        return nickname+"'s "+stat_name+" harshly fell!";
    }else{
        return nickname+"'s "+stat_name+" severely fell!";
    }
} 

// VolatileCondition ------------------------------------------------------------------------------------------------------------

bool isVolatileConditionClearedByRapidSpin(VolatileStatusCondition status) {
    switch (status) {
        case LEECH_SEED:
        case WRAP:
        case BIND:
        case FIRESPIN:
        case INFESTED:
        case SANDTOMB:
        case WHIRLPOOL:
            return true;
        default:
            return false;
    }
}

// Modifiers ------------------------------------------------------------------------------------------------------------

Modifiers::Modifiers() {
    attack = 0;
    defense = 0;
    special_attack = 0;
    special_defense = 0;
    speed = 0;
    accuracy = 0;
    evasion = 0;
}

Modifiers::Modifiers(unsigned int attack, unsigned int defense, unsigned int special_attack, unsigned int special_defense, unsigned int speed, unsigned int accuracy, unsigned int evasion) {
    this->attack = attack;
    this->defense = defense;
    this->special_attack = special_attack;
    this->special_defense = special_defense;
    this->speed = speed;
    this->accuracy = accuracy;
    this->evasion = evasion;
}

Modifiers::~Modifiers(){}

unsigned int Modifiers::getAttack()const {
    return attack;
}
unsigned int Modifiers::getDefense()const {
    return defense;
}
unsigned int Modifiers::getSpecialAttack()const {
    return special_attack;
}
unsigned int Modifiers::getSpecialDefense()const {
    return special_defense;
}
unsigned int Modifiers::getSpeed()const {
    return speed;
}
unsigned int Modifiers::getAccuracy()const {
    return accuracy;
}
unsigned int Modifiers::getEvasion()const {
    return evasion;
}

bool Modifiers::setAtk(int amount){
    attack = amount;
    return true;
}
bool Modifiers::setDef(int amount){
    defense = amount;
    return true;
}
bool Modifiers::setSpatk(int amount){
    special_attack = amount;
    return true;
}
bool Modifiers::setSpdef(int amount){
    special_defense = amount;
    return true;
}
bool Modifiers::setSpd(int amount){
    speed = amount;
    return true;
}
bool Modifiers::setAccuracy(int amount){
    accuracy = amount;
    return true;
}
bool Modifiers::setEvasion(int amount){
    evasion = amount;
    return true;
}

bool Modifiers::changeAtk(int amount) {
    if (amount > 0) {
        if(attack == MAX_MODIFIER)
            return false;
        if (attack + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - attack;
        attack += amount;
        return true;
    } else if (amount < 0) {
        if(attack == MIN_MODIFIER)
            return false;
        if (attack + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - attack;
        attack += amount;
        return true;
    }
    return false;
}

bool Modifiers::changeDef(int amount) {
    if (amount > 0) {
        if(defense == MAX_MODIFIER)
            return false;
        if (defense + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - defense;
        defense += amount;
        return true;
    } else if (amount < 0) {
        if(defense == MIN_MODIFIER)
            return false;
        if (defense + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - defense;
        defense += amount;
        return true;
    }
    return false;
}
bool Modifiers::changeSpatk(int amount) {
    if (amount > 0) {
        if(special_attack == MAX_MODIFIER)
            return false;
        if (special_attack + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - special_attack;
        special_attack += amount;
        return true;
    } else if (amount < 0) {
        if(special_attack == MIN_MODIFIER)
            return false;
        if (special_attack + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - special_attack;
        special_attack += amount;
        return true;
    }
    return false;
}

bool Modifiers::changeSpdef(int amount) {
    if (amount > 0) {
        if(special_defense == MAX_MODIFIER)
            return false;
        if (special_defense + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - special_defense;
        special_defense += amount;
        return true;
    } else if (amount < 0) {
        if(special_defense == MIN_MODIFIER)
            return false;
        if (special_defense + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - special_defense;
        special_defense += amount;
        return true;
    }
    return false;
}

bool Modifiers::changeSpd(int amount) {
    if (amount > 0) {
        if(speed == MAX_MODIFIER)
            return false;
        if (speed + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - speed;
        speed += amount;
        return true;
    } else if (amount < 0) {
        if(speed == MIN_MODIFIER)
            return false;
        if (speed + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - speed;
        speed += amount;
        return true;
    }
    return false;
}

bool Modifiers::changeAccuracy(int amount) {
    if (amount > 0) {
        if(accuracy == MAX_MODIFIER)
            return false;
        if (accuracy + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - accuracy;
        accuracy += amount;
        return true;
    } else if (amount < 0) {
        if(accuracy == MIN_MODIFIER)
            return false;
        if (accuracy + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - accuracy;
        accuracy += amount;
        return true;
    }
    return false;
}

bool Modifiers::changeEvasion(int amount) {
    if (amount > 0) {
        if(evasion == MAX_MODIFIER)
            return false;
        if (evasion + amount > MAX_MODIFIER)
            amount = MAX_MODIFIER - evasion;
        evasion += amount;
        return true;
    } else if (amount < 0) {
        if(evasion == MIN_MODIFIER)
            return false;
        if (evasion + amount < MIN_MODIFIER)
            amount = MIN_MODIFIER - evasion;
        evasion += amount;
        return true;
    }
    return false;
}

// Battler ------------------------------------------------------------------------------------------------------------

Battler::Battler() {
    monster = nullptr;
}

Battler::Battler(Monster* monster, Field*field,BattleActionActor actor,EventHandler* handler) {
    this->monster = monster;
    bad_poison_counter = 0;
    if(monster->getPermanentStatus() == BAD_POISON){
        bad_poison_counter = 1;
    }
    this->battler_ability = monster->getAbility();
    this->actor = actor;
    this->field = field;
    this->types.insert(monster->getType1());
    if(monster->getType2()!=NO_TYPE){
        this->types.insert(monster->getType2());
    }
    this->handler = handler;
    consecutive_protects_counter=0;
    bad_poison_counter=0;
    has_ability_suppressed = false;
    has_ability_neutralized = false;
    stockpiles = 0;
    last_attack_used = 0;
    same_attack_counter = 0;
    had_held_item = false;
    stat_modifiers = Modifiers(0,0,0,0,0,0,0);
    mimicked_attack = nullptr;
    disabled_attack_id = 0;
    disabled_turns_left = 0;
    turns_in_battle = 0;
    last_attack_failed = false;
    had_flying_type = false;
    hits_taken = 0;
    resetDamageTakenThisTurn();
    is_mold_breaker_active = false;
    weight = monster->getWeight();
    height = monster->getHeight();
    battle_stats = monster->getStats();
}

void Battler::setMonster(Monster* monster){
    mimicked_attack = nullptr;
    this->monster = monster;
    consecutive_protects_counter = 0;
    last_attack_used = 0;
    same_attack_counter = 0;
    attacks_used.clear();
    stockpiles = 0;
    bad_poison_counter = 0;
    has_ability_neutralized = false;
    disabled_attack_id = 0;
    disabled_turns_left = 0;
    turns_in_battle = 0;
    last_attack_failed = false;
    hits_taken = 0;
    had_held_item = false;
    weight = monster->getWeight();
    height = monster->getHeight();
    battle_stats = monster->getStats();
    // substituteHP = 0;//switching removes the substitute
    resetDamageTakenThisTurn();
    removeVolatileCondition(INFATUATION);
    removeVolatileCondition(PROTECT);
    removeVolatileCondition(CHARGED);
    removeVolatileCondition(CHARGED_2);
    removeVolatileCondition(ENCORE);
    removeVolatileCondition(IMPRISON);
    removeVolatileCondition(QUICK_GUARD);
    removeVolatileCondition(SMACKED_DOWN);
    removeVolatileCondition(UPROARING);
    removeVolatileCondition(JUST_EATEN_BERRY);
    removeVolatileCondition(THROAT_CHOPPED);
    removeVolatileCondition(RECHARGING);
    removeVolatileCondition(TRUANTING);

}

Battler::~Battler() {
    if(mimicked_attack != nullptr)
        delete mimicked_attack;
}

Monster* Battler::getMonster()const {
    return monster;
}

void Battler::addVolatileCondition(VolatileStatusCondition condition, int duration) {
    if(hasVolatileCondition(condition))
        return;
    if(hasAbility(OWN_TEMPO) && condition == CONFUSION){
        handler->displayMsg(getNickname()+"'s Own Tempo prevents confusion!");
        return;
    }
    if(hasAbility(INNER_FOCUS) && condition == FLINCH){
        return;
    }
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD) &&
        condition==DROWSY){
        handler->displayMsg(getNickname()+"'s Leaf Guard prevents drowsiness!");
        return;
    }
    volatile_conditions.insert({condition,duration});
    switch(condition){
        case GROUNDED:{
            if(monster->hasType(FLYING)){
                handler->displayMsg(getNickname()+" lands on the ground!");
            }
            break;
        }
        case FLYING_HIGH:{
            handler->displayMsg(getNickname()+" flies high!");
            break;
        }
        case UNDERGROUND:{
            handler->displayMsg(getNickname()+" digs under ground!");
            break;
        }
        case CHARGING_SKYATTACK:{
            handler->displayMsg(getNickname()+" became cloaked in a harsh light!");
            break;
        }
        case UNDERWATER:{
            handler->displayMsg(getNickname()+" dives deep!");
            break;
        }
        case INFATUATION:{
            handler->displayMsg(getNickname()+" is infatuated!");
            break;
        }
        case CONFUSION:{
            handler->displayMsg(getNickname()+" is confused!");
            break;
        }
        case FIRESPIN:{
            handler->displayMsg(getNickname()+" is trapped in a vortex of fire!");
            break;
        }
        case INFESTED:{
            handler->displayMsg(getNickname()+" is infested!");
            break;
        }
        case WHIRLPOOL:{
            handler->displayMsg(getNickname()+" is trapped in a whirlpool of water!");
            break;
        }
        case SANDTOMB:{
            handler->displayMsg(getNickname()+" is trapped in a vortex of sand!");
            break;
        }
        case WRAP:{
            handler->displayMsg(getNickname()+" is trapped in a wrap!");
            break;
        }
        case BIND:{
            handler->displayMsg(getNickname()+" is binded by the its opponent!");
            break;
        }
        case LEECH_SEED:{
            handler->displayMsg(getNickname()+" is seeded!");
            break;
        }
        case LASER_FOCUS:{
            handler->displayMsg(getNickname()+" is focusing its energy!");
            break;
        }
        case FOCUS_ENERGY:{
            handler->displayMsg(getNickname()+" is getting pumped!");
            break;
        }
        case MEAN_LOOK:
        case BLOCKED:{
            handler->displayMsg(getNickname()+" cannot escape!");
            break;
        }
        case TAUNTED:{
            handler->displayMsg(getNickname()+" is taunted!");
            break;
        }
        case PROTECT:{
            handler->displayMsg(getNickname()+" protects itself!");
            break;
        }
        case IMPRISON:{
            handler->displayMsg(getNickname()+" cannot use attacks known by its opponent!");
            break;
        }
        case CHARGING_SOLARBEAM:
        case CHARGING_SOLARBLADE:{
            handler->displayMsg(getNickname()+" collects power from the sun!");
            break;
        }
        case VANISHED:{
            handler->displayMsg(getNickname()+" vanished!");
            break;
        }
        case DROWSY:{
            handler->displayMsg(getNickname()+" is drowsy!");
            break;
        }
        case CURSED:{
            handler->displayMsg(getNickname()+" was cursed!");
            break;
        }
        case DESTINY_BOND:{
            handler->displayMsg(getNickname()+" tries to knock out whoever knocks it out!");
            break;
        }
        case CHARGED:{
            handler->displayMsg(getNickname()+" is charging its next Electric type attack!");
            break;
        }
        case FOCUSED:{
            handler->displayMsg(getNickname()+" is focusing!");
            break;
        }
        case INGRAINED:{
            handler->displayMsg(getNickname()+" plants its roots on the ground!");
            break;
        }
        default:
            break;
    }
    //eat persim berry in case of confusion
    if(hasVolatileCondition(CONFUSION) && hasHeldItem(PERSIM_BERRY))
        tryEatStatusBerry();
    // try to consume held item
    if((hasVolatileCondition(TAUNTED) || hasVolatileCondition(TORMENTED) ||
        hasVolatileCondition(INFATUATION) || hasVolatileCondition(ENCORE)) && 
        hasHeldItem(MENTAL_HERB))
        consumeHeldItem();
        
}

void Battler::removeVolatileCondition(VolatileStatusCondition condition) {
    if(!hasVolatileCondition(condition))
        return;
    volatile_conditions.erase(condition);
    switch(condition){
        case GROUNDED:
            if(monster->hasType(FLYING)){
                handler->displayMsg(getNickname()+" tries to take flight!");
            }
            break;
        case INFATUATION:
            handler->displayMsg(getNickname()+" is no longer infatuated!");
            break;
        case FIRESPIN:
            handler->displayMsg(getNickname()+" is no longer trapped in a vortex of fire!");
            break;
        case INFESTED:
            handler->displayMsg(getNickname()+" is no longer infested!");
            break;
        case WHIRLPOOL:
            handler->displayMsg(getNickname()+" is no longer trapped in a whirlpool of water!");
            break;
        case SANDTOMB:
            handler->displayMsg(getNickname()+" is no longer trapped in a vortex of sand!");
            break;
        case WRAP:
            handler->displayMsg(getNickname()+" is no longer trapped in a wrap!");
            break;
        case BIND:
            handler->displayMsg(getNickname()+" is no longer binded!");
            break;
        case LEECH_SEED:
            handler->displayMsg(getNickname()+" is no longer seeded!");
            break;
        case TAUNTED:
            handler->displayMsg(getNickname()+" is no longer taunted!");
            break;
        case CONFUSION:
            handler->displayMsg(getNickname()+" snapped out of confusion!");
            break;
        default:
            break;
    }

}

void Battler::decrementVolatileCondition(VolatileStatusCondition condition) {
    auto it = volatile_conditions.find(condition);
    if(it != volatile_conditions.end()){
        if(it->second < 0)
            return;
        it->second--;
        if(it->first == PERISH_SONG)
            handler->displayMsg("The countdown of Perish Song for "+getNickname()+" drops to "+std::to_string(it->second)+"!");
        if(it->second <= 0){
            removeVolatileCondition(condition);
        }
    }
}

void Battler::removeAllVolatileConditions() {
    volatile_conditions.clear();
}

bool Battler::clearVolatilesSuchThat(bool (*func)(VolatileStatusCondition)){
    std::set<VolatileStatusCondition> volatiles_to_remove;
    bool has_cleared=false;
    for(auto it=volatile_conditions.begin(); it!=volatile_conditions.end(); it++){
        VolatileStatusCondition vol = it->first;
        if(func(vol)){
            volatiles_to_remove.insert(vol);
        }
    }
    for(auto it=volatiles_to_remove.begin();it!=volatiles_to_remove.end();it++){
        removeVolatileCondition(*it);
        has_cleared=true;
    }
    return has_cleared;
}

bool Battler::hasVolatilesSuchThat(bool (*func)(VolatileStatusCondition)){
    for(auto it=volatile_conditions.begin(); it!=volatile_conditions.end(); it++){
        VolatileStatusCondition vol = it->first;
        if(func(vol)){
            return true;
        }
    }
    return false;
}

bool Battler::hasVolatileCondition(VolatileStatusCondition condition)const {
    return volatile_conditions.find(condition) != volatile_conditions.end();
}

unsigned int Battler::getConsecutiveProtects()const{
    return consecutive_protects_counter;
}
void Battler::incrementConsecutiveProtect(){
    consecutive_protects_counter++;
}
void Battler::resetConsecutiveProtect(){
    consecutive_protects_counter=0;
}

void Battler::displayStatModifyResult(bool res,int amount,std::string stat_name){
    std::string name;
    if(actor==PLAYER){
        name = "Player's ";
    }else{
        name = "opponent's ";
    }
    name += getNickname();
    if(!res){
        if(amount > 0)
            handler->displayMsg(cannotRaiseMsg(name,stat_name));
        else if(amount < 0)
            handler->displayMsg(cannotFallMsg(name,stat_name));
    }else{
        if(amount > 0)
            handler->displayMsg(statRaiseMsg(name,stat_name,amount));
        else if(amount < 0)
            handler->displayMsg(statFallMsg(name,stat_name,-amount));
    }
}

bool Battler::changeAttackModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    if(hasAbility(HYPER_CUTTER) && amount < 0){
        handler->displayMsg(getNickname()+"'s Hyper Cutter prevents the Attack reduction!");
        return false;
    }
    bool res = stat_modifiers.changeAtk(amount);
    displayStatModifyResult(res,amount,"Attack");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeAttackModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res = stat_modifiers.changeAtk(amount);
    displayStatModifyResult(res,amount,"Attack");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeDefenseModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    std::string name = getNickname();
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(name+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    if(hasAbility(BIG_PECKS) && amount < 0){
        handler->displayMsg(name+"'s Defense cannot be lowered!");
        return false;
    }
    bool res= stat_modifiers.changeDef(amount);
    displayStatModifyResult(res,amount,"Defense");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeDefenseModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res = stat_modifiers.changeDef(amount);
    displayStatModifyResult(res,amount,"Defense");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeSpecialAttackModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    bool res= stat_modifiers.changeSpatk(amount);
    displayStatModifyResult(res,amount,"Special Attack");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}


bool Battler::changeSpecialAttackModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res= stat_modifiers.changeSpatk(amount);
    displayStatModifyResult(res,amount,"Special Attack");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeSpecialDefenseModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    bool res= stat_modifiers.changeSpdef(amount);
    displayStatModifyResult(res,amount,"Special Defense");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeSpecialDefenseModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res= stat_modifiers.changeSpdef(amount);
    displayStatModifyResult(res,amount,"Special Defense");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeSpeedModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    bool res = stat_modifiers.changeSpd(amount);
    displayStatModifyResult(res,amount,"Speed");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeSpeedModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res = stat_modifiers.changeSpd(amount);
    displayStatModifyResult(res,amount,"Speed");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeAccuracyModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    if(hasAbility(ILLUMINATE) && amount<0){
        handler->displayMsg(getNickname()+"'s Accuracy cannot be lowered!");
        return false;
    }
    std::string name = getNickname();
    if(hasAbility(KEEN_EYE) && amount<0){
        handler->displayMsg(name+"'s Accuracy cannot be lowered!");
        return false;
    }
    bool res = stat_modifiers.changeAccuracy(amount);
    displayStatModifyResult(res,amount,"Accuracy");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeAccuracyModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res = stat_modifiers.changeAccuracy(amount);
    displayStatModifyResult(res,amount,"Accuracy");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeEvasionModifier(int amount) {
    // if(hasSubstitute() && !opponent->hasAbility(INFILTRATOR)){
    //     handler->displayMsg(getNickname()+"'s substitute prevents stat changes!");
    //     return false;
    // }
    if(hasAbility(CONTRARY))
        amount = -amount;
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(hasAbility(CLEAR_BODY) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(hasHeldItem(CLEAR_AMULET) && amount < 0){
        handler->displayMsg(getNickname()+"'s Clear amulet prevents Stat reductions!");
        return false;
    }
    bool res = stat_modifiers.changeEvasion(amount);
    displayStatModifyResult(res,amount,"Evasion");
    if(res && amount < 0 && hasAbility(COMPETITIVE)){
        changeSpecialAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpecialAttackModifier(2);
        }
    }
    if(res && amount < 0 && hasAbility(DEFIANT)){
        changeAttackModifier(2);
        if(opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeAttackModifier(2);
        }
    }
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

bool Battler::changeEvasionModifierForced(int amount) {
    if(hasAbility(CONTRARY))
        amount = -amount;
    bool res = stat_modifiers.changeEvasion(amount);
    displayStatModifyResult(res,amount,"Evasion");
    if(res && amount<0){
        addVolatileCondition(STAT_JUST_DROPPED,1);
    }
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    return res;
}

int Battler::getAttackModifier()const {
    return stat_modifiers.getAttack();
}
int Battler::getDefenseModifier()const {
    return stat_modifiers.getDefense();
}
int Battler::getSpecialAttackModifier()const {
    return stat_modifiers.getSpecialAttack();
}
int Battler::getSpecialDefenseModifier()const {
    return stat_modifiers.getSpecialDefense();
}
int Battler::getSpeedModifier()const {
    return stat_modifiers.getSpeed();
}
int Battler::getAccuracyModifier()const {
    return stat_modifiers.getAccuracy();
}
int Battler::getEvasionModifier()const {
    return stat_modifiers.getEvasion();
}
unsigned int Battler::getBadPoisonCounter()const {
    return bad_poison_counter;
}
void Battler::increaseBadPoisonCounter() {
    bad_poison_counter += 1;
}
void Battler::resetBadPoisonCounter() {
    bad_poison_counter = 0;
}

unsigned int Battler::getModifiedAttack()const {
    unsigned int base_attack = battle_stats.getAtk();
    unsigned int base_modified = base_attack;
    int modifier = getAttackModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_attack;
    }else if(modifier<0){
        base_modified = base_attack * 2 / (-modifier + 2);
    }
    if(hasAbility(HUSTLE))
        base_modified*=1.5;
    if(hasAbility(GUTS) && hasPermanentStatus())
        base_modified*=1.5;
    // if(hasAbility(HUGE_POWER))//huge power doubles attack in battle
    //     base_modified*=2;
    if(hasHeldItem(LIGHT_BALL) && monster->getSpeciesId()==25)//light ball doubles attack of pikachu
        base_modified*=2;
    // if(hasHeldItem(CHOICE_BAND))
    //     base_modified*=1.5;
    if(hasHeldItem(THICK_CLUB) &&
        (monster->getSpeciesId()==104 || monster->getSpeciesId()==105 ||//thick club doubles attack of cubone and marowak
        monster->getTransformedSpeciesId()==104 || monster->getTransformedSpeciesId()==105))//and of a Monster transformed into cubone or marowak
        base_modified*=2;
    return base_modified;
}

unsigned int Battler::getModifiedDefense()const{
    if(field->hasFullFieldEffect(WONDER_ROOM))
        return getModifiedSpecialDefenseInternal();
    return getModifiedDefenseInternal();
}

unsigned int Battler::getModifiedSpecialDefense()const{
    if(field->hasFullFieldEffect(WONDER_ROOM))
        return getModifiedDefenseInternal();
    return getModifiedSpecialDefenseInternal();
}

unsigned int Battler::getModifiedDefenseInternal()const {
    unsigned int base_defense = battle_stats.getDef();
    unsigned int base_modified = base_defense;
    int modifier = getDefenseModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_defense;
    }else if(modifier<0){
        base_modified = base_defense * 2 / (-modifier + 2);
    }
    if(hasAbility(MARVEL_SCALE) && hasPermanentStatus()){
        base_modified = base_modified * 3 / 2;
    }
    if(hasHeldItem(EVIOLITE) && monster->hasEvolutions())
        base_modified = base_modified * 1.5;
    if(hasHeldItem(METAL_POWDER)&& monster->getSpeciesId()==132 && !monster->isTransformed())//metal powder oncreases defense of ditto
        base_modified*=1.5;
    return base_modified;
}

unsigned int Battler::getModifiedSpecialAttack()const {
    unsigned int base_special_attack = battle_stats.getSpatk();
    unsigned int base_modified = base_special_attack;
    int modifier = getSpecialAttackModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_special_attack;
    }else if(modifier<0){
        base_modified = base_special_attack * 2 / (-modifier + 2);
    }
    // if(hasAbility(SOLAR_POWER) && field->getWeather() == SUN){// SOLAR POWER
    //     base_modified = base_modified * 3 / 2;
    // }
    if(hasHeldItem(LIGHT_BALL) && monster->getSpeciesId()==25)//light ball doubles special attack of pikachu
        base_modified*=2;
    // if(hasHeldItem(CHOICE_SPECS))
    //     base_modified*=1.5;
    return base_modified;
}

unsigned int Battler::getModifiedSpecialDefenseInternal()const {
    unsigned int base_special_defense = battle_stats.getSpdef();
    unsigned int base_modified = base_special_defense;
    int modifier = getSpecialDefenseModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_special_defense;
    }else if(modifier<0){
        base_modified = base_special_defense * 2 / (-modifier + 2);
    }
    if(field->getWeather() == SANDSTORM && hasType(ROCK)){
        base_modified = base_modified * 1.5;
    }
    if(hasHeldItem(ASSULT_VEST))
        base_modified = base_modified * 1.5;
    if(hasHeldItem(EVIOLITE) && monster->hasEvolutions())
        base_modified = base_modified * 1.5;
    if(hasHeldItem(METAL_POWDER)&& monster->getSpeciesId()==132 && !monster->isTransformed())//metal powder oncreases defense of ditto
        base_modified*=1.5;
    return base_modified;
}

unsigned int Battler::getModifiedSpeed()const {
    unsigned int base_speed = battle_stats.getSpdef();
    unsigned int base_modified = base_speed;
    int modifier = getSpeedModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_speed;
    }else if(modifier<0){
        base_modified = base_speed * 2 / (-modifier + 2);
    }
    if(field->hasFieldEffect(TAILWIND,actor)){//tailwind doubles speed of the team
        base_modified = base_modified * 2;
    }
    if(hasAbility(QUICK_FEET) && hasPermanentStatus()){
        base_modified =  base_modified * 1.5;
    }else if(monster->getPermanentStatus() == PARALYSIS){
        base_modified = base_modified * 0.5;
    }
    if(hasAbility(UNBURDEN) && hasLostHeldItem()){
        base_modified = base_modified * 2;
    }
    if(hasAbility(CHLOROPHYL) && field->getWeather() == SUN){// CHLOROPHYL
        base_modified = base_modified * 2;
    }
    if(hasAbility(SWIFT_SWIM) && field->getWeather() == RAIN){// SWIFT SWIM
        base_modified = base_modified * 2;
    }
    if(hasAbility(SLUSH_RUSH) && 
        (field->getWeather() == SNOWSTORM || field->getWeather()==HAIL)){// SLUSH RUSH
        base_modified = base_modified * 2;
    }
    if(hasAbility(SURGE_SURFER) && field->getTerrain()==ELECTRIC_FIELD){// SURGE SURFER
        base_modified = base_modified * 2;
    }
    if(field->getWeather()==SANDSTORM && hasAbility(SAND_RUSH)){//Sand rush doubles speed under sandstorm
        base_modified = base_modified * 2;
    }
    if(hasHeldItem(POWER_ANKLET))
        base_modified *= 0.5;
    if(hasHeldItem(POWER_BELT))
        base_modified *= 0.5;
    if(hasHeldItem(POWER_BRACER))
        base_modified *= 0.5;
    if(hasHeldItem(POWER_WEIGHT))
        base_modified *= 0.5;
    if(hasHeldItem(POWER_LENS))
        base_modified *= 0.5;
    if(hasHeldItem(POWER_BAND))
        base_modified *= 0.5;
    if(hasHeldItem(CHOICE_SCARF))
        base_modified *= 1.5;
    if(hasHeldItem(IRON_BALL))
        base_modified *= 0.5;
    if(hasHeldItem(QUICK_POWDER)&& monster->getSpeciesId()==132 && !monster->isTransformed())//metal powder increases speed of ditto
        base_modified*=2;
    return base_modified;
}

unsigned int Battler::getModifiedAccuracy()const {
    unsigned int accuracy = 100;
    int modifier = getAccuracyModifier();
    unsigned int modified_accuracy = accuracy;
    if(modifier>0){
        modified_accuracy = accuracy * (modifier + 3)/3;
    }else if(modifier<0){
        modified_accuracy = accuracy * 3 / (-modifier + 3);
    }
    if(hasAbility(COMPOUND_EYES))
        modified_accuracy *= 13 / 10;
    if(hasHeldItem(WIDE_LENS))
        modified_accuracy *= 1.1;
    return modified_accuracy;
}

unsigned int Battler::getModifiedEvasion()const {
    unsigned int evasion = 100;
    int modifier = getEvasionModifier();
    unsigned int modified_evasion = evasion;
    if(modifier>0){
        modified_evasion= evasion * (modifier + 3)/3;
    }else if(modifier<0){
        modified_evasion= evasion * 3 / (-modifier + 3);
    }
    return modified_evasion;
}

void Battler::resetAllStatChanges(){
    stat_modifiers = Modifiers();
    if(hasVolatileCondition(FOCUS_ENERGY))
        removeVolatileCondition(FOCUS_ENERGY);
}

Ability Battler::getAbility()const {
    if(hasHeldItem(ABILITY_SHIELD))
        return battler_ability;
    if(has_ability_neutralized)
        return NO_ABILITY;
    if(has_ability_suppressed)
        return NO_ABILITY;
    if(is_mold_breaker_active && isAbilityIgnorable(battler_ability))
        return NO_ABILITY;
    return battler_ability;
}

bool Battler::hasAbility(Ability ability)const{
    return getAbility() == ability;
}

Ability Battler::getNonNeutralizedAbility()const{
    if(has_ability_suppressed)
        return NO_ABILITY;
    return battler_ability;
}
void Battler::neutralizeAbility(){
    if(getAbility() != NEUTRALIZING_GAS)
        has_ability_neutralized = true;
}
void Battler::cancelAbilityNeutralization(){
    has_ability_neutralized = false;
}

void Battler::setAbility(Ability ability){
    if(hasHeldItem(ABILITY_SHIELD))
        return;
    if(isAbilitySuppressed())
        has_ability_suppressed = false;
    this->battler_ability = ability;
}
bool Battler::canSwitchOut(Battler* enemy)const {
    if(hasAbility(RUN_AWAY))
        return true;
    if(hasHeldItem(SMOKE_BALL))
        return true;
    if(hasVolatileCondition(INGRAINED))
        return false;
    if(hasType(GHOST))
        return true;
    if(enemy != nullptr &&
        enemy->hasAbility(ARENA_TRAP) && 
        isTouchingGround())
        return false;
    if(enemy != nullptr &&
        enemy->hasAbility(MAGNET_PULL) && 
        hasType(STEEL))
        return false;
    if(enemy != nullptr &&
        enemy->hasAbility(SHADOW_TAG) &&
        !hasAbility(SHADOW_TAG))//having shadow tag makes you immune from other shadow tag holders
        return false;
    if(hasVolatileCondition(WRAP))
        return false;
    if(hasVolatileCondition(BIND))
        return false;
    if(hasVolatileCondition(FIRESPIN))
        return false;
    if(hasVolatileCondition(INFESTED))
        return false;
    if(hasVolatileCondition(WHIRLPOOL))
        return false;
    if(hasVolatileCondition(SANDTOMB))
        return false;
    if(hasVolatileCondition(MEAN_LOOK))
        return false;
    if(hasVolatileCondition(BLOCKED))
        return false;
    if(field->hasFieldEffectsSuchThat(&isFieldEffectTrapping,actor))
        return false;
    return true;
}

BattleActionActor Battler::getActor()const{
    return actor;
}

std::set<Type> Battler::getTypes()const{
    return types;
}

void Battler::clearTypes(){
    types.clear();
}

void Battler::changeType(Type type){
    types.clear();
    if(type != NO_TYPE)
        types.insert(type);
}
void Battler::addType(Type type){
    if(type!=NO_TYPE)
        types.insert(type);
}

bool Battler::hasType(Type t)const{
    return types.find(t)!=types.end();
}

unsigned int Battler::getLastAttackUsed()const{
    // 0 means no attack used before
    return last_attack_used;
}
void Battler::setLastAttackUsed(unsigned int attack_id){
    attacks_used.insert(attack_id);
    if(attack_id == STRUGGLE_ID || attack_id == 0){
        last_attack_used = 0;
        same_attack_counter = 0;
        return;
    }
    if(attack_id == last_attack_used){
        same_attack_counter++;
    }else{
        same_attack_counter = 1;
    }
    last_attack_used = attack_id;
}

bool Battler::hasUsedAttack(unsigned int attack_id)const{
    if(attack_id == STRUGGLE_ID || attack_id == 0)
        return false;
    return attacks_used.find(attack_id) != attacks_used.end();
}

bool Battler::hasPreviouslyUsed(unsigned int attack_id)const{
    if(attack_id == STRUGGLE_ID || attack_id == 0)
        return false;
    return attacks_used.find(attack_id) != attacks_used.end();
}

bool Battler::knowsAttack(unsigned int attack_id)const{
    if(attack_id == 0 || attack_id == STRUGGLE_ID)
        return false;
    return monster->hasAttack(attack_id);
}

unsigned int Battler::getSameAttackCounter()const{
    return same_attack_counter;
}

bool Battler::isTouchingGround()const{
    if(field->hasFullFieldEffect(GRAVITY))
        return true;
    if(hasVolatileCondition(GROUNDED))
        return true;
    if(hasVolatileCondition(SMACKED_DOWN))
        return true;
    if(hasVolatileCondition(INGRAINED))
        return true;
    if(hasHeldItem(IRON_BALL))
        return true;
    if(hasHeldItem(AIR_BALLOON))
        return false;
    if(hasVolatileCondition(MAGNET_RISE))
        return false;
    if(hasAbility(LEVITATE))
        return false;
    if(hasType(FLYING))
        return false;
    return true;
}

bool Battler::isAsleep()const{
    if((monster->getPermanentStatus() == SLEEP_1) ||
        (monster->getPermanentStatus() == SLEEP_2) ||
        (monster->getPermanentStatus() == SLEEP_3) ||
        (monster->getPermanentStatus() == SLEEP_4))
        return true;
    return false;
}

bool Battler::canFallAsleep()const{
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(field->getTerrain() == ELECTRIC_FIELD)
        return false;
    if(hasAbility(INSOMNIA))
        return false;
    if(hasAbility(VITAL_SPIRIT))
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasPermanentStatus())
        return false;
    return true;
}

bool Battler::isParalyzed()const{
    if(monster->getPermanentStatus() == PARALYSIS)
        return true;
    return false;
}

bool Battler::canBeParalyzed()const{
    if(hasType(ELECTRIC))
        return false;
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasPermanentStatus())
        return false;
    if(hasAbility(LIMBER))
        return false;
    return true;
}

bool Battler::isFrozen()const{
    if(monster->getPermanentStatus() == FREEZE)
        return true;
    return false;
}

bool Battler::canBeFrozen()const{
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(field->getWeather()==SUN)
        return false;
    if(hasPermanentStatus())
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasAbility(MAGMA_ARMOR))
        return false;
    return true;
}

bool Battler::isBurned()const{
    if(monster->getPermanentStatus() == BURN)
        return true;
    return false;
}

bool Battler::canBeBurned()const{
    if(hasPermanentStatus())
        return false;
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasAbility(WATER_VEIL))
        return false;
    return true;
}

bool Battler::isPoisoned()const{
    if(monster->getPermanentStatus() == POISONED || 
        monster->getPermanentStatus() == BAD_POISON)
        return true;
    return false;
}

bool Battler::canBePoisoned()const{
    if(hasPermanentStatus())
        return false;
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(hasType(POISON) || hasType(STEEL))
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasAbility(IMMUNITY))
        return false;
    if(hasAbility(PASTEL_VEIL))
        return false;
    return true;
}

bool Battler::canBeBadlyPoisoned()const{
    if(field->getTerrain() == MISTY_FIELD)
        return false;
    if(hasPermanentStatus() && monster->getPermanentStatus()!=POISONED)
        return false;
    if(hasType(POISON) || hasType(STEEL))
        return false;
    if(field->getWeather()==SUN && hasAbility(LEAF_GUARD))
        return false;
    if(hasAbility(IMMUNITY))
        return false;
    return true;
}

void Battler::landOnGround(){
    if(hasType(FLYING)){
        types.erase(FLYING);
        had_flying_type = true;
    }
    addVolatileCondition(GROUNDED,5);
}

void Battler::leaveGround(){
    if(!hasVolatileCondition(GROUNDED))
        return;
    removeVolatileCondition(GROUNDED);
    if(had_flying_type){
        types.insert(FLYING);
    }
}

bool Battler::hasPermanentStatus()const{
    return monster->getPermanentStatus()!=NO_PERMANENT_CONDITION;
}

unsigned int Battler::getStockpiles()const{
    return stockpiles;
}

bool Battler::incrementStockpiles(){
    if(stockpiles < 3){
        bool res = changeDefenseModifier(+1);
        res = res || changeSpecialDefenseModifier(+1);
        stockpiles++;
        return res;
    }
    return false;
}

bool Battler::resetStockpiles(){
    if(stockpiles>0){
        bool res = changeDefenseModifier(-stockpiles);
        res = res || changeSpecialDefenseModifier(-stockpiles);
        stockpiles = 0;
        return res;
    }
    stockpiles = 0;
    return false;
}

void Battler::suppressAbility(){
    if(hasHeldItem(ABILITY_SHIELD))
        return;
    has_ability_suppressed=true;
}

bool Battler::isAbilitySuppressed()const{
    return has_ability_suppressed;
}

bool Battler::isFainted()const{
    return monster->isFainted();
}

bool Battler::disableAttack(unsigned int attack_id, unsigned int turns){
    if(attack_id == STRUGGLE_ID || attack_id == 0)
        return false;
    if(hasDiabledAttack()) 
        return false;
    if(turns == 0)
        return false;
    disabled_attack_id = attack_id;
    disabled_turns_left = turns;
    if(hasHeldItem(MENTAL_HERB))
        consumeHeldItem();
    return true;
}
bool Battler::isAttackDisabled(unsigned int attack)const{
    if(disabled_attack_id == attack)
        return true;
    return false;
}
bool Battler::hasDiabledAttack()const{
    return disabled_attack_id != 0;
}
void Battler::decrementDisabledTurns(){
    if(disabled_turns_left > 0){
        disabled_turns_left--;
        if(disabled_turns_left == 0){
            std::string attack_name = Attack::getAttack(disabled_attack_id)->getName();
            disabled_attack_id = 0;
            handler->displayMsg(getNickname()+" can now use "+attack_name+" again!");
        }
    }
}

unsigned int Battler::getDisabledAttack()const{
    return disabled_attack_id;
}

std::map<unsigned int,unsigned int> Battler::getAttacks()const{
    auto monster_attacks = monster->getAttacks();
    auto res = std::map<unsigned int,unsigned int>();
    for(auto it=monster_attacks.begin();it!=monster_attacks.end();it++){
        if(it->first == MIMIC_ID && mimicked_attack != nullptr){
            res.insert({mimicked_attack->attack_id,mimicked_attack->current_pp});
        }else{
            res.insert({it->first,it->second});
        }
    }
    return res;
}

bool Battler::setMimickedAttack(unsigned int attack_id){
    if(attack_id == STRUGGLE_ID || attack_id == 0 || attack_id == MIMIC_ID)
        return false;
    if(mimicked_attack != nullptr)
        return false;
    mimicked_attack = new AttackData(attack_id,5);
    return true;
}

bool Battler::hasMimickedAttack()const{
    return mimicked_attack != nullptr;
}

void Battler::usePP(unsigned int attack_id, unsigned int amount){
    if(attack_id == STRUGGLE_ID)
        return;
    if(mimicked_attack!=nullptr && mimicked_attack->attack_id == attack_id){
        if(mimicked_attack->current_pp < amount)
            amount = mimicked_attack->current_pp;
        mimicked_attack->current_pp -= amount;
        tryEatLeppaBerry(attack_id);
        return;
    }
    monster->usePP(attack_id,amount);
    tryEatLeppaBerry(attack_id);
}

void Battler::tryEatLeppaBerry(unsigned int attack_id){
    if(attack_id==0 || attack_id==STRUGGLE_ID)
        return;
    if(hasVolatileCondition(UNNERVED))
        return;
    if(hasPP(attack_id))
        return;
    if(!hasHeldItem(LEPPA_BERRY))
        return;
    recoverPP(attack_id,10);
    ItemData* item_data = ItemData::getItemData(LEPPA_BERRY);
    Attack* attack_data = Attack::getAttack(attack_id);
    handler->displayMsg(getNickname()+" ate its "+item_data->getName()+"!");
    addVolatileCondition(JUST_EATEN_BERRY,2);
    handler->displayMsg(getNickname()+"'s "+attack_data->getName()+" PP was restored!");
    removeHeldItem();
    monster->setConsumedItem(LEPPA_BERRY);
}

bool Battler::hasPP(unsigned int attack_id)const{
    if(attack_id == STRUGGLE_ID)
        return true;
    if(mimicked_attack!=nullptr && mimicked_attack->attack_id == attack_id){
        if(mimicked_attack->current_pp == 0)
            return false;
        return true;
    }
    return monster->hasPP(attack_id);
}

void Battler::recoverPP(unsigned int attack_id, unsigned int amount){
    if(mimicked_attack!=nullptr && mimicked_attack->attack_id == attack_id){
        mimicked_attack->current_pp += amount;
        if(mimicked_attack->current_pp > 5)
            mimicked_attack->current_pp = 5;
        return;
    }
    monster->recoverPP(attack_id,amount);
}

unsigned int Battler::getMaxPPForAttack(unsigned int attack_id)const{
    if(attack_id == STRUGGLE_ID || attack_id == 0)
        return 0;
    if(mimicked_attack!=nullptr && mimicked_attack->attack_id == attack_id){
        return 5;
    }
    return monster->getMaxPPForAttack(attack_id);
}

void Battler::transformInto(Monster* other){
    if(mimicked_attack != nullptr){
        delete mimicked_attack;
        mimicked_attack = nullptr;
    }
    monster->transformInto(other);
    this->types.clear();
    this->types.insert(monster->getType1());
    if(other->getType2()!=NO_TYPE){
        this->types.insert(monster->getType2());
    }
    this->battler_ability = monster->getAbility();
    this->has_ability_suppressed = false;
    this->stockpiles = 0;
    this->disabled_attack_id = 0;
    this->disabled_turns_left = 0;
    this->same_attack_counter = 0;
    this->last_attack_used = TRANSFORM_ID;
    this->attacks_used.clear();
}

std::pair<unsigned int,bool> Battler::addDamage(unsigned int amount, AttackType category, float effectiveness, bool is_sound){
    //try damage the substitute
    if(hasSubstitute() && !is_sound &&!opponent->hasAbility(INFILTRATOR)){
        handler->displayMsg("The substitute took the hit!");
        if(amount >= substituteHP){
            amount = substituteHP;
            removeSubstitute();
        }else{
            substituteHP -= amount;
        }
        return {amount,true};
    }
    unsigned int currentHP = monster->getCurrentHP();
    if(isAtFullHP() && hasAbility(MULTISCALE)){
        handler->displayMsg(getNickname()+"'s Multiscale reduces the damage!");
        amount = max(1,amount / 2);
    }
    if(currentHP != 1 && 
        currentHP < amount &&
        hasAbility(STURDY)){
        handler->displayMsg(getNickname()+" endures the hit!");
        amount = currentHP - 1;
    }else if(currentHP != 1 && 
        currentHP < amount &&
        hasHeldItem(FOCUS_BAND) &&
        RNG::getRandomInteger(1,10)==1){
        handler->displayMsg(getNickname()+"'s endures the it thanks to its Focus Band!");
        amount = currentHP - 1;
    }else if(currentHP != 1 && 
        currentHP < amount &&
        hasHeldItem(FOCUS_SASH)){
        consumeHeldItem();
        handler->displayMsg(getNickname()+"'s endures the it thanks to its Focus Sash!");
        amount = currentHP - 1;
    }else if(//currentHP != 1 &&
        currentHP < amount &&
        hasVolatileCondition(ENDURE)){
        handler->displayMsg(getNickname()+" endures the hit!");
        amount = currentHP - 1;
    }
    unsigned int dmg = monster->addDamage(amount);
    if(hasHeldItem(AIR_BALLOON)){
        ItemData* item_data = ItemData::getItemData(AIR_BALLOON);
        handler->displayMsg(getNickname()+"'s "+item_data->getName()+" popped!");
        removeHeldItem();
    }
    if(dmg>0)
        removeVolatileCondition(FOCUSED);
    if(category==PHYSICAL)
        setPhysicalDamageTakenThisTurn(dmg);
    else if(category==SPECIAL)
        setSpecialDamageTakenThisTurn(dmg);
    if(isFainted()){
        if(hasSubstitute()){
            removeSubstitute();
        }
        handler->displayMsg(getNickname()+" fainted!");
        return {dmg,false};
    }
    
    return {dmg,false};
}

void Battler::tryEatAfterGettingHitBerry(AttackType category,float effectiveness,Battler*attacker){
    if(isFainted())
        return;
    ItemType held_item = getHeldItem();
    ItemData* item_data = ItemData::getItemData(held_item);
    if(item_data==nullptr)  
        return;
    if(item_data->getCategory()!=BERRY) 
        return;
    if(hasVolatileCondition(UNNERVED))
        return;
    switch(held_item){
        case ENIGMA_BERRY:{
            if(effectiveness > 1.1)
                return;
            consumeHeldItem();
            break;
        }
        case ROWAP_BERRY:{
            if(category != SPECIAL)
                return;
            if(attacker == nullptr)
                return;
            if(attacker->isFainted())
                return;
            handler->displayMsg(getNickname()+" ate its "+item_data->getName()+"!");
            removeHeldItem();
            monster->setConsumedItem(ROWAP_BERRY);
            addVolatileCondition(JUST_EATEN_BERRY,2);
            unsigned int dmg = max(1,attacker->getMaxHP()/8);
            unsigned int actual_dmg = attacker->addDirectDamage(dmg);
            handler->displayMsg(attacker->getNickname()+" took "+std::to_string(actual_dmg)+" damage thanks to "+getNickname()+"'s "+item_data->getName()+"!");
            break;
        }
        case JABOCA_BERRY:{
            if(category != PHYSICAL)
                return;
            if(attacker == nullptr)
                return;
            if(attacker->isFainted())
                return;
            handler->displayMsg(getNickname()+" ate its "+item_data->getName()+"!");
            removeHeldItem();
            monster->setConsumedItem(JABOCA_BERRY);
            addVolatileCondition(JUST_EATEN_BERRY,2);
            unsigned int dmg = attacker->getMaxHP()/8;
            unsigned int actual_dmg = attacker->addDirectDamage(dmg);
            handler->displayMsg(attacker->getNickname()+" took "+std::to_string(actual_dmg)+" damage thanks to "+getNickname()+"'s "+item_data->getName()+"!");
            break;
        }
        case KEE_BERRY:{
            if(category!=PHYSICAL)
                return;
            consumeHeldItem();
            break;
        }
        case MARANGA_BERRY:{
            if(category!=SPECIAL)
                return;
            consumeHeldItem();
            break;
        }
        default:
            return;
    }
}

unsigned int Battler::addDirectDamage(unsigned int amount){
    unsigned int dmg = monster->addDamage(amount);
    if(isFainted()){
        if(hasSubstitute()){
            removeSubstitute();
        }
        handler->displayMsg(getNickname()+" fainted!");
        return dmg;
    }
    // berry check
    tryEatLowHPBerry();
    return dmg;
}

void Battler::tryEatStartOfTurnBerry(){
    if(isFainted())
        return;
    ItemType held_item = getHeldItem();
    ItemData* item_data = ItemData::getItemData(held_item);
    if(item_data==nullptr)  
        return;
    if(item_data->getCategory()!=BERRY) 
        return;
    if(hasVolatileCondition(UNNERVED))
        return;
    switch(held_item){
        case MICLE_BERRY:
        case APICOT_BERRY:
        case CUSTAP_BERRY:
        case LANSAT_BERRY:{
            if(!(getCurrentHP() < getMaxHP()/4 || (hasAbility(GLUTTONY) && getCurrentHP() < getMaxHP()/2)))
                return;
            consumeHeldItem();
            break;
        }
        default:
            return;
    }
    
}

void Battler::tryEatStatusBerry(){
    if(isFainted())
        return;
    ItemType held_item = getHeldItem();
    ItemData* item_data = ItemData::getItemData(held_item);
    if(item_data==nullptr)  
        return;
    if(item_data->getCategory()!=BERRY) 
        return;
    if(hasVolatileCondition(UNNERVED))
        return;
    switch(held_item){
        //berries activating in case of status
        case ASPEAR_BERRY:{
            if(!isFrozen())
                return;
            consumeHeldItem();
            break;      
        }
        case CHERY_BERRY:{
            if(!isParalyzed())
                return;
            consumeHeldItem();
            break;      
        }
        case CHESTO_BERRY:{
            if(!isAsleep())
                return;
            consumeHeldItem();
            break;      
        }
        case PECHA_BERRY:{
            if(!isPoisoned())
                return;
            consumeHeldItem();
            break;      
        }
        case RAWST_BERRY:{
            if(!isBurned())
                return;
            consumeHeldItem();
            break;      
        }
        case PERSIM_BERRY:{
            if(!hasVolatileCondition(CONFUSION))
                return;
            consumeHeldItem();
            break;
        }
        case LUM_BERRY:{
            if(!hasPermanentStatus() && !hasVolatileCondition(CONFUSION))
                return;
            consumeHeldItem();
            break;
        }
        // berries that cannot be consumed in battle
        default:
            return;
    }
}

void Battler::tryEatLowHPBerry(){
    if(isFainted())
        return;
    ItemType held_item = getHeldItem();
    ItemData* item_data = ItemData::getItemData(held_item);
    if(item_data==nullptr)  
        return;
    if(item_data->getCategory()!=BERRY && held_item!=BERRY_JUICE) 
        return;
    if(item_data->getCategory()==BERRY && hasVolatileCondition(UNNERVED))
        return;
    switch(held_item){
        //berries activating at below 50% health
        case ORAN_BERRY:
        case BERRY_JUICE:
        case SITRUS_BERRY:{
            if(getCurrentHP() >= getMaxHP()/2)
                return;
            consumeHeldItem();
            break;
        }
        // berries activating below 25%
        // case CUSTAP_BERRY:
        // case MICLE_BERRY:
        // case LANSAT_BERRY:
        case AGUAV_BERRY:
        case WIKI_BERRY:
        case IAPAPA_BERRY:
        case MAGO_BERRY:
        case FIGY_BERRY:
        case LIECHI_BERRY:
        case STARF_BERRY:
        case SALAC_BERRY:
        case PETAYA_BERRY:
        // case APICOT_BERRY:
        case GANION_BERRY:{
            if(!(getCurrentHP() < getMaxHP()/4 || (hasAbility(GLUTTONY) && getCurrentHP() < getMaxHP()/2)))
                return;
            consumeHeldItem();
            break;
        }
        default:
            return;
    }
   
}

unsigned int Battler::getCurrentHP()const{
    return monster->getCurrentHP();
}
unsigned int Battler::getMaxHP()const{
    return monster->getMaxHP();
}

unsigned int Battler::removeDamage(unsigned int amount){
    unsigned int dmg = monster->removeDamage(amount);
    return dmg;
}

unsigned int Battler::getLevel()const{
    return monster->getLevel();
}

std::string Battler::getNickname()const{
    if(monster->isMegaEvolved())
        return "M-"+monster->getNickname();
    return monster->getNickname();
}

unsigned int Battler::getTurnsInBattle()const{
    return turns_in_battle;
}

void Battler::nextTurn(){
    turns_in_battle++;
}

bool Battler::isFirstTurn()const{
    return turns_in_battle == 1;
}

bool Battler::setAttackModifier(int amount){
    return stat_modifiers.setAtk(amount);
}
bool Battler::setDefenseModifier(int amount){
    return stat_modifiers.setDef(amount);
}
bool Battler::setSpecialAttackModifier(int amount){
    return stat_modifiers.setSpatk(amount);
}
bool Battler::setSpecialDefenseModifier(int amount){
    return stat_modifiers.setSpdef(amount);
}
bool Battler::setSpeedModifier(int amount){
    return stat_modifiers.setSpd(amount);
}
bool Battler::setAccuracyModifier(int amount){
    return stat_modifiers.setAccuracy(amount);
}
bool Battler::setEvasionModifier(int amount){
    return stat_modifiers.setEvasion(amount);
}

bool Battler::setPermanentStatus(PermanentStatusCondition status){
    if(getPermanentStatus() != NO_PERMANENT_CONDITION && getPermanentStatus() !=POISONED)
        return false;
    if(getPermanentStatus()==POISONED && status != BAD_POISON)
        return false;
    switch(status){
        case BURN:
            if(!canBeBurned())
                return false;
            monster->setPermanentStatus(BURN);
            handler->displayMsg(getNickname()+" was burned!");
            if(hasHeldItem(RAWST_BERRY))
                tryEatStatusBerry();
            break;
        case PARALYSIS:
            if(!canBeParalyzed())
                return false;
            monster->setPermanentStatus(PARALYSIS);
            handler->displayMsg(getNickname()+" was paralyzed!");
            if(hasHeldItem(CHERY_BERRY))
                tryEatStatusBerry();
            break;
        case FREEZE:
            if(!canBeFrozen())
                return false;
            monster->setPermanentStatus(FREEZE);
            handler->displayMsg(getNickname()+" was frozen!");
            if(hasHeldItem(ASPEAR_BERRY))
                tryEatStatusBerry();
            break;
        case POISONED:
            if(!canBePoisoned())
                return false;
            monster->setPermanentStatus(POISONED);
            handler->displayMsg(getNickname()+" was poisoned!");
            if(hasHeldItem(PECHA_BERRY))
                tryEatStatusBerry();
            break;
        case BAD_POISON:
            if(!canBeBadlyPoisoned())
                return false;
            monster->setPermanentStatus(BAD_POISON);
            bad_poison_counter = 1;
            handler->displayMsg(getNickname()+" was badly poisoned!");
            break;
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            if(!canFallAsleep())
                return false;
            monster->setPermanentStatus(status);
            handler->displayMsg(getNickname()+" fell asleep!");
            if(hasHeldItem(CHESTO_BERRY))
                tryEatStatusBerry();
            break;
        default:
            return false;
            break;
    }
    return true;
}
bool Battler::setPermanentStatusForced(PermanentStatusCondition status){
    PermanentStatusCondition old_status = monster->getPermanentStatus();
    monster->setPermanentStatus(status);
    if(status == BAD_POISON)
        bad_poison_counter = 1;
    switch(status){
        case BURN:
            monster->setPermanentStatus(BURN);
            handler->displayMsg(getNickname()+" was burned!");
            break;
        case PARALYSIS:
            monster->setPermanentStatus(PARALYSIS);
            handler->displayMsg(getNickname()+" was paralyzed!");
            break;
        case FREEZE:
            monster->setPermanentStatus(FREEZE);
            handler->displayMsg(getNickname()+" was frozen!");
            break;
        case POISONED:
            monster->setPermanentStatus(POISONED);
            handler->displayMsg(getNickname()+" was poisoned!");
            break;
        case BAD_POISON:
            monster->setPermanentStatus(BAD_POISON);
            bad_poison_counter = 1;
            handler->displayMsg(getNickname()+" was badly poisoned!");
            break;
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            monster->setPermanentStatus(status);
            if(old_status != SLEEP_1 &&
                old_status != SLEEP_2 &&
                old_status != SLEEP_3 &&
                old_status != SLEEP_4)//display msg only if it was not asleep before
                handler->displayMsg(getNickname()+" fell asleep!");
            break;
        default:
            return false;
            break;
    }
    return true;
}


PermanentStatusCondition Battler::getPermanentStatus()const{
    return monster->getPermanentStatus();
}

bool Battler::clearPermanentStatus(){
    if(getPermanentStatus() == NO_PERMANENT_CONDITION)
        return false;
    PermanentStatusCondition old_status = monster->getPermanentStatus();
    switch(old_status){
        case BURN:
            handler->displayMsg(getNickname()+" was cured of its burn!");
            break;
        case PARALYSIS:
            handler->displayMsg(getNickname()+" was cured of its paralysis!");
            break;
        case FREEZE:
            handler->displayMsg(getNickname()+" thawed out!");
            break;
        case POISONED:
        case BAD_POISON:
            handler->displayMsg(getNickname()+" was cured from the poisoning!");
            break;
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            handler->displayMsg(getNickname()+" woke up!");
            break;
        default:
            return false;
    }
    monster->setPermanentStatus(NO_PERMANENT_CONDITION);
    bad_poison_counter = 0;
    return true;
}

unsigned int Battler::getHeight()const{
    return height;
}

unsigned int Battler::getWeight()const{
    unsigned int res = weight;
    if(hasAbility(LIGHT_METAL))
        res = max(1,res/2);
    if(hasAbility(HEAVY_METAL))
        res = res*2;
    if(hasHeldItem(FLOAT_STONE))
        res = max(1,res/2);
    return res;
}

void Battler::changeWeight(int amount){
    if(amount == 0)
        return;
    if(amount > 0){
        weight+=amount;
    }else{
        amount = -amount;
        if(amount >= (int)weight)
            weight = 1;
        else
            weight -= amount;
    }
}
void Battler::changeHeight(int amount){
    if(amount == 0)
        return;
    if(amount > 0){
        height+=amount;
    }else{
        amount = -amount;
        if(amount >= (int)height)
            height = 1;
        else
            height -= amount;
    }
}

bool Battler::lastAttackFailed()const{
    return last_attack_failed;
}
void Battler::setLastAttackFailed(){
    last_attack_failed = true;
}
void Battler::setLastAttackHit(){
    last_attack_failed = false;
}
void Battler::hitOnceMore(Type attack_type){
    hits_taken++;
    if((attack_type == BUG || attack_type == DARK || attack_type == GHOST) &&
        hasAbility(RATTLED)){
        handler->displayMsg(getNickname()+"'s Rattled boosts its Speed!");
        bool res = changeSpeedModifier(1);
        if(res && opponent->hasHeldItem(MIRROR_HERB)){
            opponent->consumeHeldItem();
            opponent->changeSpeedModifier(1);
        }
    }
}
unsigned int Battler::numberOfHitsTaken()const{
    return hits_taken;
}

Stats Battler::getStats()const{
    return monster->getStats();
}

Gender Battler::getGender()const{
    return monster->getGender();
}

bool Battler::isAtFullHP()const{
    return monster->isAtFullHP();
}

unsigned int Battler::getPhysicalDamageTakenThisTurn()const{
    return physical_dmg_taken_this_turn;
}
unsigned int Battler::getSpecialDamageTakenThisTurn()const{
    return special_dmg_taken_thi_turn;
}
void Battler::setPhysicalDamageTakenThisTurn(unsigned int dmg){
    physical_dmg_taken_this_turn += dmg;
}
void Battler::setSpecialDamageTakenThisTurn(unsigned int dmg){
    special_dmg_taken_thi_turn += dmg;
}
void Battler::resetDamageTakenThisTurn(){
    physical_dmg_taken_this_turn = 0;
    special_dmg_taken_thi_turn = 0;
}

void Battler::addLockOn(Monster* monster){
    if(monster == nullptr)
        return;
    if(hasLockOn(monster))
        return;
    lock_ons.insert(monster);
    handler->displayMsg(getNickname()+" is locked on!");
}
void Battler::removeLockOn(Monster* monster){
    auto it = lock_ons.find(monster);
    if(it != lock_ons.end()){
        lock_ons.erase(it);
    }
}
bool Battler::hasLockOn(Monster* monster)const{
    if(monster == nullptr)
        return false;
    auto it = lock_ons.find(monster);
    if(it != lock_ons.end()){
        return true;
    }
    return false;
}

void Battler::activateMoldBreaker(){
    is_mold_breaker_active = true;
}
bool Battler::isMoldBreakerActive()const{
    return is_mold_breaker_active;
}
void Battler::deactivateMoldBreaker(){
    is_mold_breaker_active = false;
}

Type Battler::getHiddenPowerType()const{
    return monster->getHiddenPowerType();
}

bool Battler::hasHeldItem()const{
    return monster->hasHeldItem();
}
ItemType Battler::getHeldItem()const{
    return monster->getHeldItem();
}
ItemType Battler::getConsumedItem()const{
    return monster->getConsumedItem();
}
bool Battler::hasHeldItem(ItemType item)const{
    if(item == NO_ITEM_TYPE)
        return false;
    if(!hasHeldItem())
        return false;
    ItemType held_item = monster->getHeldItem();
    if(held_item == item)
        return true;
    return false;
}
ItemType Battler::setHeldItem(ItemType item){
    if(item != NO_ITEM_TYPE)
        had_held_item = false;
    ItemType old_item = monster->setHeldItem(item);
    if(canConsumeWhiteHerb())
        consumeHeldItem();
    if((hasDiabledAttack() || hasVolatileCondition(INFATUATION) ||
        hasVolatileCondition(ENCORE) || hasVolatileCondition(TAUNTED) ||
        hasVolatileCondition(TORMENTED))&&hasHeldItem(MENTAL_HERB)){
        consumeHeldItem();
    }
    return old_item;
}
ItemType Battler::removeHeldItem(){
    ItemType res = monster->removeHeldItem();
    if(res != NO_ITEM_TYPE){
        had_held_item = true;
    }
    return res;
}

bool Battler::useItem(ItemType item_type,unsigned int data){
    if(item_type == NO_ITEM_TYPE)
        return false;
    if(isFainted())
        return false;
    // ItemData* item_data = ItemData::getItemData(item_type);
    bool res = monster->useItem(item_type, handler, data);
    switch (item_type){
        case FULL_HEAL:
        case LUM_BERRY:
        case FULL_RESTORE:
        case PERSIM_BERRY:{
            if(hasVolatileCondition(CONFUSION)){
                removeVolatileCondition(CONFUSION);
                res = true;
            }
            break;
        }
        case AGUAV_BERRY:
        case MAGO_BERRY:
        case IAPAPA_BERRY:
        case WIKI_BERRY:
        case FIGY_BERRY:{
            if(!hasVolatileCondition(CONFUSION) && monster->dislikesBerry(item_type) && !monster->likesBerry(item_type)){
                addVolatileCondition(CONFUSION,RNG::getRandomInteger(2,5));
                res = true;
            }
            break;
        }
        case APICOT_BERRY:
        case MARANGA_BERRY:{
            bool item_res = changeSpecialDefenseModifier(1);
            if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                opponent->consumeHeldItem();
                opponent->changeSpecialDefenseModifier(1);
            }
            res = true;
            break;
        }
        case GANION_BERRY:
        case KEE_BERRY:{
                bool item_res = changeDefenseModifier(1);
                if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                    opponent->consumeHeldItem();
                    opponent->changeDefenseModifier(1);
                }
                res = true;
                break;
            }
        case LIECHI_BERRY:{
            bool item_res = changeAttackModifier(1);
            if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                opponent->consumeHeldItem();
                opponent->changeAttackModifier(1);
            }
            res = true;
            break;
        }
        case SALAC_BERRY:{
            bool item_res = changeSpeedModifier(1);
            if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                opponent->consumeHeldItem();
                opponent->changeSpeedModifier(1);
            }
            res = true;
            break;
        }
        case PETAYA_BERRY:{
            bool item_res = changeSpecialAttackModifier(1);
            if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                opponent->consumeHeldItem();
                opponent->changeSpecialAttackModifier(1);
            }
            res = true;
            break;
        }
        case STARF_BERRY:{
            // sharply boost random stat
            int random_integer = RNG::getRandomInteger(0,4);
            switch(random_integer){
                case 0:{
                    bool item_res = changeAttackModifier(2);
                    if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                        opponent->consumeHeldItem();
                        opponent->changeAttackModifier(2);
                    }
                    break;
                }
                case 1: {
                    bool item_res = changeSpecialAttackModifier(2);
                    if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                        opponent->consumeHeldItem();
                        opponent->changeSpecialAttackModifier(2);
                    }
                    break;
                }
                case 2:{
                    bool item_res = changeDefenseModifier(2);
                    if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                        opponent->consumeHeldItem();
                        opponent->changeDefenseModifier(2);
                    }
                    break;
                }
                case 3:{
                    bool item_res = changeSpecialDefenseModifier(2);
                    if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                        opponent->consumeHeldItem();
                        opponent->changeSpecialDefenseModifier(2);
                    }
                    break;
                }
                case 4:{
                    bool item_res = changeSpeedModifier(2);
                    if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                        opponent->consumeHeldItem();
                        opponent->changeSpeedModifier(2);
                    }
                    break;
                }
                default:break;
            }
            break;
        }
        case CUSTAP_BERRY:{
            addVolatileCondition(MOVING_FIRST,-1);
            res = true;
            break;
        }
        case MICLE_BERRY:{
            addVolatileCondition(INCREASED_ACCURACY,-1);
            res = true;
            break;
        }
        case LANSAT_BERRY:{
            addVolatileCondition(INCREASED_CRIT,-1);
            res = true;
            break;
        }
        // case ELECTRIC_SEED:{
        //     if(field->getTerrain() != ELECTRIC_FIELD)
        //         return false;
        //     changeDefenseModifier(1);
        //     res = true;
        //     break;
        // }
        // case GRASSY_SEED:{
        //     if(field->getTerrain() != GRASSY_FIELD)
        //         return false;
        //     changeDefenseModifier(1);
        //     res = true;
        //     break;
        // }
        // case MISTY_SEED:{
        //     if(field->getTerrain() != MISTY_FIELD)
        //         return false;
        //     changeSpecialDefenseModifier(1);
        //     res = true;
        //     break;
        // }
        // case PSYCHIC_SEED:{
        //     if(field->getTerrain() != PSYCHIC_FIELD)
        //         return false;
        //     changeSpecialDefenseModifier(1);
        //     res = true;
        //     break;
        // }
        case WHITE_HERB:{
            if(getAttackModifier() < 0){
                setAttackModifier(0);
                res = true;
            }
            if(getSpecialAttackModifier() < 0){
                setSpecialAttackModifier(0);
                res = true;
            }
            if(getDefenseModifier() < 0){
                setDefenseModifier(0);
                res = true;
            }
            if(getSpecialDefenseModifier() < 0){
                setSpecialDefenseModifier(0);
                res = true;
            }
            if(getSpeedModifier() < 0){
                setSpeedModifier(0);
                res = true;
            }
            if(getAccuracyModifier() < 0){
                setAccuracyModifier(0);
                res = true;
            }
            if(getEvasionModifier() < 0){
                setEvasionModifier(0);
                res = true;
            }
            if(res)
                handler->displayMsg(getNickname()+"'s White Herb restored its stats!");
            break;
        }
        case MENTAL_HERB:{
            if(hasDiabledAttack()){
                removeDisable();
                res = true;
            }
            if(hasVolatileCondition(TORMENTED)){
                removeVolatileCondition(TORMENTED);
                res = true;
            }
            if(hasVolatileCondition(ENCORE)){
                removeVolatileCondition(ENCORE);
                res = true;
            }
            if(hasVolatileCondition(TAUNTED)){
                removeVolatileCondition(TAUNTED);
                res = true;
            }
            if(hasVolatileCondition(INFATUATION)){
                removeVolatileCondition(INFATUATION);
                res = true;
            }
            if(res)
                handler->displayMsg(getNickname()+"'s Mental Herb cured it of some conditions!");
            break;
        }
        case BLUNDER_POLICY:{
            bool item_res = changeSpeedModifier(2);
            if(item_res && opponent->hasHeldItem(MIRROR_HERB)){
                opponent->consumeHeldItem();
                opponent->changeSpeedModifier(2);
            }
            res = true;
            break;
        }
        default:
            break;
    }   
    if(!res)
        handler->displayMsg("But it failed");
    return res;
}

bool Battler::itemWouldHaveEffect(ItemType item_type)const{
    if(item_type == NO_ITEM_TYPE)
        return false;
    if(isFainted())
        return false;
    // ItemData* item_data = ItemData::getItemData(item_type);
    bool res = monster->itemWouldHaveEffect(item_type);
    switch (item_type){
        case FULL_HEAL:
        case LUM_BERRY:
        case FULL_RESTORE:
        case PERSIM_BERRY:
            if(hasVolatileCondition(CONFUSION))
                res = true;
            break;
        case AGUAV_BERRY:
        case IAPAPA_BERRY:
        case WIKI_BERRY:
        case MAGO_BERRY:
        case FIGY_BERRY:
            if(!hasVolatileCondition(CONFUSION) && monster->dislikesBerry(item_type) && !monster->likesBerry(item_type))
                res = true;
            break;
        case APICOT_BERRY:
        case GANION_BERRY:
        case LIECHI_BERRY:
        case STARF_BERRY:
        case SALAC_BERRY:
        case PETAYA_BERRY:
            res = true;
            break;
        default:
            break;
    }   
    return res;
}

bool Battler::hasConsumedBerry()const{
    ItemData * item_data = ItemData::getItemData(monster->getConsumedItem());
    if(item_data == nullptr)
        return false;
    if(item_data->getCategory() == BERRY)
        return true;
    return false;
}

bool Battler::hasConsumedItem()const{
    ItemData * item_data = ItemData::getItemData(monster->getConsumedItem());
    return item_data != nullptr;
}

bool Battler::canStealItem()const{
    ItemType item = getHeldItem();
    if(!canBeStolen(item))
        return false;
    if(hasAbility(STICKY_HOLD))
        return false;
    return true;
}

bool Battler::consumeHeldItem(){
    if(!hasHeldItem())
        return false;
    ItemType item = monster->getHeldItem();
    if(canItemBeConsumed(item)){
        ItemData* item_data = ItemData::getItemData(item);
        if(item_data->getCategory()==BERRY){
            handler->displayMsg(getNickname()+" ate its "+item_data->getName()+"!");
            addVolatileCondition(JUST_EATEN_BERRY,2);
        }else
            handler->displayMsg(getNickname()+" consumed its "+item_data->getName()+"!");
        monster->setConsumedItem(item);
        removeHeldItem();
        useItem(item,0);
        had_held_item = true;
        return true;
    }
    return false;
}

void Battler::consumeItem(ItemType item){
    if(item == NO_ITEM_TYPE)
        return;
    if(!canItemBeConsumed(item))
        return;
    useItem(item,0);
    monster->setConsumedItem(item);
    had_held_item = true;
}

bool Battler::restoreBerry(){
    if(monster->getConsumedItem() == NO_ITEM_TYPE)
        return false;
    ItemData* item_data = ItemData::getItemData(monster->getConsumedItem());
    if(item_data == nullptr)
        return false;
    if(item_data->getCategory() != BERRY)
        return false;
    setHeldItem(monster->getConsumedItem());
    monster->setConsumedItem(NO_ITEM_TYPE);
    return true;
}

bool Battler::restoreItem(){
    if(monster->getConsumedItem() == NO_ITEM_TYPE)
        return false;
    ItemData* item_data = ItemData::getItemData(monster->getConsumedItem());
    if(item_data == nullptr)
        return false;
    setHeldItem(monster->getConsumedItem());
    monster->setConsumedItem(NO_ITEM_TYPE);
    return true;
}

bool Battler::hasLostHeldItem()const{
    return had_held_item;
}

bool Battler::tryEatSuperEffectiveBerry(Type attack_type, bool is_supereffective){
    if(isFainted())
        return false;
    if(!is_supereffective && !hasHeldItem(CHILAN_BERRY))
        return false;
    ItemType held_item = getHeldItem();
    ItemData* item_data = ItemData::getItemData(held_item);
    if(item_data==nullptr)  
        return false;
    if(item_data->getCategory()!=BERRY)
        return false;
    if(hasVolatileCondition(UNNERVED))
        return false;
    bool consume = false;
    switch(held_item){
        case BABIRI_BERRY:{
            if(attack_type == STEEL)
                consume = true;
            break;
        }
        case CHARTI_BERRY:{
            if(attack_type == ROCK)
                consume = true;
            break;
        }
        case CHILAN_BERRY:{
            if(attack_type == NORMAL)
                consume = true;
            break;
        }
        case CHOPLE_BERRY:{
            if(attack_type == FIGHTING)
                consume = true;
            break;
        }
        case COBA_BERRY:{
            if(attack_type == FLYING)
                consume = true;
            break;
        }
        case COLBUR_BERRY:{
            if(attack_type == DARK)
                consume = true;
            break;
        }
        case HABAN_BERRY:{
            if(attack_type == DRAGON)
                consume = true;
            break;
        }
        case KASIB_BERRY:{
            if(attack_type == GHOST)
                consume = true;
            break;
        }
        case KEBIA_BERRY:{
            if(attack_type == POISON)
                consume = true;
            break;
        }
        case OCCA_BERRY:{
            if(attack_type == FIRE)
                consume = true;
            break;
        }
        case PASSHO_BERRY:{
            if(attack_type == WATER)
                consume = true;
            break;
        }
        case PAYAPA_BERRY:{
            if(attack_type == PSYCHIC)
                consume = true;
            break;
        }
        case RINDO_BERRY:{
            if(attack_type == GRASS)
                consume = true;
            break;
        }
        case ROSELI_BERRY:{
            if(attack_type == FAIRY)
                consume = true;
            break;
        }
        case SHUCA_BERRY:{
            if(attack_type == GROUND)
                consume = true;
            break;
        }
        case TANGA_BERRY:{
            if(attack_type == BUG)
                consume = true;
            break;
        }
        case WACAN_BERRY:{
            if(attack_type == ELECTRIC)
                consume = true;
            break;
        }
        case YACHE_BERRY:{
            if(attack_type == ICE)
                consume = true;
            break;
        }
        default:return false;
    }
    if(consume){
        removeHeldItem();
        monster->setConsumedItem(held_item);
        handler->displayMsg(getNickname()+" ate its "+item_data->getName()+" to reduce the incoming damage!");
        return true;
    }
    return false;
}

bool Battler::ismegaEvolved()const{
    return monster->isMegaEvolved();
}

bool Battler::megaEvolve(){
    std::string old_nick = getNickname();
    if(monster->megaEvolve()){
        handler->displayMsg(old_nick+" mega evolved!");
        battler_ability = monster->getAbility();
        weight = monster->getWeight();
        height = monster->getHeight();
        types.clear();
        types.insert(monster->getType1());
        if(monster->getType2()!=NO_TYPE){
            types.insert(monster->getType2());
        }
        return true;
    }
    return false;
}

bool Battler::canMegaEvolve()const{
    if(isFainted())
        return false;
    return monster->canMegaEvolve();
}

Stats Battler::getBattleStats()const{
    return battle_stats;
}
void Battler::setBattleAttack(unsigned int attack){
    battle_stats.setAtk(attack);
}
void Battler::setBattleDefense(unsigned int defense){
    battle_stats.setDef(defense);
}
void Battler::setBattleSpecialAttack(unsigned int special_attack){
    battle_stats.setSpatk(special_attack);
}
void Battler::setBattleSpecialDefense(unsigned int special_defense){
    battle_stats.setSpdef(special_defense);
}
void Battler::setBattleSpeed(unsigned int speed){
    battle_stats.setSpd(speed);
}
void Battler::addSeenOpponent(Monster* opponent){
    monster->addSeenOpponent(opponent);
}

bool Battler::canConsumeWhiteHerb()const{
    if(!hasHeldItem(WHITE_HERB))
        return false;
    if(getAttackModifier() < 0)
        return true;
    if(getSpecialAttackModifier() < 0)
        return true;
    if(getDefenseModifier() < 0)
        return true;
    if(getSpecialDefenseModifier() < 0)
        return true;
    if(getSpeedModifier() < 0)
        return true;
    if(getAccuracyModifier() < 0)
        return true;
    if(getEvasionModifier() < 0)
        return true;
    return false;
}

void Battler::removeDisable(){
    disabled_attack_id = 0;
    disabled_turns_left = 0;
}

void Battler::setOpponent(Battler* opponent){
    this->opponent = opponent;
}

bool Battler::hasSubstitute()const{
    return substituteHP > 0;
}

void Battler::setSubstituteHP(unsigned int amount){
    substituteHP = amount;
    removeVolatileCondition(FIRESPIN);
    removeVolatileCondition(WRAP);
    removeVolatileCondition(BIND);
    removeVolatileCondition(SANDTOMB);
    removeVolatileCondition(WHIRLPOOL);
    removeVolatileCondition(INFESTED);
}

void Battler::removeSubstitute(){
    if(substituteHP > 0){
        handler->displayMsg(getNickname()+"'s substitute faded!");
        substituteHP = 0;
    }
}