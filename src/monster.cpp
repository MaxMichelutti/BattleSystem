#include "monster.h"

// ATTACKDATA ----------------------------------------------------------------------------------------------------------

AttackData::AttackData() {
    attack_id = 0;
    current_pp = 0;
}

AttackData::AttackData(unsigned int attack_id) {
    this->attack_id = attack_id;
    current_pp = Attack::getAttack(attack_id)->getMaxPP();
}

AttackData::AttackData(unsigned int attack_id, unsigned int current_pp){
    this->attack_id = attack_id;
    this->current_pp = current_pp;
}

// MONSTER -------------------------------------------------------------------------------------------------------------    

Monster::Monster(unsigned int species_id) {
    init(species_id, 1);
}

Monster::Monster(unsigned int species_id, unsigned int level) {
    init(species_id, level);
}

void Monster::init(unsigned int species_id, unsigned int level) {
    transformation=nullptr;
    this->species_id = species_id;
    has_hidden_ability = false;
    Species* spec = Species::getSpecies(species_id);

    // init status
    permanent_status = NO_PERMANENT_CONDITION;
    
    // init stats
    this->level = level;
    nature = randomNature();
    experience = getExpForLevel(level, spec->getExpCurve());
    init_stats();    

    // set name
    nickname = spec->getName();

    // set moves
    for (int i = 0; i < 4; i++) {
        attack_ids[i].attack_id = 0;
        attack_ids[i].current_pp = 0;
    }

    // set gender
    init_gender(spec->getGenderRule());

    // set ability
    // choose a random ability
    std::vector<Ability> abilities;
    if(spec->getAbility1() != NO_ABILITY)
        abilities.push_back(spec->getAbility1());
    if(spec->getAbility2() != NO_ABILITY)
        abilities.push_back(spec->getAbility2());
    unsigned int random_index = RNG::getRandomInteger(0,abilities.size()-1);
    ability = abilities[random_index];
}

void Monster::init_gender(GenderRule rule){
    switch(rule){
        case HALF_MALE_HALF_FEMALE:
            if(RNG::coinFlip())
                gender = MALE;
            else
                gender = FEMALE;
            break;
        case ALWAYS_FEMALE:
            gender = FEMALE;
            break;
        case ALWAYS_MALE:
            gender = MALE;
            break;
        case FEMALE_ONE_EIGHT:
            if(RNG::oneEight())
                gender = FEMALE;
            else
                gender = MALE;
            break;
        case MALE_ONE_EIGHT:
            if(RNG::oneEight())
                gender = MALE;
            else
                gender = FEMALE;
            break;
        case FEMALE_ONE_FOURTH:
            if(RNG::oneFourth())
                gender = FEMALE;
            else
                gender = MALE;
            break;
        case MALE_ONE_FOURTH:
            if(RNG::oneFourth())
                gender = MALE;
            else
                gender = FEMALE;
            break;
        case NO_GENDER: 
        default: gender = GENDERLESS;
    }
}

void Monster::init_stats(){
    damage = 0;
    individual = IVs::generateRandomIVs();
    effort = EVs();
    stats = Stats();
    updateStats();
}

Monster::~Monster() {
    if(transformation != nullptr){
        delete transformation;
    }
}

unsigned int Monster::getSpeciesId()const {
    return species_id;
}

Stats Monster::getStats()const {
    if(transformation != nullptr)
        return transformation->getStats();
    return stats;
}

EVs Monster::getEffort()const {
    return effort;
}

IVs Monster::getIndividual()const {
    return individual;
}

Ability Monster::getAbility()const {
    if (transformation != nullptr)
        return transformation->getAbility();
    return ability;
}

std::vector<unsigned int> Monster::getAttackIds()const {
    if(transformation != nullptr)
        return transformation->getAttackIds();
    std::vector<unsigned int> attack_ids_vector;
    for (int i = 0; i < 4; i++) {
        if(attack_ids[i].attack_id != 0)
            attack_ids_vector.push_back(attack_ids[i].attack_id);
    }
    return attack_ids_vector;
}

