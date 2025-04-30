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
    init(species_id, 1,0);
}

Monster::Monster(unsigned int species_id, unsigned int level) {
    init(species_id, level,0);
}

Monster::Monster(unsigned int species_id, unsigned int level, unsigned int form_id) {
    init(species_id, level, form_id);
}

Monster::Monster(Monster* other){
    transformation = nullptr;
    held_item = other->getHeldItem();
    mega_ability = NO_ABILITY;
    is_mega = false;
    has_hidden_ability = other->has_hidden_ability;
    species_id = other->getSpeciesId();
    form_id = other->getFormId();
    permanent_status = other->getPermanentStatus();
    level = other->getLevel();
    nature = other->getNature();
    gender = other->getGender();
    experience = other->getExperience();
    nickname = other->getNickname();
    stats = other->getStats();
    individual = other->getIndividual();
    effort = other->getEffort();
    for (int i = 0; i < 4; i++) {
        attack_ids[i].attack_id = other->attack_ids[i].attack_id;
        attack_ids[i].current_pp = other->attack_ids[i].current_pp;
    }
    ability = other->getAbility();
    hidden_power_type = other->getHiddenPowerType();
    damage = other->getDamage();
    friendship = other->getFriendship();
    seen_opponents.clear();
    consumed_item = NO_ITEM_TYPE;
    ball_containing_monster = POKE_BALL;
    updateStats();
}


