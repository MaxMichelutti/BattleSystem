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
    stat_modifiers = Modifiers(0,0,0,0,0,0,0);
    mimicked_attack = nullptr;
    disabled_attack_id = 0;
    disabled_turns_left = 0;
    turns_in_battle = 0;
    last_attack_failed = false;
    hits_taken = 0;
    resetDamageTakenThisTurn();
    is_mold_breaker_active = false;
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
    if(getAbility()==OWN_TEMPO && condition == CONFUSION){
        handler->displayMsg(monster->getNickname()+"'s Own Tempo prevents confusion!");
        return;
    }
    if(getAbility()==INNER_FOCUS && condition == FLINCH){
        return;
    }
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD &&
        condition==DROWSY){
        handler->displayMsg(monster->getNickname()+"'s Leaf Guard prevents drowsiness!");
        return;
    }
    volatile_conditions.insert({condition,duration});
    switch(condition){
        case GROUNDED:
            if(monster->hasType(FLYING)){
                handler->displayMsg(getNickname()+" lands on the ground!");
            }
            break;
        case FLYING_HIGH:
            handler->displayMsg(getNickname()+" flies high!");
            break;
        case UNDERGROUND:
            handler->displayMsg(getNickname()+" digs under ground!");
            break;
        case CHARGING_SKYATTACK:
            handler->displayMsg(getNickname()+" became cloaked in a harsh light!");
            break;
        case UNDERWATER:
            handler->displayMsg(getNickname()+" dives deep!");
            break;
        case INFATUATION:
            handler->displayMsg(getNickname()+" is infatuated!");
            break;
        case CONFUSION:
            handler->displayMsg(getNickname()+" is confused!");
            break;
        case FIRESPIN:
            handler->displayMsg(getNickname()+" is trapped in a vortex of fire!");
            break;
        case WHIRLPOOL:
            handler->displayMsg(getNickname()+" is trapped in a whirlpool of water!");
            break;
        case SANDTOMB:
            handler->displayMsg(getNickname()+" is trapped in a vortex of sand!");
            break;
        case WRAP:
            handler->displayMsg(getNickname()+" is trapped in a wrap!");
            break;
        case BIND:
            handler->displayMsg(getNickname()+" is binded by the its opponent!");
            break;
        case LEECH_SEED:
            handler->displayMsg(getNickname()+" is seeded!");
            break;
        case LASER_FOCUS:
            handler->displayMsg(getNickname()+" is focusing its energy!");
            break;
        case FOCUS_ENERGY:
            handler->displayMsg(getNickname()+" is getting pumped!");
            break;
        case MEAN_LOOK:
        case BLOCKED:
            handler->displayMsg(getNickname()+" cannot escape!");
            break;
        case TAUNTED:
            handler->displayMsg(getNickname()+" is taunted!");
            break;
        case PROTECT:
            handler->displayMsg(getNickname()+" protects itself!");
            break;
        case IMPRISON:
            handler->displayMsg(getNickname()+" cannot use attacks known by its opponent!");
            break;
        case CHARGING_SOLARBEAM:
            handler->displayMsg(getNickname()+" coolects power from the sun!");
            break;
        case DROWSY:
            handler->displayMsg(getNickname()+" is drowsy!");
            break;
        case CURSED:
            handler->displayMsg(getNickname()+" was cursed!");
            break;
        case DESTINY_BOND:
            handler->displayMsg(getNickname()+" tries to knock out whoever knocks it out!");
            break;
        case CHARGED:
            handler->displayMsg(getNickname()+" is charging its next Electric type attack!");
            break;
        case FOCUSED:
            handler->displayMsg(getNickname()+" is focusing!");
            break;
        case INGRAINED:
            handler->displayMsg(getNickname()+" plants its roots on the ground!");
            break;
        default:
            break;
    }
}