std::map<unsigned int,unsigned int> Monster::getAttacks()const {
    if(transformation != nullptr)
        return transformation->getAttacks();
    std::map<unsigned int,unsigned int> attacks;
    for (int i = 0; i < 4; i++) {
        if(attack_ids[i].attack_id != 0)
            attacks[attack_ids[i].attack_id] = attack_ids[i].current_pp;
    }
    return attacks;
}

std::string Monster::getNickname()const {
    return nickname;
}

unsigned int Monster::getLevel()const {
    return level;
}

Nature Monster::getNature()const {
    return nature;
}

Gender Monster::getGender()const{
    return gender;
}

Type Monster::getType1()const {
    if(transformation != nullptr)
        return transformation->getType1();
    return Species::getSpecies(species_id)->getType1();
}

Type Monster::getType2()const {
    if(transformation != nullptr)
        return transformation->getType2();
    return Species::getSpecies(species_id)->getType2();
}

unsigned long Monster::getExperience()const {
    return experience;
}

unsigned int Monster::getFriendship()const {
    return friendship;
}

unsigned int Monster::getMaxHP()const {
    return stats.getHp();
}
unsigned int Monster::getCurrentHP()const {
    return stats.getHp() - damage;
}
unsigned int Monster::getDamage()const {
    return damage;
}
PermanentStatusCondition Monster::getPermanentStatus()const {
    return permanent_status;
}
void Monster::setPermanentStatus(PermanentStatusCondition status) {
    permanent_status = status;
}

void Monster::setNickname(std::string nickname) {
    this->nickname = nickname;
}

void Monster::changeFriendship(int amount) {
    if(amount < 0){
        if(friendship == 0)
            return;
        if(friendship < -amount)
            friendship = 0;
        else
            friendship += amount;
    }else if(amount > 0){
        if(friendship == MAX_FRIENDSHIP)
            return;
        if(friendship + amount > MAX_FRIENDSHIP)
            friendship = MAX_FRIENDSHIP;
        else
            friendship += amount;
    }
}

void Monster::changeEffortAtk(int amount) {
    effort.changeAtk(amount);
    updateStats();
}

void Monster::changeEffortDef(int amount) {
    effort.changeDef(amount);
    updateStats();
}

void Monster::changeEffortSpatk(int amount) {
    effort.changeSpatk(amount);
    updateStats();
}

void Monster::changeEffortSpdef(int amount) {
    effort.changeSpdef(amount);
    updateStats();
}

void Monster::changeEffortSpd(int amount) {
    effort.changeSpd(amount);
    updateStats();
}

void Monster::changeEffortHp(int amount) {
    effort.changeHp(amount);
    updateStats();
}

void Monster::updateStats() {
    Stats base = Species::getSpecies(species_id)->getBaseStats();
    unsigned int old_hp = stats.getHp();
    Stats effort = getEffort();
    Stats individual = getIndividual();
    Stats new_stats;
    new_stats.setHp((2 * base.getHp() + individual.getHp() + effort.getHp() / 4) * level / 100 + 10 + level);
    new_stats.setAtk((2 * base.getAtk() + individual.getAtk() + effort.getAtk() / 4) * level / 100 + 5);
    new_stats.setSpatk((2 * base.getSpatk() + individual.getSpatk() + effort.getSpatk() / 4) * level / 100 + 5);
    new_stats.setDef((2 * base.getDef() + individual.getDef() + effort.getDef() / 4) * level / 100 + 5);
    new_stats.setSpdef((2 * base.getSpdef() + individual.getSpdef() + effort.getSpdef() / 4) * level / 100 + 5);
    new_stats.setSpd((2 * base.getSpd() + individual.getSpd() + effort.getSpd() / 4) * level / 100 + 5);
    new_stats.applyNatureBoosts(nature);
    // Shedinja case
    if(base.getHp() == 1)
        new_stats.setHp(1);
    // prevent Pomeg Berry glitch
    if(old_hp > new_stats.getHp()){
        unsigned int old_hp_left = old_hp - damage;
        damage = new_stats.getHp() - old_hp_left;
    }
    stats = new_stats;
    if(transformation != nullptr)
        new_stats.setHp(getMaxHP());
}