void Monster::init(unsigned int species_id, unsigned int level,unsigned int form_id) {
    transformation=nullptr;
    held_item = NO_ITEM_TYPE;
    mega_ability = NO_ABILITY;
    is_mega = false;
    has_hidden_ability = false;
    ball_containing_monster = POKE_BALL;
    Species* spec = Species::getSpecies(species_id);
    this->species_id = species_id;
    if(form_id != 0 && !spec->hasForm(form_id)){
        //error
        std::cerr << "Error: Form " << form_id << " does not exist for species " << species_id << std::endl;
        exit(DATA_ERROR);
    }
    this->form_id = form_id;

    // init status
    permanent_status = NO_PERMANENT_CONDITION;
    
    // init stats
    this->level = level;
    nature = randomNature();
    experience = getExpForLevel(level, spec->getExpCurve(getFormId()));
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
    if(spec->getAbility1(getFormId()) != NO_ABILITY)
        abilities.push_back(spec->getAbility1(getFormId()));
    if(spec->getAbility2(getFormId()) != NO_ABILITY)
        abilities.push_back(spec->getAbility2(getFormId()));
    unsigned int random_index = RNG::getRandomInteger(0,abilities.size()-1);
    ability = abilities[random_index];

    //set random hidden power type
    hidden_power_type = static_cast<Type>(RNG::getRandomInteger(1,18));
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

bool Monster::isTransformed()const{
    return transformation != nullptr;
}

unsigned int Monster::getTransformedSpeciesId()const{
    if(transformation == nullptr)
        return 0;
    return transformation->getSpeciesId();
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
    if (mega_ability != NO_ABILITY)
        return mega_ability;
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
    return Species::getSpecies(species_id)->getType1(getFormId());
}

Type Monster::getType2()const {
    if(transformation != nullptr)
        return transformation->getType2();
    return Species::getSpecies(species_id)->getType2(getFormId());
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
        if((int)friendship < -amount)
            friendship = 0;
        else
            friendship += amount;
    }else if(amount > 0){
        if(ball_containing_monster==LUXURY_BALL)
            amount += 1;
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
    Stats base = Species::getSpecies(species_id)->getBaseStats(getFormId());
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

bool Monster::learnAttackForced(unsigned int attack_id,unsigned int slot){
    // cannot learn an attack if it already knows it
    if (hasAttack(attack_id))
        return false;
    // fill the first empty slot with the attack
    if(slot>3)
        return false;
    attack_ids[slot].attack_id = attack_id;
    attack_ids[slot].current_pp = Attack::getAttack(attack_id)->getMaxPP();
    packAttacks();
            
    // if no slots are empty, return false
    return true;
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

bool Monster::replaceAttack(unsigned int old_attack_id, unsigned int new_attack_id){
    if(hasAttack(new_attack_id))
        return false;
    if(!hasAttack(old_attack_id))
        return false;
    if(new_attack_id==0 || new_attack_id==STRUGGLE_ID)
        return false;
    for(int i=0;i<4;i++){
        if(attack_ids[i].attack_id == old_attack_id){
            attack_ids[i].attack_id = new_attack_id;
            attack_ids[i].current_pp = Attack::getAttack(new_attack_id)->getMaxPP();
            packAttacks();
            return true;
        }
    }
    return false;
}

void Monster::gainExperience(unsigned long exp,EventHandler* handler) {
    if(level >= MAX_LEVEL)//stop gaining experience at max level
        return;
    experience += exp;
    unsigned int level_after_exp_gain = getLevelFromExp(experience, Species::getSpecies(species_id)->getExpCurve(getFormId()));
    while(level < level_after_exp_gain){
        levelUp(handler);
    }
}

void Monster::levelUp(EventHandler* event_handler) {
    level++;
    updateStats();
    // TODO(): allow for learning new moves when levelling up
}

Monster* Monster::generateRandomMonster(unsigned int species_id) {
    return generateRandomMonster(species_id, 1);
}

Monster* Monster::generateRandomMonster(unsigned int species_id,unsigned int level) {
    return generateRandomMonster(species_id, level, 0);
}

unsigned int Monster::getFormId()const{
    if(transformation != nullptr)
        return transformation->getFormId();
    return form_id;
}

Monster* Monster::generateRandomMonster(unsigned int species_id, unsigned int level,unsigned int form_id) {
    Monster* monster = new Monster(species_id, level,form_id);
    // choose up to 4 moves from learnset
    Species* species = Species::getSpecies(species_id);
    std::set<unsigned int> learnset = species->getLearnsetUntil(0,monster->getLevel());
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
    std::cout << "Type(s):" << typeToString(Species::getSpecies(species_id)->getType1(getFormId()));
    if(Species::getSpecies(species_id)->getType2(getFormId()) != NO_TYPE)
        std::cout << " " << typeToString(Species::getSpecies(species_id)->getType2(getFormId()));
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
        // unsigned int max_pp = attack_ids[i]
        if(attack_id == 0)
            continue;
        Attack* attack = Attack::getAttack(attack_id);
        attack->printSummary(current_pp);
    }
}

Type Monster::getHiddenPowerType()const{
    return hidden_power_type;
}

bool Monster::canEvolve(MonsterTeam* monster_team)const{
    Species* spec = Species::getSpecies(species_id);
    for(const Evolution &evo: spec->getEvolutions(getFormId())){
        switch(evo.getEvolutionMethod()){
            case LEVEL:
            case SHEDINJA:
            case CASCOON_SILCOON:{
                if(level >= evo.getMethodCondition())
                    return true;
                break;
            }
            case LEVEL_NIGHT:{
                if(level >= evo.getMethodCondition() && isNight())
                    return true;
                break;
            }
            case LEVEL_DAY:{
                if(level >= evo.getMethodCondition() && isDay())
                    return true;
                break;
            }
            case LEVEL_ATK:{
                if(level >= evo.getMethodCondition() && stats.getAtk() > stats.getDef())
                    return true;
                break;
            }
            case LEVEL_DEF:{
                if(level >= evo.getMethodCondition() && stats.getDef() > stats.getAtk())
                    return true;
                break;
            }
            case LEVEL_MALE:{
                if(level >= evo.getMethodCondition() && gender==MALE)
                    return true;
                break;
            }
            case LEVEL_FEMALE:{
                if(level >= evo.getMethodCondition() && gender==FEMALE)
                    return true;
                break;
            }
            case LEVEL_EQUAL_ATK_DEF:{
                if(level >= evo.getMethodCondition() && stats.getDef() == stats.getAtk())
                    return true;
                break;
            }
            case FRIENDSHIP_NIGHT:{
                if(isNight() && friendship >= evo.getMethodCondition())
                    return true;
                break;
            }
            case FRIENDSHIP_DAY:{
                if(isDay() && friendship >= evo.getMethodCondition())
                    return true;
                break;
            }
            case FRIENDSHIP:{
                if(friendship >= evo.getMethodCondition())
                    return true;
                break;
            }
            case KNOW_ATTACK:{
                if(hasAttack(evo.getMethodCondition()))
                    return true;
                break;
            }
            case HELD_ITEM_DAY:{
                if(getHeldItem() == static_cast<ItemType>(evo.getMethodCondition()) && isDay())
                    return true;
                break;
            }
            case HELD_ITEM_NIGHT:{
                if(getHeldItem() == static_cast<ItemType>(evo.getMethodCondition()) && isNight())
                    return true;
                break;
            }
            case HAS_IN_TEAM:{
                for(unsigned int i=0;i<monster_team->getSize();i++){
                    Monster* monster = monster_team->getMonster(i);
                    if(monster->getSpeciesId() == evo.getMethodCondition())
                        return true;
                }
                break;
            }
            default: break;
        }
    }
    return false;
}

bool Monster::hasEvolutions()const{
    Species* spec = Species::getSpecies(species_id);
    return spec->getEvolutions(getFormId()).size() > 0;
}

void Monster::evolve(MonsterTeam* monster_team){
    if(!canEvolve(monster_team))
        return;
    Species* spec = Species::getSpecies(species_id);
    bool already_flipped = false;
    for(const Evolution& evo: spec->getEvolutions(getFormId())){
        switch(evo.getEvolutionMethod()){
            case LEVEL:{
                if(level >= evo.getMethodCondition()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_MALE:{
                if(level >= evo.getMethodCondition() && gender==MALE){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_FEMALE:{
                if(level >= evo.getMethodCondition() && gender==FEMALE){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_NIGHT:{
                if(level >= evo.getMethodCondition() && isNight()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_DAY:{
                if(level >= evo.getMethodCondition() && isDay()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_ATK:{
                if(level >= evo.getMethodCondition() && stats.getAtk() > stats.getDef()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_DEF:{
                if(level >= evo.getMethodCondition() && stats.getDef() > stats.getAtk()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case LEVEL_EQUAL_ATK_DEF:{
                if(level >= evo.getMethodCondition() && stats.getDef() == stats.getAtk()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case KNOW_ATTACK:{
                if(hasAttack(evo.getMethodCondition())){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case FRIENDSHIP:{
                if(friendship >= evo.getMethodCondition()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case FRIENDSHIP_NIGHT:{
                if(friendship >= evo.getMethodCondition() && isNight()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case FRIENDSHIP_DAY:{
                if(friendship >= evo.getMethodCondition() && isDay()){
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                break;
            }
            case CASCOON_SILCOON:{
                if(level >= evo.getMethodCondition()){
                    if(!already_flipped){
                        already_flipped = true;
                        if(RNG::coinFlip()){
                            completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                            return;
                        }
                    }else{
                        completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                        return;
                    }
                }
                break;
            }
            case SHEDINJA:{
                if(level >= evo.getMethodCondition()){
                    // first try to add Shedinja to the team
                    Player* player = Player::getPlayer();
                    MonsterTeam* team = player->getTeam();
                    Bag* bag = player->getBag();
                    if(!team->isFull() && bag->hasItem(POKE_BALL)){
                        //Shedinja can be gained if there is space in the team and if a Poke Ball from the bag can be used
                        bag->removeItem(POKE_BALL);
                        Monster* cloned_nincada = new Monster(this);
                        cloned_nincada->setNickname("Shedinja");
                        cloned_nincada->setHeldItem(NO_ITEM_TYPE);
                        cloned_nincada->transformation = nullptr;
                        cloned_nincada->mega_ability = NO_ABILITY;
                        cloned_nincada->form_id = 0;
                        cloned_nincada->damage = 0;
                        cloned_nincada->setPermanentStatus(NO_PERMANENT_CONDITION);
                        cloned_nincada->is_mega = false;
                        cloned_nincada->evolveIntoShedinja();
                        team->addMonster(cloned_nincada);
                    }
                    // then evolve into Ninjask
                    completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                    return;
                }
                case HELD_ITEM_DAY:{
                    if(getHeldItem() == static_cast<ItemType>(evo.getMethodCondition()) && isDay()){
                        removeHeldItem();
                        completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                        return;
                    }
                    break;
                }
                case HELD_ITEM_NIGHT:{
                    if(getHeldItem() == static_cast<ItemType>(evo.getMethodCondition()) && isNight()){
                        removeHeldItem();
                        completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                        return;
                    }
                    break;
                }
                case HAS_IN_TEAM:{
                    for(unsigned int i=0;i<monster_team->getSize();i++){
                        Monster* monster = monster_team->getMonster(i);
                        if(monster->getSpeciesId() == evo.getMethodCondition()){
                            completeEvolution(evo.getTargetSpeciesId(),evo.getTargetFormId());
                            return;
                        }
                    }
                    break;
                }
                break;
            }
            default: continue;
        }
    }
}

void Monster::completeEvolution(unsigned int target_species_id,unsigned int target_form_id){
    Species* old_spec = Species::getSpecies(species_id);
    Species* new_spec = Species::getSpecies(target_species_id);
    this->species_id = target_species_id;
    // rename not-nicknamed monsters
    if(nickname == old_spec->getName()){
        nickname = new_spec->getName();
    }
    unsigned int new_form_id = target_form_id;
    unsigned int old_form_id = getFormId();

    //update stats
    updateStats();
    // update ability
    if(has_hidden_ability)
        ability = new_spec->getHiddenAbility(new_form_id);
    else{
        Ability old1 = old_spec->getAbility1(old_form_id);
        Ability new1 = new_spec->getAbility1(new_form_id);
        Ability old2 = old_spec->getAbility2(old_form_id);
        Ability new2 = new_spec->getAbility2(new_form_id);
        if(ability==old1 || new2 == NO_ABILITY)
            ability=new1;
        else if(ability==old2)
            ability=new2;
        //else
        //  ability is the same as before
    }
    this->form_id = new_form_id;
    updateStats();
    if(getNickname() == old_spec->getName()){
        nickname = new_spec->getName();
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

void Monster::evolveIntoShedinja(){
    // Shedinja is a special case of evolution
    // change gender to genderless
    gender = GENDERLESS;
    completeEvolution(292,0);
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
    if(consumed_item!=NO_ITEM_TYPE)
        consumed_item = NO_ITEM_TYPE;
    if(isMegaEvolved()){
        cancelMega();
    }
    //reset zen mode for darmanitan outside of battle
    resetZenMode();
    if(species_id==351 && form_id != 0){
        //castform returns to its normal form
        form_id = 0;
        updateStats();
    }
    if(species_id==421 && form_id != 0){
        //cherrim returns to its normal form
        form_id = 0;
    }
    seen_opponents.clear();

    //change form depending on season
    changeSeasonalForm();
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
    // steal form
    transformation->form_id = other->getFormId();
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
    return Species::getSpecies(species_id)->getHeight(getFormId());
}

unsigned int Monster::getWeight()const{
    if(transformation != nullptr)
        return transformation->getWeight();
    return Species::getSpecies(species_id)->getWeight(getFormId());
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
            case BERRY_JUICE:
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
            case PECHA_BERRY:{
                if(permanent_status == POISONED || permanent_status == BAD_POISON){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its poisoning!");
                }
                break;
            }
            case AWAKENING:
            case CHESTO_BERRY:{
                if(permanent_status == SLEEP_1 || permanent_status == SLEEP_2 || 
                    permanent_status == SLEEP_3 || permanent_status == SLEEP_4){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" woke up!");
                }
                break;
            }
            case PARALYZE_HEAL:
            case CHERY_BERRY:{
                if(permanent_status == PARALYSIS){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its paralysis!");
                }
                break;
            }
            case BURN_HEAL:
            case RAWST_BERRY:{
                if(permanent_status == BURN){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its burn!");
                }
                break;
            }
            case ICE_HEAL:
            case ASPEAR_BERRY:{
                if(permanent_status == FREEZE){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its freeze!");
                }
                break;
            }
            case FULL_RESTORE:
            case FULL_HEAL:
            case LUM_BERRY:{
                if(permanent_status != NO_PERMANENT_CONDITION){
                    permanent_status = NO_PERMANENT_CONDITION;
                    result = true;
                    handler->displayMsg(getNickname()+" was cured of its status condition!");
                }
                break;
            }
            default:
                break;
        }
        // PP Restoring items
        switch(item_type){
            case LEPPA_BERRY:{
                Attack* attack_to_recover = Attack::getAttack(data);
                if(data!=0){
                    recoverPP(data,10);
                    handler->displayMsg(getNickname()+"'s " + attack_to_recover->getName() + " recovered some PPs!");
                }else{
                    // if no attack is specified, recover PP for the first attack available
                    for(int i=0;i<4;i++){
                        if(attack_ids[i].attack_id != 0){
                            unsigned int attack_id = attack_ids[i].attack_id;
                            unsigned int current_pp = attack_ids[i].current_pp;
                            unsigned int max_pp = getMaxPPForAttack(attack_id);
                            if(max_pp > current_pp){
                                recoverPP(attack_id,10);
                                handler->displayMsg(getNickname()+"'s " + attack_to_recover->getName() + " recovered some PPs!");
                                result = true;
                                break;
                            }
                        }
                    }
                }
                result=true;
                break;
            }
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
            case BERRY_JUICE:
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
std::pair<ItemType,bool> Monster::setHeldItem(ItemType item){
    ItemType old_held_item = getHeldItem();
    held_item = item;
    bool res = changeFormOnNewItem();
    return {old_held_item,res};
}
std::pair<ItemType,bool> Monster::removeHeldItem(){
    if(!hasHeldItem())
        return {NO_ITEM_TYPE,false};
    ItemType old_held_item = getHeldItem();
    held_item = NO_ITEM_TYPE;
    bool res = changeFormOnNewItem();
    return {old_held_item,res};
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

void Monster::setConsumedItem(ItemType item){
    consumed_item = item;
}
ItemType Monster::getConsumedItem()const{
    return consumed_item;
}

bool Monster::canMegaEvolve()const{
    if(isFainted())
        return false;
    if(transformation != nullptr)
        return false;
    if(is_mega)
        return false;
    Species* spec = Species::getSpecies(species_id);
    return spec->canMegaEvolve(getFormId(),getHeldItem(),hasAttack(DRAGON_ASCENT_ID));
}

bool Monster::megaEvolve(){
    if(!canMegaEvolve())
        return false;
    Species* spec = Species::getSpecies(species_id);
    // if(!spec->canMegaEvolve(getFormId(),getHeldItem()))
    //     return false;
    unsigned int new_form_id = spec->getMegaForm(getFormId(),getHeldItem(),hasAttack(DRAGON_ASCENT_ID));
    if(new_form_id == getFormId() || new_form_id == 0)
        return false;
    form_id = new_form_id;
    updateStats();
    mega_ability = spec->getAbility1(new_form_id); 
    is_mega = true;
    return true;
}
bool Monster::isMegaEvolved()const{
    return is_mega;
}
void Monster::cancelMega(){
    if(!isMegaEvolved())
        return;
    Species* spec = Species::getSpecies(species_id);
    form_id = spec->getNonMegaForm(getFormId());
    updateStats();
    is_mega = false;
    mega_ability = NO_ABILITY;
}

void Monster::addSeenOpponent(Monster* opponent){
    seen_opponents.insert(opponent);
}
bool Monster::hasSeenOpponent(Monster* opponent)const{
    if(opponent == nullptr)
        return false;
    auto it = seen_opponents.find(opponent);
    if(it != seen_opponents.end()){
        return true;
    }
    return false;
}

bool Monster::isPastEvoLevel()const{
    Species* spec = Species::getSpecies(species_id);
    unsigned int min_evo_level = MAX_LEVEL;
    for(const Evolution &evo: spec->getEvolutions(getFormId())){
        if(evo.getEvolutionMethod() == LEVEL){
            unsigned int evo_level = evo.getMethodCondition();
            if(evo_level < min_evo_level)
                min_evo_level = evo_level;
        }
    }
    return level > min_evo_level;
}

void Monster::setBall(ItemType ball){
    ItemData* item_data = ItemData::getItemData(ball);
    if(item_data == nullptr)
        return;
    if(item_data->getCategory() != BALL)
        return;
    ball_containing_monster = ball;
}

bool Monster::changeFormSwitchIn(){
    if(species_id==382 && getHeldItem() == BLUE_ORB){
        //Kyogre
        if(form_id == 0){
            form_id = 123;
            updateStats();
            Species* spec = Species::getSpecies(species_id);
            mega_ability = spec->getAbility1(form_id); 
            return true;
        }
    }
    if(species_id==383 && getHeldItem() == RED_ORB){
        //Groudon
        if(form_id == 0){
            form_id = 124;
            updateStats();
            Species* spec = Species::getSpecies(species_id);
            mega_ability = spec->getAbility1(form_id); 
            return true;
        }
    }
    return false;
}

bool Monster::changeWeatherForm(Weather weather){
    //returns true if the form was actually changed
    if(species_id==351){//castform
        switch(weather){
            case RAIN:
            case HEAVY_RAIN:{
                if(form_id == 114)
                    return false;
                form_id = 114;
                return true;
            }
            case SUN:
            case EXTREME_SUN:{
                if(form_id == 113)
                    return false;
                form_id = 114;
                return true;
            }
            case SNOWSTORM:
            case HAIL:{
                if(form_id == 115)
                    return false;
                form_id = 115;
                return true;
            }
            default:{
                if(form_id == 0)
                    return false;
                form_id = 0;
                return true;
            }
        }
    }else if(species_id==421){//cherrim
        switch(weather){
            case SUN:
            case EXTREME_SUN:{
                if(form_id == 133)
                    return false;
                form_id = 133;
                return true;
            }
            default:{
                if(form_id == 0)
                    return false;
                form_id = 0;
                return true;
            }
        }
    }
    return false;
}

bool Monster::changeFormOnNewItem(){
    switch(species_id){
        case 483:{//dialga
            if(form_id==0 && getHeldItem()==ADAMANT_CRYSTAL){
                form_id = 147;
                updateStats();
                return true;
            }else if(form_id==147 && getHeldItem()!=ADAMANT_CRYSTAL){
                form_id = 0;
                updateStats();
                return true;
            }else{
                return false;
            }
            break;
        }
        case 484:{//palkia
            if(form_id==0 && getHeldItem()==LUSTROUS_GLOBE){
                form_id = 148;
                updateStats();
                return true;
            }else if(form_id==148 && getHeldItem()!=LUSTROUS_GLOBE){
                form_id = 0;
                updateStats();
                return true;
            }else{
                return false;
            }
            break;
        }
        case 487:{//giratina
            if(form_id==0 && getHeldItem()==GRISEOUS_CORE){
                form_id = 149;
                updateStats();
                Species* spec = Species::getSpecies(species_id);
                if(has_hidden_ability)
                    ability = spec->getHiddenAbility(form_id);
                else
                    ability = spec->getAbility1(form_id);
                return true;
            }else if(form_id==149 && getHeldItem()!=GRISEOUS_CORE){
                form_id = 0;
                updateStats();
                Species* spec = Species::getSpecies(species_id);
                if(has_hidden_ability)
                    ability = spec->getHiddenAbility(form_id);
                else
                    ability = spec->getAbility1(form_id);
                return true;
            }else{
                return false;
            }
            break;
        }
        case 493:{//arceus
            switch(getHeldItem()){
                case FIST_PLATE:{
                    if(form_id != 151){
                        form_id = 151;
                        return true;
                    }else{
                        return false;
                    }
                }
                case SKY_PLATE:{
                    if(form_id != 152){
                        form_id = 152;
                        return true;
                    }else{
                        return false;
                    }
                }
                case TOXIC_PLATE:{
                    if(form_id != 153){
                        form_id = 153;
                        return true;
                    }else{
                        return false;
                    }
                }
                case EARTH_PLATE:{
                    if(form_id != 154){
                        form_id = 154;
                        return true;
                    }else{
                        return false;
                    }
                }
                case STONE_PLATE:{
                    if(form_id != 155){
                        form_id = 155;
                        return true;
                    }else{
                        return false;
                    }
                }
                case INSECT_PLATE:{
                    if(form_id != 156){
                        form_id = 156;
                        return true;
                    }else{
                        return false;
                    }
                }
                case SPOOKY_PLATE:{
                    if(form_id != 157){
                        form_id = 157;
                        return true;
                    }else{
                        return false;
                    }
                }
                case IRON_PLATE:{
                    if(form_id != 158){
                        form_id = 158;
                        return true;
                    }else{
                        return false;
                    }
                }
                case FLAME_PLATE:{
                    if(form_id != 159){
                        form_id = 159;
                        return true;
                    }else{
                        return false;
                    }
                }
                case SPLASH_PLATE:{
                    if(form_id != 160){
                        form_id = 160;
                        return true;
                    }else{
                        return false;
                    }
                }
                case MEADOW_PLATE:{
                    if(form_id != 161){
                        form_id = 161;
                        return true;
                    }else{
                        return false;
                    }
                }
                case ZAP_PLATE:{
                    if(form_id != 162){
                        form_id = 162;
                        return true;
                    }else{
                        return false;
                    }
                }
                case MIND_PLATE:{
                    if(form_id != 163){
                        form_id = 163;
                        return true;
                    }else{
                        return false;
                    }
                }
                case ICICLE_PLATE:{
                    if(form_id != 164){
                        form_id = 164;
                        return true;
                    }else{
                        return false;
                    }
                }
                case DRACO_PLATE:{
                    if(form_id != 165){
                        form_id = 165;
                        return true;
                    }else{
                        return false;
                    }
                }
                case DREAD_PLATE:{
                    if(form_id != 166){
                        form_id = 166;
                        return true;
                    }else{
                        return false;
                    }
                }
                case PIXIE_PLATE:{
                    if(form_id != 167){
                        form_id = 167;
                        return true;
                    }else{
                        return false;
                    }
                }
                default:{
                    if(form_id != 0){
                        form_id = 0;
                        return true;
                    }else{
                        return false;
                    }
                }
            }
            break;
        }
    }
    return false;
}

void Monster::interactWithKeyItem(ItemType item, EventHandler* handler){
    ItemData* item_data = ItemData::getItemData(item);
    if(item_data == nullptr)
        return;
    if(item_data->getCategory() != KEY_ITEM)
        return;
    switch(item){
        case GRACIDEA:{
            if(species_id==492){//shaymin
                if(form_id==0){
                    handler->displayMsg(getNickname()+" transformed into its Sky Forme!");
                    form_id = 150;
                }else{
                    handler->displayMsg(getNickname()+" transformed into its Land Forme!");
                    form_id = 0;
                }
                updateStats();
                Species* spec = Species::getSpecies(species_id);
                if(has_hidden_ability)
                    ability = spec->getHiddenAbility(form_id);
                else
                    ability = spec->getAbility1(form_id);
                return;
            }
            break;
        }
        case METEORITE:{
            if(species_id==386){//deoxys
                if(form_id==0){
                    handler->displayMsg(getNickname()+" transformed into its Attack Forme!");
                    form_id = 126;
                }else if(form_id==126){
                    handler->displayMsg(getNickname()+" transformed into its Defense Forme!");
                    form_id = 127;
                }else if(form_id==127){
                    handler->displayMsg(getNickname()+" transformed into its Speed Forme!");
                    form_id = 128;
                }else{
                    handler->displayMsg(getNickname()+" transformed into its Normal Forme!");
                    form_id = 0;
                }
                updateStats();
                return;
            }
            break;
        }
        case ROTOM_CATALOG:{
            if(species_id==479){//rotom
                unsigned int rotom_form_choice = handler->chooseRotomForm();
                bool res = false;
                unsigned int old_form = form_id;
                if(rotom_form_choice == 1 && form_id != 0){
                    handler->displayMsg(getNickname()+" possessed the Light Bulb!");
                    form_id = 0;
                    res = true;
                }else if(rotom_form_choice == 2 && form_id != 142){
                    handler->displayMsg(getNickname()+" possessed the Washing Machine!");
                    form_id = 142;
                    res=true;
                }else if(rotom_form_choice == 3 && form_id != 143){
                    handler->displayMsg(getNickname()+" possessed the Microwave Oven!");
                    form_id = 143;
                    res=true;
                }else if(rotom_form_choice == 4 && form_id != 144){
                    handler->displayMsg(getNickname()+" possessed the Refrigerator!");
                    form_id = 144;
                    res=true;
                }else if(rotom_form_choice == 5 && form_id != 145){
                    handler->displayMsg(getNickname()+" possessed the Electric Fan!");
                    form_id = 145;
                    res=true;
                }else if(rotom_form_choice == 6 && form_id != 146){
                    handler->displayMsg(getNickname()+" possessed the Lawn Mower!");
                    form_id = 146;
                    res=true;
                }
                if(res){
                    updateStats();
                    Species* spec = Species::getSpecies(species_id);
                    unsigned int old_evo_attack = *(spec->getEvolutionLearnset(old_form).begin());
                    unsigned int new_evo_attack = *(spec->getEvolutionLearnset(form_id).begin());
                    forgetAttack(old_evo_attack);
                    learnAttack(new_evo_attack);
                    return;
                }
            }
            break;
        }
        default:break;
    }
    handler->displayMsg("Nothing happened.");
}

bool Monster::tryZenMode(){
    if(damage<getMaxHP()/2)
        return false;
    if(species_id != 555)//only works with darmanitan
        return false;
    if(form_id == 178 || form_id == 179)//already zen mode
        return false;
    if(form_id == 0){
        form_id = 178;
        updateStats();
        return true;
    }else if(form_id==177){
        form_id = 179;
        updateStats();
        return true;
    }
    return false;
}
void Monster::resetZenMode(){
    if(species_id != 555)//only works with darmanitan
        return;
    if(form_id != 178 && form_id != 179)//not in zen mode
        return;
    if(form_id == 178){
        form_id = 0;
        updateStats();
    }else if(form_id == 179){
        form_id = 177;
        updateStats();
    }
}

void Monster::changeSeasonalForm(){
    switch(species_id){
        case 585:{//deerling
            switch(getSeason()){
                case SPRING:{
                    form_id = 0;
                    break;
                }
                case SUMMER:{
                    form_id = 183;
                    break;
                }
                case FALL:{
                    form_id = 184;
                    break;
                }
                case WINTER:{
                    form_id = 185;
                    break;
                }
                default:{
                    form_id = 0;
                    break;
                }
            }
            break;
        }
        case 586:{
            switch(getSeason()){
                case SPRING:{
                    form_id = 0;
                    break;
                }
                case SUMMER:{
                    form_id = 186;
                    break;
                }
                case FALL:{
                    form_id = 187;
                    break;
                }
                case WINTER:{
                    form_id = 188;
                    break;
                }
                default:{
                    form_id = 0;
                    break;
                }
            }
            break;
        }
        default:break;
    }
}