void Battler::removeVolatileCondition(VolatileStatusCondition condition) {
    if(!hasVolatileCondition(condition))
        return;
    volatile_conditions.erase(condition);
    switch(condition){
        case GROUNDED:
            if(monster->hasType(FLYING)){
                handler->displayMsg(monster->getNickname()+" tries to take flight!");
            }
            break;
        case INFATUATION:
            handler->displayMsg(monster->getNickname()+" is no longer infatuated!");
            break;
        case FIRESPIN:
            handler->displayMsg(monster->getNickname()+" is no longer trapped in a vortex of fire!");
            break;
        case WHIRLPOOL:
            handler->displayMsg(monster->getNickname()+" is no longer trapped in a whirlpool of water!");
            break;
        case SANDTOMB:
            handler->displayMsg(monster->getNickname()+" is no longer trapped in a vortex of sand!");
            break;
        case WRAP:
            handler->displayMsg(monster->getNickname()+" is no longer trapped in a wrap!");
            break;
        case BIND:
            handler->displayMsg(monster->getNickname()+" is no longer binded!");
            break;
        case LEECH_SEED:
            handler->displayMsg(monster->getNickname()+" is no longer seeded!");
            break;
        case TAUNTED:
            handler->displayMsg(monster->getNickname()+" is no longer taunted!");
            break;
        case CONFUSION:
            handler->displayMsg(monster->getNickname()+" snapped out of confusion!");
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
    name += monster->getNickname();
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
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(getAbility() == HYPER_CUTTER && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Hyper Cutter prevents the Attack reduction!");
        return false;
    }
    bool res = stat_modifiers.changeAtk(amount);
    displayStatModifyResult(res,amount,"Attack");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeAttackModifierForced(int amount) {
    bool res = stat_modifiers.changeAtk(amount);
    displayStatModifyResult(res,amount,"Attack");
    return res;
}

bool Battler::changeDefenseModifier(int amount) {
    std::string name = monster->getNickname();
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(name+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(getAbility() == BIG_PECKS && amount < 0){
        handler->displayMsg(name+"'s Defense cannot be lowered!");
        return false;
    }
    bool res= stat_modifiers.changeDef(amount);
    displayStatModifyResult(res,amount,"Defense");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeDefenseModifierForced(int amount) {
    bool res = stat_modifiers.changeDef(amount);
    displayStatModifyResult(res,amount,"Defense");
    return res;
}

bool Battler::changeSpecialAttackModifier(int amount) {
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    bool res= stat_modifiers.changeSpatk(amount);
    displayStatModifyResult(res,amount,"Special Attack");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}


bool Battler::changeSpecialAttackModifierForced(int amount) {
    bool res= stat_modifiers.changeSpatk(amount);
    displayStatModifyResult(res,amount,"Special Attack");
    return res;
}

bool Battler::changeSpecialDefenseModifier(int amount) {
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    bool res= stat_modifiers.changeSpdef(amount);
    displayStatModifyResult(res,amount,"Special Defense");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeSpecialDefenseModifierForced(int amount) {
    bool res= stat_modifiers.changeSpdef(amount);
    displayStatModifyResult(res,amount,"Special Defense");
    return res;
}

bool Battler::changeSpeedModifier(int amount) {
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    bool res = stat_modifiers.changeSpd(amount);
    displayStatModifyResult(res,amount,"Speed");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeSpeedModifierForced(int amount) {
    bool res = stat_modifiers.changeSpd(amount);
    displayStatModifyResult(res,amount,"Speed");
    return res;
}

bool Battler::changeAccuracyModifier(int amount) {
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    if(getAbility()==ILLUMINATE && amount<0){
        handler->displayMsg(monster->getNickname()+"'s Accuracy cannot be lowered!");
        return false;
    }
    std::string name = monster->getNickname();
    if(getAbility() == KEEN_EYE && amount<0){
        handler->displayMsg(name+"'s Accuracy cannot be lowered!");
        return false;
    }
    bool res = stat_modifiers.changeAccuracy(amount);
    displayStatModifyResult(res,amount,"Accuracy");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeAccuracyModifierForced(int amount) {
    bool res = stat_modifiers.changeAccuracy(amount);
    displayStatModifyResult(res,amount,"Accuracy");
    return res;
}

bool Battler::changeEvasionModifier(int amount) {
    if(field->hasFieldEffect(MIST,actor) && amount<0){
        handler->displayMsg(monster->getNickname()+"'s stat changes are prevented by Mist!");
        return false;
    }
    if(getAbility() == CLEAR_BODY && amount < 0){
        handler->displayMsg(monster->getNickname()+"'s Clear Body prevents Stat reductions!");
        return false;
    }
    bool res = stat_modifiers.changeEvasion(amount);
    displayStatModifyResult(res,amount,"Evasion");
    if(res && amount < 0 && getAbility()==COMPETITIVE){
        changeSpecialAttackModifier(2);
    }
    if(res && amount < 0 && getAbility()==DEFIANT){
        changeAttackModifier(2);
    }
    return res;
}

bool Battler::changeEvasionModifierForced(int amount) {
    bool res = stat_modifiers.changeEvasion(amount);
    displayStatModifyResult(res,amount,"Evasion");
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
    unsigned int base_attack = monster->getStats().getAtk();
    unsigned int base_modified = base_attack;
    int modifier = getAttackModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_attack;
    }else if(modifier<0){
        base_modified = base_attack * 2 / (-modifier + 2);
    }
    if(getAbility()==HUSTLE)
        base_modified*=1.5;
    if(getAbility()==GUTS && hasPermanentStatus())
        base_modified*=1.5;
    return base_modified;
}

unsigned int Battler::getModifiedDefense()const {
    unsigned int base_defense = monster->getStats().getDef();
    unsigned int base_modified = base_defense;
    int modifier = getDefenseModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_defense;
    }else if(modifier<0){
        base_modified = base_defense * 2 / (-modifier + 2);
    }
    if(getAbility()==MARVEL_SCALE && hasPermanentStatus()){
        base_modified = base_modified * 3 / 2;
    }
    return base_modified;
}

unsigned int Battler::getModifiedSpecialAttack()const {
    unsigned int base_special_attack = monster->getStats().getSpatk();
    unsigned int base_modified = base_special_attack;
    int modifier = getSpecialAttackModifier();
    if(modifier>0){
        base_modified = (100 + modifier*50) / 100 * base_special_attack;
    }else if(modifier<0){
        base_modified = base_special_attack * 2 / (-modifier + 2);
    }
    if(monster->getAbility() == SOLAR_POWER && field->getWeather() == SUN){// SOLAR POWER
        base_modified = base_modified * 3 / 2;
    }
    return base_modified;
}

unsigned int Battler::getModifiedSpecialDefense()const {
    unsigned int base_special_defense = monster->getStats().getSpdef();
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
    return base_modified;
}

unsigned int Battler::getModifiedSpeed()const {
    unsigned int base_speed = monster->getStats().getSpdef();
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
    if(getAbility()==QUICK_FEET && hasPermanentStatus()){
        base_modified =  base_modified * 1.5;
    }else if(monster->getPermanentStatus() == PARALYSIS){
        base_modified = base_modified * 0.5;
    }
    if(monster->getAbility() == CHLOROPHYL && field->getWeather() == SUN){// CHLOROPHYL
        base_modified = base_modified * 2;
    }
    if(monster->getAbility() == SWIFT_SWIM && field->getWeather() == RAIN){// SWIFT SWIM
        base_modified = base_modified * 2;
    }
    if(field->getWeather()==SANDSTORM && getAbility()==SAND_RUSH){//Sand rush doubles speed under sandstorm
        base_modified = base_modified * 2;
    }
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
    if(getAbility() == COMPOUND_EYES)
        modified_accuracy *= 13 / 10;
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
    if(has_ability_neutralized)
        return NO_ABILITY;
    if(has_ability_suppressed)
        return NO_ABILITY;
    if(is_mold_breaker_active && isAbilityIgnorable(battler_ability))
        return NO_ABILITY;
    return battler_ability;
}

bool Battler::hasAbility(Ability ability)const{
    if(getAbility() == ability)
        return true;
    return false;
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

void Battler::setAbility(Ability ability) {
    if(isAbilitySuppressed())
        has_ability_suppressed = false;
    this->battler_ability = ability;
}
bool Battler::canSwitchOut(Battler* enemy)const {
    if(getAbility() == RUN_AWAY)
        return true;
    if(hasVolatileCondition(INGRAINED))
        return false;
    if(hasType(GHOST))
        return true;
    if(enemy != nullptr &&
        enemy->getAbility() == ARENA_TRAP && 
        isTouchingGround())
        return false;
    if(enemy != nullptr &&
        enemy->getAbility() == MAGNET_PULL && 
        hasType(STEEL))
        return false;
    if(hasVolatileCondition(WRAP))
        return false;
    if(hasVolatileCondition(BIND))
        return false;
    if(hasVolatileCondition(FIRESPIN))
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
    if(hasVolatileCondition(MAGNET_RISE))
        return false;
    if(getAbility()==LEVITATE)
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
    if(getAbility() == INSOMNIA)
        return false;
    if(getAbility() == VITAL_SPIRIT)
        return false;
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
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
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
        return false;
    if(hasPermanentStatus())
        return false;
    if(getAbility()==LIMBER)
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
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
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
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
        return false;
    if(getAbility()==WATER_VEIL)
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
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
        return false;
    if(getAbility()==IMMUNITY)
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
    if(field->getWeather()==SUN && getAbility()==LEAF_GUARD)
        return false;
    if(getAbility()==IMMUNITY)
        return false;
    return true;
}

void Battler::landOnGround(){
    // if(hasType(FLYING)){
    //     types.erase(FLYING);
    // }
    addVolatileCondition(GROUNDED,5);
}

void Battler::leaveGround(){
    if(!hasVolatileCondition(GROUNDED))
        return;
    removeVolatileCondition(GROUNDED);
    // if(monster->hasType(FLYING)){
    //     types.insert(FLYING);
    // }
}

bool Battler::hasPermanentStatus()const{
    return monster->getPermanentStatus()!=NO_PERMANENT_CONDITION;
}

unsigned int Battler::getStockpiles()const{
    return stockpiles;
}

void Battler::incrementStockpiles(){
    if(stockpiles < 3){
        changeDefenseModifier(+1);
        changeSpecialDefenseModifier(+1);
        stockpiles++;
    }
}

void Battler::resetStockpiles(){
    if(stockpiles>0){
        changeDefenseModifier(-stockpiles);
        changeSpecialDefenseModifier(-stockpiles);
    }
    stockpiles = 0;
}

void Battler::suppressAbility(){
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
            handler->displayMsg(monster->getNickname()+" can now use "+attack_name+" again!");
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
        return;
    }
    monster->usePP(attack_id,amount);
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

unsigned int Battler::addDamage(unsigned int amount, AttackType category){
    unsigned int currentHP = monster->getCurrentHP();
    if(isAtFullHP() && getAbility()==MULTISCALE){
        handler->displayMsg(monster->getNickname()+"'s Multiscale reduces the damage!");
        amount = max(1,amount / 2);
    }
    if(currentHP != 1 && 
        currentHP < amount &&
        getAbility() == STURDY){
        handler->displayMsg(monster->getNickname()+" endures the hit!");
        amount = currentHP - 1;
    }
    if(currentHP < amount &&
        hasVolatileCondition(ENDURE)){
        handler->displayMsg(monster->getNickname()+" endures the hit!");
        amount = currentHP - 1;
    }
    unsigned int dmg = monster->addDamage(amount);
    if(dmg>0)
        removeVolatileCondition(FOCUSED);
    if(category==PHYSICAL)
        setPhysicalDamageTakenThisTurn(dmg);
    else if(category==SPECIAL)
        setSpecialDamageTakenThisTurn(dmg);
    if(isFainted()){
        handler->displayMsg(monster->getNickname()+" fainted!");
        return dmg;
    }
    //weak armor activation
    if(dmg>0 && getAbility()==WEAK_ARMOR && !isFainted() && category==PHYSICAL){
        changeDefenseModifier(-1);
        changeSpeedModifier(2);
    }
    return dmg;
}

unsigned int Battler::addDirectDamage(unsigned int amount){
    unsigned int dmg = monster->addDamage(amount);
    if(isFainted())
        handler->displayMsg(monster->getNickname()+" fainted!");
    return dmg;
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
            handler->displayMsg(monster->getNickname()+" was burned!");
            break;
        case PARALYSIS:
            if(!canBeParalyzed())
                return false;
            monster->setPermanentStatus(PARALYSIS);
            handler->displayMsg(monster->getNickname()+" was paralyzed!");
            break;
        case FREEZE:
            if(!canBeFrozen())
                return false;
            monster->setPermanentStatus(FREEZE);
            handler->displayMsg(monster->getNickname()+" was frozen!");
            break;
        case POISONED:
            if(!canBePoisoned())
                return false;
            monster->setPermanentStatus(POISONED);
            handler->displayMsg(monster->getNickname()+" was poisoned!");
            break;
        case BAD_POISON:
            if(!canBeBadlyPoisoned())
                return false;
            monster->setPermanentStatus(BAD_POISON);
            bad_poison_counter = 1;
            handler->displayMsg(monster->getNickname()+" was badly poisoned!");
            break;
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            if(!canFallAsleep())
                return false;
            monster->setPermanentStatus(status);
            handler->displayMsg(monster->getNickname()+" fell asleep!");
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
            handler->displayMsg(monster->getNickname()+" was burned!");
            break;
        case PARALYSIS:
            monster->setPermanentStatus(PARALYSIS);
            handler->displayMsg(monster->getNickname()+" was paralyzed!");
            break;
        case FREEZE:
            monster->setPermanentStatus(FREEZE);
            handler->displayMsg(monster->getNickname()+" was frozen!");
            break;
        case POISONED:
            monster->setPermanentStatus(POISONED);
            handler->displayMsg(monster->getNickname()+" was poisoned!");
            break;
        case BAD_POISON:
            monster->setPermanentStatus(BAD_POISON);
            bad_poison_counter = 1;
            handler->displayMsg(monster->getNickname()+" was badly poisoned!");
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
                handler->displayMsg(monster->getNickname()+" fell asleep!");
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
            handler->displayMsg(monster->getNickname()+" was cured of its burn!");
            break;
        case PARALYSIS:
            handler->displayMsg(monster->getNickname()+" was cured of its paralysis!");
            break;
        case FREEZE:
            handler->displayMsg(monster->getNickname()+" thawed out!");
            break;
        case POISONED:
        case BAD_POISON:
            handler->displayMsg(monster->getNickname()+" was cured from the poisoning!");
            break;
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            handler->displayMsg(monster->getNickname()+" woke up!");
            break;
        default:
            return false;
    }
    monster->setPermanentStatus(NO_PERMANENT_CONDITION);
    bad_poison_counter = 0;
    return true;
}

unsigned int Battler::getHeight()const{
    return monster->getHeight();
}

unsigned int Battler::getWeight()const{
    return monster->getWeight();
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
        getAbility() == RATTLED){
        handler->displayMsg(monster->getNickname()+"'s Rattled boosts its Speed!");
        changeSpeedModifier(1);
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
    handler->displayMsg(monster->getNickname()+" is locked on!");
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

bool Battler::useItem(ItemType item_type){
    if(item_type == NO_ITEM_TYPE)
        return false;
    if(isFainted())
        return false;
    // ItemData* item_data = ItemData::getItemData(item_type);
    bool res = monster->useItem(item_type, handler);
    switch (item_type){
        case FULL_RESTORE:
            if(hasVolatileCondition(CONFUSION)){
                removeVolatileCondition(CONFUSION);
                res = true;
            }
            break;
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
        case FULL_RESTORE:
            if(hasVolatileCondition(CONFUSION))
                res = true;
            break;
        default:
            break;
    }   
    return res;
}