void Monster::packAttacks() {
    // pack the attacks array to the left
    std::vector<AttackData> attack_ids_vector;
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id != 0)
            attack_ids_vector.push_back(attack_ids[i]);
    }
    for(int i=0;i<4;i++){
        attack_ids[i].attack_id = 0;
        attack_ids[i].current_pp = 0;
    }
    unsigned int count = 0;
    for(AttackData attack: attack_ids_vector){
        if(attack.attack_id == 0)
            continue;
        attack_ids[count].attack_id = attack.attack_id;  
        attack_ids[count++].current_pp = attack.current_pp;
    }
}

bool Monster::hasAttack(unsigned int attack_id)const {
    if(transformation != nullptr)
        return transformation->hasAttack(attack_id);
    if(attack_id == 0)
        return false;
    for (int i = 0; i < 4; i++) {
        if(attack_ids[i].attack_id == attack_id)
            return true;
    }
    return false;
}

bool Monster::learnAttack(unsigned int attack_id) {
    // cannot learn an attack if it already knows it
    if (hasAttack(attack_id))
        return false;
    // fill the first empty slot with the attack
    for (int i = 0; i < 4; i++) {
        if(attack_ids[i].attack_id == 0){
            attack_ids[i].attack_id = attack_id;
            attack_ids[i].current_pp = Attack::getAttack(attack_id)->getMaxPP();
            packAttacks();
            return true;
        }
    }
    // if no slots are empty, return false
    return false;
}

bool Monster::forgetAttack(unsigned int attack_id) {
    // need to know the attack to forget
    if(!hasAttack(attack_id))
        return false;
    // cannot forget an attack if there are no attacks left
    if(attack_ids[1].attack_id == 0)
        return false;
    // this cycle is guaranteed to exit at the return true statement
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id == attack_id){
            attack_ids[i].attack_id = 0;
            attack_ids[i].current_pp = 0;
            packAttacks();
            return true;
        }
    }
    // should never reach this point
    return true;
}

void Monster::gainExperience(unsigned long exp) {
    experience += exp;
    unsigned int level_after_exp_gain = getLevelFromExp(experience, Species::getSpecies(species_id)->getExpCurve());
    while(level < level_after_exp_gain){
        levelUp();
    }
}

void Monster::levelUp() {
    level++;
    updateStats();
    // TODO(): allow for learning new moves when levelling up
}

Monster* Monster::generateRandomMonster(unsigned int species_id) {
    return generateRandomMonster(species_id, 1);
}

Monster* Monster::generateRandomMonster(unsigned int species_id, unsigned int level) {
    Monster* monster = new Monster(species_id, level);
    // choose up to 4 moves from learnset
    Species* species = Species::getSpecies(species_id);
    std::set<unsigned int> learnset = species->getLearnsetUntil(monster->getLevel());
    for(int i=0;i<4;i++){
        std::vector<unsigned int> learnset_vector(learnset.begin(), learnset.end());
        if(learnset_vector.size() == 0)
            break;
        unsigned int attack_id = learnset_vector[RNG::getRandomInteger(0,learnset_vector.size()-1)];
        monster->learnAttack(attack_id);
        learnset.erase(attack_id);
    }
    return monster;
}

void Monster::printSummary()const{
    std::cout << "Species: " << Species::getSpecies(species_id)->getName() << std::endl;
    std::cout << "Nickname: " << nickname << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Experience: " << experience << std::endl;
    std::cout << "Nature: " << natureToString(nature) << std::endl;
    std::cout << "Ability: " << abilityToString(ability) << std::endl;
    std::cout << "Friendship: " << friendship << std::endl;
    std::cout << "Permanent Status: " << permanentStatusConditionToString(permanent_status) << std::endl;
    std::cout << "Type(s):" << typeToString(Species::getSpecies(species_id)->getType1());
    if(Species::getSpecies(species_id)->getType2() != NO_TYPE)
        std::cout << " " << typeToString(Species::getSpecies(species_id)->getType2());
    std::cout << std::endl << "Stats: " << std::endl;
    std::cout << "HP: " << stats.getHp()
    << " ATK: " << stats.getAtk()
    << " DEF: " << stats.getDef()
    << " SPATK: " << stats.getSpatk()
    << " SPDEF: " << stats.getSpdef() 
    << " SPD: " << stats.getSpd() << std::endl;
    std::cout << "Effort Values: " << std::endl;
    std::cout << "HP: " << effort.getHp()
    << " ATK: " << effort.getAtk()
    << " DEF: " << effort.getDef()
    << " SPATK: " << effort.getSpatk()
    << " SPDEF: " << effort.getSpdef() 
    << " SPD: " << effort.getSpd() << std::endl;
    std::cout << "Individual Values: " << std::endl;
    std::cout << "HP: " << individual.getHp()
    << " ATK: " << individual.getAtk()
    << " DEF: " << individual.getDef()
    << " SPATK: " << individual.getSpatk()
    << " SPDEF: " << individual.getSpdef() 
    << " SPD: " << individual.getSpd() << std::endl;
    std::cout << "Gender: " << genderToString(gender) << std::endl;
    std::cout << "Attacks: " << std::endl;
    for(int i=0;i<4;i++){
        unsigned int attack_id = attack_ids[i].attack_id;
        unsigned int current_pp = attack_ids[i].current_pp;
        if(attack_id == 0)
            continue;
        Attack* attack = Attack::getAttack(attack_id);
        std::cout << attack->getName() << " -> " << attack->getDescription() << std::endl;
        std::cout << "Type: " << typeToString(attack->getType());
        unsigned int power = attack->getPower();
        if(power != 0)
            std::cout << " Power: " << power;
        else
            std::cout << " Power: -";
        unsigned int accuracy = attack->getAccuracy();
        if(accuracy != ALWAYS_HITS)
            std::cout << " Accuracy: " << accuracy;
        else
            std::cout << " Accuracy: -";
        std::cout << " PP: " <<current_pp<<"/"<< attack->getMaxPP()
        << " Priority: " << attack->getPriorityLevel() 
        << " Category: "<< attackTypeToString(attack->getCategory())<<std::endl;

    }
}

bool Monster::canEvolve()const{
    Species* spec = Species::getSpecies(species_id);
    for(const Evolution &evo: spec->getEvolutions()){
        switch(evo.getEvolutionMethod()){
            case LEVEL:
                if(level >= evo.getMethodCondition())
                    return true;
            default: continue;
        }
    }
    return false;
}

void Monster::evolve(){
    if(!canEvolve())
        return;
    Species* spec = Species::getSpecies(species_id);
    for(const Evolution& evo: spec->getEvolutions()){
        switch(evo.getEvolutionMethod()){
            case LEVEL:
                if(level >= evo.getMethodCondition()){
                    completeEvolution(evo.getTargetSpeciesId());
                    return;
                }
            default: continue;
        }
    }
}

void Monster::completeEvolution(unsigned int target_species_id){
    Species* old_spec = Species::getSpecies(species_id);
    Species* new_spec = Species::getSpecies(target_species_id);
    this->species_id = target_species_id;
    // rename not-nicknamed monsters
    if(nickname == old_spec->getName()){
        nickname = new_spec->getName();
    }
    //update stats
    updateStats();
    // update ability
    if(has_hidden_ability)
        ability = new_spec->getHiddenAbility();
    else{
        Ability old1 = old_spec->getAbility1();
        Ability new1 = new_spec->getAbility1();
        Ability old2 = old_spec->getAbility2();
        Ability new2 = new_spec->getAbility2();
        if(ability==old1)
            ability=new1;
        else if(ability==old2)
            ability=new2;
        //else
        //  ability is the same as before
    }
    // TODO: learn attacks
    // auto evo_attacks = new_spec->getEvolutionLearnset();
    // for(auto it = evo_attacks.begin(); it != evo_attacks.end(); it++){
    //     unsigned int attack_id = *it;
    //     if(!hasAttack(attack_id)){
    //         learnAttack(attack_id);
    //     }
    // }
}

void Monster::heal(){
    damage = 0;
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id != 0)
            attack_ids[i].current_pp = Attack::getAttack(attack_ids[i].attack_id)->getMaxPP();
    }
    permanent_status = NO_PERMANENT_CONDITION;
}

bool Monster::isFainted()const{
    return permanent_status == KO;
}

void Monster::usePP(unsigned int attack_id, unsigned int amount){
    if(transformation != nullptr){
        transformation->usePP(attack_id,amount);
        return;
    }
    if(attack_id == STRUGGLE_ID)
        return;
    if(!hasAttack(attack_id))
        return;
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id == attack_id){
            if(attack_ids[i].current_pp < amount)
                amount = attack_ids[i].current_pp;
            attack_ids[i].current_pp -= amount;
            return;
        }
    }
}

bool Monster::hasPP(unsigned int attack_id)const{
    if(transformation != nullptr)
        return transformation->hasPP(attack_id);
    if(attack_id == STRUGGLE_ID)
        return true;
    if(!hasAttack(attack_id))
        return false;
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id == attack_id){
            if(attack_ids[i].current_pp == 0)
                return false;
            return true;
        }
    }
    return false;
}

void Monster::recoverPP(unsigned int attack_id, unsigned int amount){
    if(transformation != nullptr){
        transformation->recoverPP(attack_id,amount);
        return;
    }
    if(!hasAttack(attack_id))
        return;
    Attack* attack = Attack::getAttack(attack_id);
    unsigned int max_pp = attack->getMaxPP();
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id == attack_id){
            unsigned int current_pp = attack_ids[i].current_pp;
            if(current_pp + amount > max_pp)
                amount = max_pp - current_pp;
            attack_ids[i].current_pp += amount;
            return;
        }
    }
}


unsigned int Monster::addDamage(unsigned int damage_to_deal){
    unsigned int old_health = getCurrentHP();
    this->damage += damage_to_deal;
    if(this->damage >= stats.getHp()){
        this->damage = stats.getHp();
        setPermanentStatus(KO);
        return old_health;
    }
    return damage_to_deal;
}

bool Monster::hasType(Type type)const{
    if(transformation != nullptr)
        return transformation->hasType(type);
    if(type == NO_TYPE)
        return false;
    if(type == getType1() || type == getType2())
        return true;
    return false;
}

unsigned int Monster::removeDamage(unsigned int damage_to_remove){
    unsigned int result = damage_to_remove;
    if(this->damage < damage_to_remove){
        result = this->damage;
        this->damage = 0;
    }else
        this->damage -= damage_to_remove;
    // revive if dead
    if(permanent_status == KO)
        setPermanentStatus(NO_PERMANENT_CONDITION);
    return result;
}

bool Monster::isAtFullHP()const{
    return damage == 0;
}

unsigned int Monster::getMaxPPForAttack(unsigned int attack_id)const{
    if(attack_id == STRUGGLE_ID || attack_id == 0)
        return 0;
    if(transformation != nullptr)
        return 5;
    Attack* attack = Attack::getAttack(attack_id);
    return ((attack==nullptr)?0:attack->getMaxPP());
}

void Monster::clearBattleData(){
    if(transformation != nullptr){
        delete transformation;
        transformation = nullptr;
    }
}

void Monster::transformInto(Monster* other){
    transformation = new Monster(other->getSpeciesId(),getLevel());
    // add copied moves with 5 PP each
    transformation->attack_ids[0].attack_id = other->attack_ids[0].attack_id;
    transformation->attack_ids[1].attack_id = other->attack_ids[1].attack_id;
    transformation->attack_ids[2].attack_id = other->attack_ids[2].attack_id;
    transformation->attack_ids[3].attack_id = other->attack_ids[3].attack_id;
    transformation->attack_ids[0].current_pp = 5;
    transformation->attack_ids[1].current_pp = 5;
    transformation->attack_ids[2].current_pp = 5;
    transformation->attack_ids[3].current_pp = 5;
    // keep gender
    transformation->gender = getGender();
    // steal ability
    transformation->ability = other->getAbility();
    // steal all stats...
    transformation->stats = other->getStats();
    // but keep Max HP
    transformation->stats.setHp(getMaxHP());
    // keep IVs and EVs
    transformation->effort = getEffort();
    transformation->individual = getIndividual();
    // keep Nature
    transformation->nature = getNature();
    // keep friendship value
    transformation->friendship = getFriendship();
}

unsigned int Monster::getHeight()const{
    if(transformation != nullptr)
        return transformation->getHeight();
    return Species::getSpecies(species_id)->getHeight();
}

unsigned int Monster::getWeight()const{
    if(transformation != nullptr)
        return transformation->getWeight();
    return Species::getSpecies(species_id)->getWeight();
}

bool Monster::useItem(ItemType item_type, EventHandler* handler, unsigned int data){
    if(item_type == NO_ITEM_TYPE)
        return false;
    bool result = false;
    if(!isFainted()){
        // HP restoring items
        unsigned int amount = 0;
        switch (item_type){
            case ORAN_BERRY:
                amount=10;
                break;
            case SITRUS_BERRY:
            case ENIGMA_BERRY:
                amount=max(1,getMaxHP()/4);
                break;
            case AGUAV_BERRY:
            case FIGY_BERRY:
            case WIKI_BERRY:
            case IAPAPA_BERRY:
            case MAGO_BERRY:
                amount=max(1,getMaxHP()/3);
                break;
            case POTION:
                amount=20;
                break;
            case SUPER_POTION:
                amount=60;
                break;
            case HYPER_POTION:
                amount=120;
                break;
            case MAX_POTION:
            case FULL_RESTORE:
                amount=getMaxHP();
                break;
            default:
                break;
        }
        unsigned int actual_heal_amount = removeDamage(amount);
        if(actual_heal_amount>0){
            result = true;
            handler->displayMsg(getNickname()+" was healed of " + std::to_string(actual_heal_amount) + " HP!");
        }
        // status restoring items
        switch(item_type){
            case ANTIDOTE:
            case PECHA_BERRY:
                if(permanent_status == POISONED || permanent_status == BAD_POISON){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its poisoning!");
                }
                break;
            case AWAKENING:
            case CHESTO_BERRY:
                if(permanent_status == SLEEP_1 || permanent_status == SLEEP_2 || 
                    permanent_status == SLEEP_3 || permanent_status == SLEEP_4){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" woke up!");
                }
                break;
            case PARALYZE_HEAL:
            case CHERY_BERRY:
                if(permanent_status == PARALYSIS){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its paralysis!");
                }
                break;
            case BURN_HEAL:
            case RAWST_BERRY:
                if(permanent_status == BURN){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its burn!");
                }
                break;
            case ICE_HEAL:
            case ASPEAR_BERRY:
                if(permanent_status == FREEZE){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its freeze!");
                }
                break;
            case FULL_RESTORE:
            case FULL_HEAL:
            case LUM_BERRY:
                if(permanent_status != NO_PERMANENT_CONDITION){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its status condition!");
                }
                break;
            default:
                break;
        }
        // PP Restoring items
        switch(item_type){
            case LEPPA_BERRY:
                if(data!=0)
                    recoverPP(data,10);
                else{
                    // if no attack is specified, recover PP for the first attack available
                    for(int i=0;i<4;i++){
                        if(attack_ids[i].attack_id != 0){
                            unsigned int attack_id = attack_ids[i].attack_id;
                            unsigned int current_pp = attack_ids[i].current_pp;
                            unsigned int max_pp = getMaxPPForAttack(attack_id);
                            if(max_pp > current_pp){
                                recoverPP(attack_id,10);
                                result = true;
                                break;
                            }
                        }
                    }
                }
                result=true;
                break;
            default:break;
        }
        // friendship berries
        switch(item_type){
            case GREPA_BERRY:{
                if(effort.getSpdef() > 0){
                    effort.changeSpdef(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                }
                break;
            }
            case HONDEW_BERRY:{
                if(effort.getSpatk() > 0){
                    effort.changeSpatk(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                }
                break;
            }
            case KELPSY_BERRY:{
                if(effort.getAtk() > 0){
                    effort.changeAtk(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                }
                break;
            }
            case QUALOT_BERRY:{
                if(effort.getDef() > 0){
                    effort.changeDef(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                }
                break;
            }
            case TAMATO_BERRY:{
                if(effort.getSpd() > 0){
                    effort.changeSpd(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                }
                break;
            }
            case POMEG_BERRY:{
                if(effort.getHp() > 0){
                    unsigned int old_hp = getCurrentHP();
                    effort.changeHp(-10);
                    if(friendship < 100)
                        changeFriendship(10);
                    else if(friendship<200)
                        changeFriendship(5);
                    else
                        changeFriendship(2);
                    updateStats();
                    result = true;
                    //avoid pomeg berry glitch
                    if(isFainted())
                        damage = getMaxHP();
                    else{
                        if(old_hp >= getMaxHP()){
                            damage = 0;
                        }else{
                            damage = getMaxHP() - old_hp;
                        }
                    }
                }
                break;
            }
            default:break;
        }
    }
    return result;
}

bool Monster::itemWouldHaveEffect(ItemType item_type)const{
    if(item_type == NO_ITEM_TYPE)
        return false;
    bool result = false;
    if(!isFainted()){
        //healing items
        switch(item_type){
            case POTION:
            case SUPER_POTION:
            case HYPER_POTION:
            case MAX_POTION:
            case FULL_RESTORE:
            case ORAN_BERRY:
            case SITRUS_BERRY:
            case ENIGMA_BERRY:
            case IAPAPA_BERRY:
            case MAGO_BERRY:
            case AGUAV_BERRY:
            case WIKI_BERRY:
            case FIGY_BERRY:
                if(getCurrentHP() < getMaxHP())
                    result = true;
                break;
            default:
                break;
        }
        // status restoring items
        switch(item_type){
            case ANTIDOTE:
            case PECHA_BERRY:
                if(permanent_status == POISONED || permanent_status == BAD_POISON)
                    result = true;
                break;
            case AWAKENING:
            case CHESTO_BERRY:
                if(permanent_status == SLEEP_1 || permanent_status == SLEEP_2 || 
                    permanent_status == SLEEP_3 || permanent_status == SLEEP_4)
                    result = true;
                break;
            case PARALYZE_HEAL:
            case CHERY_BERRY:
                if(permanent_status == PARALYSIS)
                    result = true;
                break;
            case BURN_HEAL:
            case RAWST_BERRY:
                if(permanent_status == BURN)
                    result = true;
                break;
            case ICE_HEAL:
            case ASPEAR_BERRY:
                if(permanent_status == FREEZE)
                    result = true;
                break;
            case FULL_RESTORE:
            case FULL_HEAL:
            case LUM_BERRY:
                if(permanent_status != NO_PERMANENT_CONDITION)
                    result = true;
                break;
            default:
                break;
        }
        // PP restoring items
        switch(item_type){
            case LEPPA_BERRY:
                for(int i=0; i<4;i++){
                    if(attack_ids[i].attack_id != 0){
                        unsigned int attack_id = attack_ids[i].attack_id;
                        unsigned int current_pp = attack_ids[i].current_pp;
                        unsigned int max_pp = getMaxPPForAttack(attack_id);
                        if(max_pp > current_pp){
                            result = true;
                            break;
                        }
                    }
                }
                break;
            default:break;
        }
        //friendship berries
        switch(item_type){
            case GREPA_BERRY:{
                if(effort.getSpdef() > 0)
                    result = true;
                break;
            }
            case HONDEW_BERRY:{
                if(effort.getSpatk() > 0)
                    result = true;
                break;
            }
            case KELPSY_BERRY:{
                if(effort.getAtk() > 0)
                    result = true;
                break;
            }
            case POMEG_BERRY:{
                if(effort.getHp() > 0)
                    result = true;
                break;
            }
            case QUALOT_BERRY:{
                if(effort.getDef() > 0)
                    result = true;
                break;
            }
            case TAMATO_BERRY:{
                if(effort.getSpd() > 0)
                    result = true;
                break;
            }
            default:break;
        }

    }
    return result;   
}

bool Monster::hasHeldItem()const{
    return held_item != NO_ITEM_TYPE;
}
ItemType Monster::getHeldItem()const{
    return held_item;
}
ItemType Monster::setHeldItem(ItemType item){
    ItemType old_held_item = getHeldItem();
    held_item = item;
    return old_held_item;
}
ItemType Monster::removeHeldItem(){
    ItemType old_held_item = getHeldItem();
    held_item = NO_ITEM_TYPE;
    return old_held_item;
}

bool Monster::dislikesBerry(ItemType item)const{
    if(item == NO_ITEM_TYPE)
        return false;
    ItemData* item_data = ItemData::getItemData(item);
    if(item_data == nullptr)
        return false;
    if(item_data->getCategory() != BERRY)
        return false;
    switch(getNature()){
        case HARDY:
        case BOLD:
        case TIMID:
        case MODEST:
        case CALM:
            if(item_data->getFlavour(0)>0)
                return true;
            break;
        case LONELY:
        case DOCILE:
        case HASTY:
        case GENTLE:
        case MILD:
            if(item_data->getFlavour(1)>0)
                return true;
            break;
        case BRAVE:
        case RELAXED:
        case SERIOUS:
        case QUIET:
        case SASSY:
            if(item_data->getFlavour(2)>0)
                return true;
            break;
        case ADAMANT:
        case IMPISH:
        case JOLLY:
        case BASHFUL:
        case CAREFUL:
            if(item_data->getFlavour(3)>0)
                return true;
            break;
        case NAUGHTY:
        case LAX:
        case NAIVE:
        case RASH:
        case QUIRKY:
            if(item_data->getFlavour(4)>0)
                return true;
            break;
    }
    return false;
}

bool Monster::likesBerry(ItemType item)const{
    if(item == NO_ITEM_TYPE)
        return false;
    ItemData* item_data = ItemData::getItemData(item);
    if(item_data == nullptr)
        return false;
    if(item_data->getCategory() != BERRY)
        return false;
    switch(getNature()){
        case HARDY:
        case LONELY:
        case BRAVE:
        case ADAMANT:
        case NAUGHTY:
            if(item_data->getFlavour(0)>0)
                return true;
            break;
        case BOLD:
        case DOCILE:
        case RELAXED:
        case IMPISH:
        case LAX:
            if(item_data->getFlavour(1)>0)
                return true;
            break;
        case TIMID:
        case HASTY:
        case SERIOUS:
        case JOLLY:
        case NAIVE:
            if(item_data->getFlavour(2)>0)
                return true;
            break;
        case MODEST:
        case MILD:
        case QUIET:
        case BASHFUL:
        case RASH:
            if(item_data->getFlavour(3)>0)
                return true;
            break;
        case CALM:
        case GENTLE:
        case QUIRKY:
        case SASSY:
        case CAREFUL:
            if(item_data->getFlavour(4)>0)
                return true;
            break;
    }
    return false;
}