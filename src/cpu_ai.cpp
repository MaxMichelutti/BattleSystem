#include "cpu_ai.h"

// Choice

Choice::Choice(){
    choice_id = 0;
    utility = 0;
    action_type = ATTACK;
    item = NO_ITEM_TYPE;
}
Choice::Choice(unsigned int choice_id, int utility, BattleActionType action_type){
    this->choice_id = choice_id;
    this->utility = utility;
    this->action_type = action_type;
    this->item = NO_ITEM_TYPE;
}
Choice::Choice(ItemType item, int utility){
    this->choice_id = 0;
    this->utility = utility;
    this->action_type = USE_ITEM;
    this->item = item;
}
Choice::~Choice(){}

// CPUAI

CPUAI::CPUAI(){
    skill = 0;
}
CPUAI::CPUAI(unsigned int skill){
    this->skill = skill;
}
CPUAI::~CPUAI(){}
unsigned int CPUAI::getSkill()const{
    return skill;
}

int CPUAI::computeAttackUtility(unsigned int attack_id, Battler* cpu_active,MonsterTeam * cpu_team,Battler* enemy_active, Field* field)const{
    Attack* attack = Attack::getAttack(attack_id);
    unsigned int effect = attack->getEffectId();
    // if(cpu_active->isFainted()){
    //     return -10000;
    // }
    unsigned int accuracy = attack->getAccuracy();
    Type attack_type = attack->getType(enemy_active,field);
    if(accuracy==ALWAYS_HITS)
        accuracy = 100;
    int total_utility = 10;
    float sleep_mult = 1;
    if(enemy_active->isAsleep())
        sleep_mult=1.5;//incentivize hitting asleep opponents
    double screen_mult = 1;
    // decentivize hitting screens, unless brick break is used
    if((field->hasFieldEffect(REFLECT,OPPONENT) ||
        field->hasFieldEffect(AURORA_VEIL,OPPONENT)) && attack->getCategory() == PHYSICAL){
        screen_mult = (effect==172 || effect==215)?1.1:0.5;
    }
    if((field->hasFieldEffect(LIGHT_SCREEN,OPPONENT) ||
        field->hasFieldEffect(AURORA_VEIL,OPPONENT)) && attack->getCategory() == SPECIAL){
        screen_mult = (effect==172 || effect==215)?1.1:0.5;
    }
    unsigned int power = attack->getPower();
    switch(effect){//change power of particular attacks
        case 81:{
            // power depends on stat schanges
            unsigned int attack_mod = max(0,cpu_active->getAttackModifier());
            unsigned int special_attack_mod = max(0,cpu_active->getSpecialAttackModifier());
            unsigned int defense_mod = max(0,enemy_active->getDefenseModifier());
            unsigned int special_defense_mod = max(0,enemy_active->getSpecialDefenseModifier());
            unsigned int speed_mod = max(0,cpu_active->getSpeedModifier());
            unsigned int sum = attack_mod + special_attack_mod +
                defense_mod + special_defense_mod + speed_mod;
            power = 20 + sum * 20;
        }
        // compute power for electro ball
        case 64:{
            unsigned int user_speed = cpu_active->getModifiedSpeed();
            unsigned int enemy_speed = enemy_active->getModifiedSpeed();
            double speed_comparison = 100.0 *  enemy_speed / user_speed;
            if(speed_comparison>=100.0){
                power = 40;
            }else if(speed_comparison > 50.0){
                power = 60;
            }else if(speed_comparison > 33.34){
                power = 80;
            }else if(speed_comparison > 25.0){
                power = 120;
            }else{
                power = 160;
            }
            break;
        }
        //base power for gyro ball
        case 71:{
            unsigned int user_speed = cpu_active->getModifiedSpeed();
            unsigned int enemy_speed = enemy_active->getModifiedSpeed();
            if(user_speed < 1)
                power = 1;
            else
                power = min(150,(25*enemy_speed/user_speed)+1);
            break;
        }
        //check venoshock
        case 40:{
            if(enemy_active->isPoisoned()){ 
                power *= 2.0;
            }
            break;
        }
        // spit up
        case 57:{
            // spit up power depends on stockpiles
            unsigned int stockpiles = cpu_active->getStockpiles();
            if(stockpiles == 0){
                power = 0;
            }else if(stockpiles == 1){
                power = 100;
            }else if(stockpiles == 2){
                power = 200;
            }else{
                power = 300;
            }
            break;
        }
        case 111:{
            //power depends on target weight
            unsigned int weight = enemy_active->getWeight();
            if(weight<100)
                power = 20;
            else if(weight<250)
                power = 40;
            else if(weight<500)
                power = 60;
            else if(weight<1000)
                power = 80;
            else if(weight<2000)
                power = 100;
            else
                power = 120;
            break;
        }
        case 116:{
            //double power if last move failed
            if(cpu_active->lastAttackFailed()){
                power *= 2;
            }
            break;
        }
        case 118:{
            // power increases as many times as the use has been hit
            unsigned int hit_counter = cpu_active->numberOfHitsTaken();
            power *= (min(hit_counter,6)+1);
            break;
        }
        case 120:{
            //power depends on user current % of HP
            double current_hp_percentage = (double)cpu_active->getCurrentHP() / (double)cpu_active->getMaxHP() * 100;
            if(current_hp_percentage < 4.2)
                power = 200;
            else if(current_hp_percentage < 10.4)
                power = 150;
            else if(current_hp_percentage < 20.8)
                power = 100;
            else if(current_hp_percentage < 35.4)
                power = 80;
            else if(current_hp_percentage < 68.8)
                power = 40;
            else
                power = 20;
            break;
        }
        case 134:{
            // power doubles if target has Permanent status
            if(enemy_active->hasPermanentStatus()){
                power *= 2;
            }
            break;
        }
        case 140:{
            //power depends on % oh weight
            double weight_percentage = (double)enemy_active->getWeight() / (double)cpu_active->getWeight() * 100;
            if(weight_percentage > 50){
                power = 40;
            }else if(weight_percentage > 33.34){
                power = 60;
            }else if(weight_percentage > 25){
                power = 80;
            }else if(weight_percentage > 20){
                power = 100;
            }else{
                power = 120;
            }
            break;
        }
        case 155:{
            // power doubles if HP < 50%
            if(cpu_active->getCurrentHP() <= (cpu_active->getMaxHP()+1) / 2){
                power *= 2;
            }
            break;
        }
        case 165:{
            // power depends on current user %v of HP
            double current_hp_percentage = (double)cpu_active->getCurrentHP() / (double)cpu_active->getMaxHP() * 100;
            if(current_hp_percentage < 4.2)
                power = 200;
            else if(current_hp_percentage < 9.4)
                power = 150;
            else if(current_hp_percentage < 20.3)
                power = 100;
            else if(current_hp_percentage < 34.4)
                power = 80;
            else if(current_hp_percentage < 67.2)
                power = 40;
            else
                power = 20;
            break;
        }
        case 259:{
            // power depends on target hp percent
            power = 100 * enemy_active->getCurrentHP() / (double)enemy_active->getMaxHP();
            break;
        }
        case 289:{
            // power depends on target hp percent
            power = 120 * enemy_active->getCurrentHP() / (double)enemy_active->getMaxHP();
            break;
        }
    }
    if(power != 0){
        bool can_hit_ghosts = cpu_active->hasAbility(SCRAPPY);
        float effectiveness = getTypeEffectiveness(attack_type, 
            enemy_active->getTypes(),enemy_active->isTouchingGround(),
            can_hit_ghosts,effect == 196);
        unsigned int dmg = baseDamage(cpu_active->getLevel(),power,
            cpu_active->getModifiedAttack(),enemy_active->getModifiedDefense());
        if(effect == 36)// triple expected damage for multi hit moves
            dmg*=3;
        if(effect == 75)// double expected damage for twice hit moves
            dmg*=2;
        if(effect == 37 && (cpu_active->getLastAttackUsed() == attack_id))//fury cutter
            dmg *= 2; // expect double damage from multiple uses of fury cutter
        double final_expected_damage = effectiveness * dmg * screen_mult;
        total_utility += final_expected_damage / enemy_active->getMaxHP() * 100;// % of HP that opponent will lose
        if(final_expected_damage > enemy_active->getCurrentHP()){
            // I see a kill!!!
            total_utility *= 5;
        }
        total_utility*=sleep_mult;
    }
    unsigned int effect_probability = attack->getEffectChance();
    if(effect_probability == ALWAYS_HITS || effect_probability == 0){
        effect_probability = 100;
    }
    double effect_prob_mult = effect_probability / 100.0;
    int actual_stat_zero = 0 - MIN_MODIFIER;
    if(attack->getCategory() == STATUS)
        total_utility = 0;
    if(attack->isSoundBased() && cpu_active->hasVolatileCondition(THROAT_CHOPPED))
        return -50;
    switch(effect){
        case 1:{
            // -1 att opponent
            unsigned int attack_mod = enemy_active->getAttackModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            break;
        }
        case 2:{
            // +1 att spatt user
            unsigned int att_mod = cpu_active->getAttackModifier();
            unsigned int spatt_mod = cpu_active->getSpecialAttackModifier();
            total_utility += 5 * (actual_stat_zero - att_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - spatt_mod) * effect_prob_mult;
            break;
        }
        case 3:{
            //seed opponent
            if(enemy_active->hasVolatileCondition(LEECH_SEED) || enemy_active->hasType(GRASS)){
                total_utility -= 10;
            }else{
                total_utility += 50 * effect_prob_mult;
            }
            break;
        }
        case 4:case 232:case 241:{
            // poison opp
            if(enemy_active->canBePoisoned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 50 * effect_prob_mult;
            }
            if(effect==241 && enemy_active->isPoisoned()){
                //doubles in power if opponent is already poinìsoned
                total_utility *= 2;
            }
            break;
        }
        case 5:case 143:{
            // sleep  opp
            if(enemy_active->canFallAsleep() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD &&
                field->getTerrain()!=ELECTRIC_FIELD){
                total_utility += 70 * effect_prob_mult;
            }
            if(effect==143)
                total_utility *= 0.8;
            break;
        }
        case 6:case 229:{
            //recoil 1/4 dmg
            total_utility *= 0.9;
            if(effect==229){
                // recoil 1/2 dmg
                total_utility *= 0.8;
            }
            break;
        }
        case 7:{
            //-2 evesiveness opponent
            unsigned int evasion_mod = enemy_active->getEvasionModifier();
            total_utility += 6 * effect_prob_mult * (actual_stat_zero + evasion_mod);
            break;
        }
        case 8:{
            // heal 50% user clear, 66% sun, 25% other
            unsigned int HP_percent = enemy_active->getCurrentHP() * 100 / enemy_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 10;
            else if(HP_percent >= 25)
                total_utility += 25;
            else if(HP_percent >= 10)
                total_utility += 50;
            else
                total_utility += 100;
            if(field->getWeather() == SUN || field->getWeather()==EXTREME_SUN)
                total_utility *= 1.2;
            else if(field->getWeather()!=CLEAR)
                total_utility *= 0.5;
            break;
        }
        case 13:case 281:{
            // ability of opp becomes insomnia/simple
            Ability target_ability;
            if(effect==13)
                target_ability = INSOMNIA;
            else if(effect==281)
                target_ability = SIMPLE;
            if(enemy_active->hasAbility(target_ability) || abilityCannotBeChanged(enemy_active->getAbility()))
                total_utility -= 10;
            else
                total_utility += 10;
            break;
        }
        case 14:{
            // burn opp
            if(enemy_active->canBeBurned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 60 * effect_prob_mult;
            }
            break;
        }
        case 15:{
            //-1 accuracy opp
            unsigned int accuracy_mod = enemy_active->getAccuracyModifier();
            total_utility += 3 * effect_prob_mult * (actual_stat_zero + accuracy_mod);
            break;
        }
        case 16:case 67:{
            // paralysis
            if(enemy_active->canBeParalyzed() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 50 * effect_prob_mult;
            }
            if(effect==67 && (field->getWeather() == RAIN || field->getWeather()==HEAVY_RAIN)){
                total_utility *= 1.5;
            }
            break;
        }
        case 17:{
            // crits are more likely
            total_utility *= 1.15;
            break;
        }
        case 18:{
            // -2 Speed opp
            unsigned int speed_mod = enemy_active->getSpeedModifier();
            total_utility += 10 * effect_prob_mult * (actual_stat_zero + speed_mod);
            break;
        }
        case 19:{
            // fire spin
            if(enemy_active->hasVolatileCondition(FIRESPIN)){
                total_utility -= 10;
            }else{
                total_utility += 25;
            }
            break;
        }
        case 287:{
            // magma storm
            if(enemy_active->hasVolatileCondition(MAGMA_STORM)){
                total_utility -= 10;
            }else{
                total_utility += 25;
            }
            break;
        }
        case 220:{
            // infestation
            if(enemy_active->hasVolatileCondition(INFESTED)){
                total_utility -= 10;
            }else{
                total_utility += 25;
            }
            break;
        }
        case 20:{
            // recoil and burn opp
            if(enemy_active->canBeBurned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 60 * effect_prob_mult;
            }
            total_utility *= 0.9;
            break;
        }
        case 296:{
            // recoil and paralyze opp
            if(enemy_active->canBeParalyzed() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 50 * effect_prob_mult;
            }
            total_utility *= 0.9;
            break;
        }
        case 21:case 141:{
            // flinch
            total_utility += 80 * effect_prob_mult;
            break;
        }
        case 22:{
            // -1 def opp
            unsigned int defense_mod = enemy_active->getDefenseModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + defense_mod);
            break;
        }
        case 23:{
            // +1 def user
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            break;
        }
        case 29:{
            // +2 def user
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            total_utility += 10 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            break;
        }
        case 24:{
            // clear field and +1 speed user
            if(cpu_active->hasVolatilesSuchThat(&isVolatileConditionClearedByRapidSpin))
                total_utility += 50;
            else
                total_utility -= 10;
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 25:case 174:case 299:{
            // confusion
            if(!enemy_active->hasVolatileCondition(CONFUSION) &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 35 * effect_prob_mult;
            }
            if(effect==174){
                // recoil risk
                total_utility *= 0.7;
            }
            break;
        }
        case 26:
        case 268:{
            // protect
            if(cpu_active->getConsecutiveProtects() > 0)
                total_utility -= 10;
            else
                total_utility += 80;
            break;
        }
        case 27:{
            // set rain weather
            if(field->getWeather() == RAIN || field->weatherCannotChange())
                total_utility -= 10;
            else
                total_utility += 90;
            break;
        }
        case 28:{
            // +2 att apatt spd -1 def spdef user
            int attack_mod = cpu_active->getAttackModifier();
            int special_attack_mod = cpu_active->getSpecialAttackModifier();
            // int defense_mod = cpu_active->getDefenseModifier();
            // int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 10 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 10 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            total_utility += 10 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            total_utility -= 5 * effect_prob_mult;
            total_utility -= 5 * effect_prob_mult;
            break;
        }
        case 30:{
            // +1 spd user, recoil
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            total_utility *= 0.9;
            break;
        }
        case 31:{
            // -1 spdef opp
            unsigned int special_defense_mod = enemy_active->getSpecialDefenseModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_defense_mod);
            break;
        }
        case 32:case 123:case 124:case 185:
        case 231:case 235:case 284:case 300:{
            // involves switching
            auto enemy_types = enemy_active->getTypes();
            float effectiveness = 1;
            for(auto type:enemy_types){
                effectiveness *= getTypeEffectiveness(type, 
                    cpu_active->getTypes(),cpu_active->isTouchingGround(),
                    false,false);
            }
            total_utility += 20 * effectiveness + effect_prob_mult;
            if(effect==235 && field->getWeather()!=SNOWSTORM){
                // set snowstorm
                total_utility += 30;
            }
            break;
        }
        case 33:{
            // safeguard
            if(field->hasFieldEffect(SAFEGUARD,OPPONENT))
                total_utility -= 10;
            else
                total_utility += 80;
            break;
        }
        case 34:{
            // tailowind
            if(field->hasFieldEffect(TAILWIND,OPPONENT))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 35:{
            // +1 spatt spdef spd user
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_defense_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 36:{
            // multi hit move from 2 to 5 times
            if(cpu_active->hasAbility(SKILL_LINK))
                total_utility *= 5;
            else
                total_utility *= 3;
            break;
        }
        case 38:{
            // setup crit for next turn
            if(cpu_active->hasVolatileCondition(LASER_FOCUS))
                total_utility -= 20;
            else
                total_utility += 30;
            break;
        }
        case 39:{
            // crits 50% of times
            total_utility *= 1.25;
            break;
        }
        case 41:{
            // toxic spikes
            if(field->hasFieldEffect(BAD_TOXIC_SPIKES,PLAYER))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 42:case 240:{
            // +2 speed user
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 10 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 43:{
            //endeavor
            unsigned int enemy_current_hp = enemy_active->getCurrentHP();
            unsigned int user_current_hp = cpu_active->getCurrentHP();
            if(enemy_current_hp > user_current_hp)
                total_utility += (enemy_current_hp - user_current_hp)*100/ enemy_active->getMaxHP();
            else
                total_utility -=  20;
            break;
        }
        case 44:{
            // if opponent faints user gains attack
            unsigned int enemy_active_hp_percent = 
                enemy_active->getCurrentHP() * 100 / enemy_active->getMaxHP();
            if(enemy_active_hp_percent <= 10)
                total_utility += 30;
            break;
        }
        case 46:{
            //-2 att opp
            unsigned int attack_mod = enemy_active->getAttackModifier();
            total_utility += 10 * effect_prob_mult * (actual_stat_zero + attack_mod);
            break;
        }
        case 47:case 126:{
            // heal 50% and land
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 10;
            else if(HP_percent >= 25)
                total_utility += 25;
            else if(HP_percent >= 10)
                total_utility += 50;
            else
                total_utility += 100;
            break;
        }
        case 50:{
            //halves opponents HP
            unsigned int enemy_current_hp_percent = 
                enemy_active->getCurrentHP() * 100 / enemy_active->getMaxHP();
            total_utility += (enemy_current_hp_percent / 2);
            break;
        }
        case 51:{
            //recoil
            total_utility *= 0.9;
            break;
        }
        case 52:{
            //+2 ATT user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            total_utility += 10 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            break;
        }
        case 53:{
            //wrap 
            if(!enemy_active->hasVolatileCondition(WRAP))
                total_utility += 25;
            break;
        }
        case 164:{
            //bind
            if(!enemy_active->hasVolatileCondition(BIND))
                total_utility += 25;
            break;
        }
        case 54:{
            // -2 spdef opp
            unsigned int special_defense_mod = enemy_active->getSpecialDefenseModifier();
            total_utility += 10 * effect_prob_mult * (actual_stat_zero + special_defense_mod);
            break;
        }
        case 55:{
            // stockpile
            if(cpu_active->getStockpiles() >= 3)
                total_utility -= 10;
            else
                total_utility += 33;
            break;
        }
        case 56:{
            // swallow
            unsigned int hp_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(cpu_active->getStockpiles() == 0)
                total_utility -= 10;
            else if(hp_percent >= 50)
                total_utility -= 10;
            else if(hp_percent >= 25)
                total_utility += 25;
            else if(hp_percent >= 10)
                total_utility += 50;
            else
                total_utility += 100;
            break;
        }
        case 57:{
            // spit up
            if(cpu_active->getStockpiles() == 0)
                total_utility = -10;
            break;
        }
        case 58:{
            // suppress ability
            if(enemy_active->isAbilitySuppressed() || abilityCannotBeSuppressed(enemy_active->getAbility()))
                total_utility -= 10;
            else
                total_utility += 50 * effect_prob_mult;
            break;
        }
        case 59:{
            // cancel all stat changes
            unsigned int enemy_boosts = 0;
            if(enemy_active->getAttackModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpecialAttackModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getDefenseModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpecialDefenseModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpeedModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getEvasionModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getAccuracyModifier() > 0)
                enemy_boosts++;
            unsigned int user_boosts = 0;
            if(cpu_active->getAttackModifier() > 0)
                user_boosts++;
            if(cpu_active->getSpecialAttackModifier() > 0)
                user_boosts++;
            if(cpu_active->getDefenseModifier() > 0)
                user_boosts++;
            if(cpu_active->getSpecialDefenseModifier() > 0)
                user_boosts++;
            if(cpu_active->getSpeedModifier() > 0)
                user_boosts++;
            if(cpu_active->getEvasionModifier() > 0)
                user_boosts++;
            if(cpu_active->getAccuracyModifier() > 0)
                user_boosts++;
            if(enemy_boosts > user_boosts)
                total_utility += 50 * effect_prob_mult;
            else
                total_utility -= 10;
            break;
        }
        case 60:{
            // +1 att def acc user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            unsigned int accuracy_mod = cpu_active->getAccuracyModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            total_utility += 3 * (actual_stat_zero - accuracy_mod) * effect_prob_mult;
            break;
        } 
        case 61:{
            //freeze
            if(enemy_active->canBeFrozen() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 40 * effect_prob_mult;
            }
            break;
        }
        case 62:{
            //+2 spatt user
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            total_utility += 10 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            break;
        }
        case 63:{
            // +1 evasivenss user
            unsigned int evasion_mod = cpu_active->getEvasionModifier();
            total_utility += 3 * (actual_stat_zero - evasion_mod) * effect_prob_mult;
            break;
        }
        case 66:{
            //light screen
            if(field->hasFieldEffect(LIGHT_SCREEN,OPPONENT))
                total_utility -= 10;
            else
                total_utility += 80;
            break;
        }
        case 125:{
            //reflect
            if(field->hasFieldEffect(REFLECT,OPPONENT))
                total_utility -= 10;
            else
                total_utility += 80;
            break;
        }
        case 69:case 122:{
            // -1 speed opp
            unsigned int speed_mod = enemy_active->getSpeedModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + speed_mod);
            if(effect == 69 && field->getTerrain()==GRASSY_FIELD)
                total_utility *= 0.8;
            break;
        }
        case 72:{
            //sandstorm
            if(field->getWeather() == SANDSTORM || field->weatherCannotChange())
                total_utility -= 10;
            else
                total_utility += 90;
            break;
        }
        case 73:{
            //earthquake
            if(enemy_active->hasVolatileCondition(UNDERGROUND))
                total_utility *= 1.25;
            if(field->getTerrain()==GRASSY_FIELD)
                total_utility *= 0.8;
            break;
        }
        case 74:{
            //sand tomb
            if(enemy_active->hasVolatileCondition(SANDTOMB))
                total_utility -= 10;
            else
                total_utility += 25;
            break;
        }
        case 76:case 298:{
            //bad poison
            if(effect==298 && cpu_active->hasType(POISON))
                effect_prob_mult = 1.1;
            // poison opp
            if(enemy_active->canBeBadlyPoisoned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 70 * effect_prob_mult;
            }
            break;
        }
        case 77:{
            //+1 spatt opp + confusion
            unsigned int special_attack_mod = enemy_active->getSpecialAttackModifier();
            total_utility -= 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            if(!enemy_active->hasVolatileCondition(CONFUSION) &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 35 * effect_prob_mult;
            }
            break;
        }
        case 78:{
            // -1 att def user
            total_utility -= 10 * effect_prob_mult;
            break;
        }
        case 79:{
            //does nothing
            total_utility -= 100;
            break;
        }
        case 80:{
            //copycat
            total_utility += 30;
            break;
        }
        case 82:{
            //encore
            if(enemy_active->hasVolatileCondition(ENCORE))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 83:case 291:{
            // heal 25% user
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 20;
            else if(HP_percent >= 25)
                total_utility += 12;
            else if(HP_percent >= 10)
                total_utility += 25;
            else
                total_utility += 50;
            break;
        }
        case 84:{
            // metronome
            total_utility = RNG::getRandomInteger(1,110);
            break;
        }
        case 85:{
            // gravity
            if(field->hasFullFieldEffect(GRAVITY))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 86:{
            // +1 att user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            break;
        }
        case 87:{
            // +1 def spdef user
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_defense_mod) * effect_prob_mult;
            break;
        }
        case 88:{
            //-1 spatt opp
            unsigned int special_attack_mod = enemy_active->getSpecialAttackModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_attack_mod);
            break;
        }
        case 89:{
            //user dies and heal entering monster
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 20;
            else if(HP_percent >= 25)
                total_utility += 1;
            else if(HP_percent >= 10)
                total_utility += 10;
            else
                total_utility += 30;
            break;
        }
        case 91:case 236:{
            //remove 4 PP from enemy last used attack
            if(enemy_active->getLastAttackUsed()==0)
                total_utility -= 10;
            else
                total_utility += 25;
            break; 
        }
        case 90:{
            //disable
            if(enemy_active->hasDiabledAttack())
                total_utility -= 10;
            else
                total_utility += 45;
            break;
        }
        case 92:{
            //imprison
            if(enemy_active->hasVolatileCondition(IMPRISON))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 93:{
            // mimic
            if(cpu_active->hasMimickedAttack())
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 95:{
            // rest
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 10;
            else if(HP_percent >= 25)
                total_utility += 37;
            else if(HP_percent >= 10)
                total_utility += 45;
            else
                total_utility += 80;
            break;
        }
        case 96:{
            // trap opponent (mean look)
            if(enemy_active->hasVolatileCondition(MEAN_LOOK))
                total_utility -= 10;
            else
                total_utility += 60;
            break;
        }
        case 97:{
            // heal 50% of damage dealt
            total_utility *= 1.3;
            break;
        }
        case 98:{
            // quick guard
            total_utility += 20;
            break;
        }
        case 99:{
            //set grassy field
            if(field->getTerrain() == GRASSY_FIELD)
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 100:
        case 275:{
            // user faints after use
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility *= 0.1;
            else if(HP_percent >= 25)
                total_utility *= 0.3;
            else if(HP_percent >= 10)
                total_utility *= 1.1;
            else
                total_utility *= 3.0;
            if(effect==275 && field->getTerrain()==MISTY_FIELD)
                total_utility *= 1.5;
            break;
        }
        case 101:{
            //heal perm status user
            if(cpu_active->hasPermanentStatus() && !cpu_active->isAsleep())
                total_utility += 40;
            else
                total_utility -= 20;
            break;
        }
        case 102:{
            //transform
            total_utility += 90;
            break;
        }
        case 103:case 156:{
            //ohko
            total_utility = 100;
            break;
        }
        case 104:{
            // spec condition
            if(!enemy_active->hasPermanentStatus())
                total_utility += 10;
            break;
        }
        case 105:{
            // fails if not used at first turn + flinch
            if(cpu_active->isFirstTurn())
                total_utility += 80;
            else
                total_utility -= 30;
            break;
        }
        case 106:{
            // taunt
            if(!enemy_active->hasVolatileCondition(TAUNTED))
                total_utility += 70;
            else
                total_utility -= 10;
            break;
        }
        case 107:{
            // turn enemy into water type
            if(enemy_active->hasType(WATER))
                total_utility -= 20;
            else
                total_utility += 30;
            break;
        }
        case 108:{
            // copy target stat changes
            unsigned int target_boosts = 0;
            target_boosts += enemy_active->getAttackModifier();
            target_boosts += enemy_active->getDefenseModifier();
            target_boosts += enemy_active->getSpecialAttackModifier();
            target_boosts += enemy_active->getSpecialDefenseModifier();
            target_boosts += enemy_active->getSpeedModifier();
            unsigned int user_boosts = 0;
            user_boosts += cpu_active->getAttackModifier();
            user_boosts += cpu_active->getDefenseModifier();
            user_boosts += cpu_active->getSpecialAttackModifier();
            user_boosts += cpu_active->getSpecialDefenseModifier();
            user_boosts += cpu_active->getSpeedModifier();
            total_utility += (target_boosts - user_boosts) * 5;
            break;
        }
        case 109:{
            // +2 spdef user
            unsigned int spdef_mod = cpu_active->getSpecialDefenseModifier();
            total_utility += 10 * (actual_stat_zero - spdef_mod) * effect_prob_mult;
            break;
        }
        case 110:{
            // wonder room
            if(field->hasFullFieldEffect(WONDER_ROOM))
                total_utility -= 20;
            else
                total_utility += 50;
            break;
        }
        case 112:{
            //damage equals user level
            total_utility += cpu_active->getLevel() * 100 / enemy_active->getMaxHP();
            break;
        }
        case 113:{
            // confusion and +2 att opp
            if(!enemy_active->hasVolatileCondition(CONFUSION) &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 35 * effect_prob_mult;
            }
            unsigned int attack_mod = enemy_active->getAttackModifier();
            total_utility -= 10 * effect_prob_mult * (actual_stat_zero + attack_mod);
            break;
        }
        case 115:{
            //-1 def spdef opp
            unsigned int defense_mod = enemy_active->getDefenseModifier();
            unsigned int special_defense_mod = enemy_active->getSpecialDefenseModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + defense_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_defense_mod);
            break;
        }
        case 117:{
            // just a bad dumb attack
            total_utility += 10;
            break;   
        }
        case 121:{
            // -50% HP user, then attack maxed
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent < 50 || cpu_active->getAttackModifier() == MAX_MODIFIER)
                total_utility -= 20;
            else if(HP_percent < 75)
                total_utility += 10;
            else
                total_utility += 50;
            break;
        }
        case 127:{
            //copy target ability
            if(cpu_active->hasAbility(enemy_active->getAbility()) || 
                enemy_active->hasAbility(NO_ABILITY) ||
                abilityCannotBeCopied(enemy_active->getAbility()))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 129:{
            //+1 def spdef user
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_defense_mod) * effect_prob_mult;
            break;
        }
        case 132:{
            //+1 ATT DEF USER
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            break;
        }
        case 133:{
            // -2 spatt user
            total_utility -= 10;
            break;
        }
        case 135:{
            //hits underwater
            if(enemy_active->hasVolatileCondition(UNDERWATER))
                total_utility *= 1.5;
            break;
        }
        case 137:{
            //user becomes opp type
            auto enemy_types = enemy_active->getTypes();
            auto user_types = cpu_active->getTypes();
            float effectiveness = 1;
            for(auto type:enemy_types){
                effectiveness *= getTypeEffectiveness(type, 
                    user_types,cpu_active->isTouchingGround(),
                    false,false);
            }
            total_utility += 20 * effectiveness + effect_prob_mult;
        }
        case 138:{
            // smack target on ground
            if(!enemy_active->isTouchingGround())
                total_utility += 10;
            break;
        }
        case 139:{
            //stealth rock
            if(field->hasFieldEffect(STEALTH_ROCKS,PLAYER))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 142:{
            //+1 speed user
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 144:{
            //curse
            if(cpu_active->hasType(GHOST)){
                unsigned int hp_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
                if(hp_percent < 50)
                    total_utility -= 10;
                else if(!enemy_active->hasVolatileCondition(CURSED))
                    total_utility -= 30;
                else
                    total_utility += 60;
            }else{
                // +1 att def -1 spd user
                int attack_mod = cpu_active->getAttackModifier();
                int defense_mod = cpu_active->getDefenseModifier();
                // int speed_mod = cpu_active->getSpeedModifier();
                total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
                total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
                total_utility -= 5 * effect_prob_mult;
            }
            break;
        }
        case 145:{
            // heal 50% target
            total_utility -= 10;
            break;
        }
        case 146:{
            //infatuate opponent
            Gender user_gender = cpu_active->getGender();
            Gender enemy_gender = enemy_active->getGender();
            if(!enemy_active->hasVolatileCondition(INFATUATION) && 
                areMaleAndFemale(user_gender,enemy_gender))
                total_utility += 50;
            else
                total_utility -= 10;
            break;
        }
        case 147:{
            //lock on target
            if(cpu_active->hasLockOn(enemy_active->getMonster()))
                total_utility -= 10;
            else
                total_utility += 30;
            break;
        }
        case 148:{
            // set up electric field
            if(field->getTerrain() == ELECTRIC_FIELD)
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 149:{
            //magnet rise
            if(enemy_active->isTouchingGround() && 
                !enemy_active->hasVolatileCondition(MAGNET_RISE))
                total_utility += 30;
            else
                total_utility -= 10;
            break;
        }
        case 150:{
            //cant KO
            total_utility -= 10;
            break;
        }
        case 152:{
            //raise random stat by 2
            total_utility += 20;
            break;
        }
        case 153:{
            //start snowstorm weather
            if(field->getWeather() == SNOWSTORM  || field->weatherCannotChange())
                total_utility -= 10;
            else
                total_utility += 80;
            break;
        }
        case 154:{
            //aqua ring
            if(cpu_active->hasVolatileCondition(AQUA_RING))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 157:{
            //memento
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 10;
            else if(HP_percent >= 25)
                total_utility += 1;
            else if(HP_percent >= 10)
                total_utility += 10;
            else
                total_utility += 30;
            unsigned int attack_mod = enemy_active->getAttackModifier();
            unsigned int special_attack_mod = enemy_active->getSpecialAttackModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_attack_mod);
            break;
        }
        case 158:{
            //whirlpool
            if(enemy_active->hasVolatileCondition(WHIRLPOOL))
                total_utility -= 10;
            else
                total_utility += 25;
            break;
        }
        case 159:{
            //spikes
            if(field->hasFieldEffect(SPIKES_3,PLAYER))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 221:{
            //sticky web
            if(field->hasFieldEffect(STICKY_WEB,PLAYER))
                total_utility -= 10;
            else
                total_utility += 60;
            break;
        }
        case 161:{
            //destiny bond
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 30;
            else if(HP_percent >= 25)
                total_utility += 10;
            else if(HP_percent >= 10)
                total_utility += 50;
            else
                total_utility += 80;
            break;
        }
        case 162:{
            //fails if opponent is not asleep
            if(!enemy_active->isAsleep())
                total_utility = -40;
            else
                total_utility *= 1.23;
            break;
        }
        case 163:{
            //perish song
            if(enemy_active->hasVolatileCondition(PERISH_SONG))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 166:{
            //-1 speed user
            // int speed_mod = cpu_active->getSpeedModifier();
            total_utility -= 5 * effect_prob_mult;
            break;
        }
        case 167:{
            //charge
            if(cpu_active->hasVolatileCondition(CHARGED) || 
                cpu_active->hasVolatileCondition(CHARGED_2))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 168:{
            //reflect special damage taken
            total_utility = 30;
            break;
        }
        case 169:{
            //+1 spatt user
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            total_utility += 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            break;
        }
        case 170:{
            // bad move dont use
            total_utility -= 30;
            break;
        }
        case 171:
        case 277:{
            // risk taking big recoil
            total_utility *= 0.7;
            break;
        }
        case 173:{
            //endure
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 10;
            else if(HP_percent >= 25)
                total_utility += 1;
            else if(HP_percent >= 10)
                total_utility += 10;
            else
                total_utility += 30;
            break;
        }
        case 175:case 206:{
            total_utility = 30;
            break;
        }
        case 177: case 293:{
            //177: reset stat changes
            //293: switch changes with opponent
            unsigned int enemy_boosts = 0;
            if(enemy_active->getAttackModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpecialAttackModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getDefenseModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpecialDefenseModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getSpeedModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getEvasionModifier() > 0)
                enemy_boosts++;
            if(enemy_active->getAccuracyModifier() > 0)
                enemy_boosts++;
            unsigned int user_boosts = 0;
            if(cpu_active->getAttackModifier() > 0)
                user_boosts++;  
            if(cpu_active->getSpecialAttackModifier() > 0)
                user_boosts++;
            if(cpu_active->getDefenseModifier() > 0)
                user_boosts++;
            if(cpu_active->getSpecialDefenseModifier() > 0)
                user_boosts++;
            if(cpu_active->getSpeedModifier() > 0)
                user_boosts++;
            if(cpu_active->getEvasionModifier() > 0)
                user_boosts++;
            if(cpu_active->getAccuracyModifier() > 0)
                user_boosts++;
            if(enemy_boosts > user_boosts)
                total_utility += 50 * effect_prob_mult;
            else
                total_utility -= 10;
            break;
        }
        case 178:{
            //avoid this move
            total_utility *= 0.2;    
            break;
        }
        case 179:{
            //ingrain
            if(cpu_active->hasVolatileCondition(INGRAINED))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 180:{
            //omniboost
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_defense_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 181:{
            //-1 att def opp
            unsigned int attack_mod = enemy_active->getAttackModifier();
            unsigned int defense_mod = enemy_active->getDefenseModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + defense_mod);
            break;
        }
        case 182:{
            //+1 att speed user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }
        case 183:{
            //swap OFFENSIVE stat changes with target
            unsigned int enemy_boosts = 0;
            enemy_boosts += enemy_active->getAttackModifier();
            enemy_boosts += enemy_active->getSpecialAttackModifier();
            unsigned int user_boosts = 0;
            user_boosts += cpu_active->getAttackModifier();
            user_boosts += cpu_active->getSpecialAttackModifier();
            total_utility += (enemy_boosts - user_boosts) * 5;
            break;
        }
        case 184:{
           //swap DEFENSIVE stat changes with target
           unsigned int enemy_boosts = 0;
           enemy_boosts += enemy_active->getDefenseModifier();
           enemy_boosts += enemy_active->getSpecialDefenseModifier();
           unsigned int user_boosts = 0;
           user_boosts += cpu_active->getDefenseModifier();
           user_boosts += cpu_active->getSpecialDefenseModifier();
           total_utility += (enemy_boosts - user_boosts) * 5;
           break; 
        }
        case 186:{
            //always hits under snow or hail
            if(field->getWeather() == SNOWSTORM || field->getWeather() == HAIL)
                total_utility *= 1.5;
            break;
        }
        case 187:{
            //recharge next turn
            total_utility *= 0.66;
            break;
        }
        case 188:{
            //set sunny weather
            if(field->getWeather() == SUN || field->weatherCannotChange())
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 189:{
            //always crits
            total_utility *= 2;
            break;
        }
        case 190:{
            // mist
            if(field->hasFieldEffect(MIST,OPPONENT))
                total_utility -= 10;
            else
                total_utility += 90;
            break;
        }
        case 191:{
            //user changes type to one of its attacks types
            total_utility += 10;
            break;
        }
        case 192:{
            //user changes type to resist opponent
            auto enemy_types = enemy_active->getTypes();
            auto user_types = cpu_active->getTypes();
            float effectiveness = 1;
            for(auto type:enemy_types){
                effectiveness *= getTypeEffectiveness(type, 
                    user_types,cpu_active->isTouchingGround(),
                    false,false);
            }
            total_utility += 20 * effectiveness + effect_prob_mult;
            break;
        }
        case 193:{
            //block opponent
            if(enemy_active->hasVolatileCondition(BLOCKED))
                total_utility -= 10;
            else
                total_utility += 30;
            break;
        }
        case 194:{
            // only used while sleeping selects another random attack
            if(cpu_active->isAsleep())
                total_utility+= RNG::getRandomInteger(1,110);
            else
                total_utility -= 10;
            break;
        }
        case 195:{
            //fails if user is not sleeping + flinch
            if(!cpu_active->isAsleep())
                total_utility = -40;
            else
                total_utility *= 1.23 * effect_prob_mult;
            break;
        }
        case 196:{
            //supereffective against water
            if(enemy_active->hasType(WATER))
                total_utility *= 3.33;
            break;
        }
        case 198:case 202:{
            // try to steal held item
            if(!cpu_active->hasHeldItem() && enemy_active->hasHeldItem())
                total_utility += 40;
            else
                total_utility -= 40;
            break;
        }
        case 200:{
            //fails if user hasnt consumed berries
            if(!cpu_active->hasConsumedBerry())
                total_utility -= 100; //attack is going to fail
            break;
        }
        case 203:{
            //if target has item power is increased
            if(enemy_active->hasHeldItem())
                total_utility *= 1.5;
            break;
        }
        case 204:{
            //if user has no item attack fails
            if(!cpu_active->hasHeldItem())
                total_utility -= 100; //attack is going to fail
            else
                total_utility += 50;
            break;
        }
        case 207:{
            //+1 att acc user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int accuracy_mod = cpu_active->getAccuracyModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 3 * (actual_stat_zero - accuracy_mod) * effect_prob_mult;
            break;
        }
        case 208:{
            //aurora veil
            if(field->hasFieldEffect(AURORA_VEIL,OPPONENT) || 
                (field->getWeather()!=SNOWSTORM && field->getWeather()!=HAIL))
                total_utility -= 30;
            else
                total_utility += 90;
            break;
        }
        case 211:{
            //defog
            // -1 evasion
            unsigned int evasion_mod = enemy_active->getEvasionModifier();
            total_utility += 3 * effect_prob_mult * (actual_stat_zero + evasion_mod);
            // clear terrain
            if(field->getTerrain() != NO_TERRAIN)
                total_utility += 30;
            // clear field effects
            //positive impacts
            if(field->hasFieldEffect(LIGHT_SCREEN,OPPONENT))
                total_utility += 20;
            if(field->hasFieldEffect(REFLECT,OPPONENT))
                total_utility += 20;
            if(field->hasFieldEffect(SAFEGUARD,OPPONENT))
                total_utility += 20;
            if(field->hasFieldEffect(MIST,OPPONENT))
                total_utility += 20;
            if(field->hasFieldEffect(AURORA_VEIL,OPPONENT))
                total_utility += 20;
            //negative impacts
            if(field->hasFieldEffect(STEALTH_ROCKS,OPPONENT))
                total_utility -= 20;
            if(field->hasFieldEffect(SPIKES_3,OPPONENT))
                total_utility -= 20;
            if(field->hasFieldEffect(SPIKES_2,OPPONENT))
                total_utility -= 10;
            if(field->hasFieldEffect(SPIKES,OPPONENT))
                total_utility -= 5;
            if(field->hasFieldEffect(BAD_TOXIC_SPIKES,OPPONENT))
                total_utility -= 20;
            if(field->hasFieldEffect(TOXIC_SPIKES,OPPONENT))
                total_utility -= 10;
            break;
        }
        case 212:{
            //-1 opponent speed
            unsigned int speed_mod = enemy_active->getSpeedModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + speed_mod);
            // recoil 50% user health
            total_utility *= 0.6;
            break;
        }
        case 213:{
            //misty terrain
            if(field->getTerrain() == MISTY_FIELD)
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 214:{
            //restore item
            if(cpu_active->hasConsumedItem())
                total_utility += 30;
            else
                total_utility -= 10;
            break;
        }
        case 216:{
            //trick room
            if(field->hasFullFieldEffect(TRICK_ROOM))
                total_utility -= 10;
            else
                total_utility += 60;
            break;
        }
        case 218:{
            //lower power at low HP
            total_utility *= cpu_active->getCurrentHP() / (double)cpu_active->getMaxHP(); 
            break;
        }
        case 219:{
            // burn + power doubled if opponent has status
            // double power
            if(enemy_active->hasPermanentStatus())
                total_utility *= 2;
            // burn
            if(enemy_active->canBeBurned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 60 * effect_prob_mult;
            }
            break;
        }
        case 222:{
            //poison opponent
            if(enemy_active->canBePoisoned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 60 * effect_prob_mult;
            }
            // -1 speed opp
            unsigned int speed_mod = enemy_active->getSpeedModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + speed_mod);
            break;
        }
        case 223:{
            //wish
            unsigned int curr_hp_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(curr_hp_percent >= 50)
                total_utility -= 10;
            else if(curr_hp_percent >= 25)
                total_utility += 30;
            else if(curr_hp_percent >= 10)
                total_utility += 50;
            else
                total_utility += 80;
            break;
        }
        case 224:{
            //give status to opponent
            if(cpu_active->hasPermanentStatus())
                total_utility += 20;
            if(enemy_active->hasPermanentStatus())
                total_utility -= 10;
            else{
                switch(cpu_active->getPermanentStatus()){
                    case BURN:
                        if(enemy_active->canBeBurned() &&
                            (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                            field->getTerrain()!=MISTY_FIELD){
                            total_utility += 60 * effect_prob_mult;
                        }
                        break;
                    case PARALYSIS:
                        if(enemy_active->canBeParalyzed() &&
                            (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                            field->getTerrain()!=MISTY_FIELD){
                            total_utility += 60 * effect_prob_mult;
                        }
                        break;  
                    case POISON:
                        if(enemy_active->canBePoisoned() &&
                            (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                            field->getTerrain()!=MISTY_FIELD){
                            total_utility += 60 * effect_prob_mult;
                        }
                        break;
                    case BAD_POISON:
                        if(enemy_active->canBeBadlyPoisoned() &&
                            (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                            field->getTerrain()!=MISTY_FIELD){
                            total_utility += 60 * effect_prob_mult;
                        }
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        case 225:{
            // +3 DEF USER
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            total_utility += 15 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            break;
        }
        case 228:{
            // -1 att spatt opponent
            unsigned int attack_mod = enemy_active->getAttackModifier();
            unsigned int special_attack_mod = enemy_active->getSpecialAttackModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_attack_mod);
            break;
        }
        case 230:{
            //utiility is doubled if user does not hold items
            if(!cpu_active->hasHeldItem())
                total_utility *= 2;
            break;
        }
        case 234:{
            //torment opponent
            if(!enemy_active->hasVolatileCondition(TORMENTED))
                total_utility += 60;
            else
                total_utility -= 10;
            break;
        }
        case 237:{
            //pain split
            unsigned int user_hp = cpu_active->getCurrentHP();
            unsigned int enemy_hp = enemy_active->getCurrentHP();
            if(enemy_hp<=user_hp){
                total_utility-=25;
                break;
            }
            unsigned int percent = (user_hp*100)/enemy_hp;
            total_utility += 50 - percent;
            break;
        }
        case 242:{
            //power split
            Stats user_stats = cpu_active->getStats();
            Stats enemy_stats = enemy_active->getStats();
            if(user_stats.getAtk() < enemy_stats.getAtk() + 1)
                total_utility += 20;
            if(user_stats.getSpatk() < enemy_stats.getSpatk() + 1)
                total_utility += 20;
            break;
        }
        case 243:{
            //guard split
            Stats user_stats = cpu_active->getStats();
            Stats enemy_stats = enemy_active->getStats();
            if(user_stats.getDef() < enemy_stats.getDef() + 1)
                total_utility += 15;
            if(user_stats.getSpdef() < enemy_stats.getSpdef() + 1)
                total_utility += 15;
            break;
        }
        case 244:{
            //power trick
            Stats user_stats = cpu_active->getStats();
            if(user_stats.getAtk() < user_stats.getDef())
                total_utility += 10;
            else
                total_utility -= 10;
            break;
        }
        case 246:{
            //utility is multiplied by amount of non fainted monsters in the team
            unsigned int non_fainted = 0;
            for(int i=0;i<cpu_team->getSize();i++){
                if(cpu_team->getMonster(i)->isFainted())
                    continue;
                non_fainted++;
            }
            total_utility *= non_fainted;
            break;
        }
        case 247:{
            //-1 att opponent + heal
            unsigned int attack_mod = enemy_active->getAttackModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility -= 5;
            else if(HP_percent >= 25)
                total_utility += 15;
            else if(HP_percent >= 10)
                total_utility += 25;
            else
                total_utility += 40;
            break;
        }
        case 248:{
            //grudge
            if(cpu_active->hasVolatileCondition(GRUDGED))
                total_utility -= 10;
            else
                total_utility += 25;
            break;
        }
        case 249:{
            //present, avoid it if possible
            total_utility = 7;
            break;
        }
        case 250:{
            //sketch, avoid it if possible
            total_utility = 7;
            break;
        }
        case 251:{
            //hits trice
            total_utility *= 3;
            break;
        }
        case 252:{
            //heal from status whole team
            for(int i=0;i<cpu_team->getSize();i++){
                Monster* cpu_monster = cpu_team->getMonster(i);
                if(cpu_monster->isFainted())
                    continue;
                else if(cpu_monster->getPermanentStatus() != NO_PERMANENT_CONDITION)
                    total_utility += 20;
            }
            break;
        }
        case 253:{
            //burn + automaticaly thaw user out
            if(cpu_active->canBeBurned() &&
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) || cpu_active->hasAbility(INFILTRATOR)) &&
                field->getTerrain()!=MISTY_FIELD){
                total_utility += 60 * effect_prob_mult;
            }
            if(cpu_active->isFrozen())
                total_utility += 100;
            break;
        }
        case 254:{
            //power doubles in non clea weather
            if(field->getWeather() != CLEAR)
                total_utility *= 2;
            break;
        }
        case 255:{
            //create a substitute and switch out
            if(!cpu_team->hasAliveBackup() || !cpu_active->canSwitchOut(enemy_active)){
                total_utility -= 50;
                break;
            }
            if(cpu_active->getCurrentHP() < cpu_active->getMaxHP()/2)
                total_utility -= 50;
            else if(cpu_active->getCurrentHP() < cpu_active->getMaxHP()*3.0/4)
                total_utility += 10;
            else
                total_utility += 70;
            break;
        }
        case 256:{
            //substitute
            if(cpu_active->getCurrentHP() < cpu_active->getMaxHP()/4)
                total_utility -= 10;
            else if(cpu_active->getCurrentHP() < cpu_active->getMaxHP()/2)
                total_utility += 20;
            else
                total_utility += 80;
            break;
        }
        case 257:{
            //power doubles if user is par, brn, or psn
            if(cpu_active->isPoisoned())
                total_utility *= 2;
            else if(cpu_active->isBurned())
                total_utility *= 2;
            else if(cpu_active->isParalyzed())
                total_utility *= 2;
            break;
        }
        case 260:{
            //clear terrain
            if(field->getTerrain() != NO_TERRAIN)
                total_utility += 20;
            break;
        }
        case 262:{
            //start hail
            if(field->getWeather() == HAIL || field->weatherCannotChange())
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 265:{
            //user gives its ability to target
            if(enemy_active->hasAbility(cpu_active->getAbility()) || abilityCannotBeChanged(enemy_active->getAbility()))
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 266:{
            //stronger under psychic terrain
            if(field->getTerrain() == PSYCHIC_FIELD)
                total_utility *= 1.5/1.3;
            break;
        }
        case 267:{
            //target gets heal block
            if(enemy_active->hasVolatileCondition(HEAL_BLOCKED))
                total_utility -= 10;
            else
                total_utility += 10;
            break;
        }
        case 271:{
            //magic room
            if(field->hasFullFieldEffect(MAGIC_ROOM))
                total_utility -= 10;
            else
                total_utility += 40;
            break;
        }
        case 272:{
            //user and target swap abilities
            if(cpu_active->hasAbility(enemy_active->getAbility()) || 
                abilityCannotBeChanged(cpu_active->getAbility()) ||
                abilityCannotBeCopied(enemy_active->getAbility()))
                total_utility -= 10;
            else
                total_utility += 10;
            break;
        }
        case 273:{
            //psychic terrain
            if(field->getTerrain() == PSYCHIC_FIELD)
                total_utility -= 10;
            else
                total_utility += 50;
            break;
        }
        case 276:{
            //user dies if under 50% and takes 1/2 recoil otherwise
            unsigned int HP_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(HP_percent >= 50)
                total_utility *= 0.6;
            else
                total_utility = -90;
            break;
        }
        case 278:{
            //meteor beam: raise user spatt and attack one turn after
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            total_utility += 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            break;
        }
        case 279:{
            //bad attack, avoid it
            total_utility -= 30;
            break;
        }
        case 280:{
            //fails if target does not hold an item
            if(!enemy_active->hasHeldItem())
                total_utility = -10; //attack is going to fail
            break;
        }
        case 282:{
            //raise def of all grass types
            if(cpu_active->hasType(GRASS)){
                //+1 def user
                unsigned int defense_mod = cpu_active->getDefenseModifier();
                total_utility += 5 * (actual_stat_zero - defense_mod) * effect_prob_mult;
            }
            if(enemy_active->hasType(GRASS)){
                //+1 def enemy
                unsigned int defense_mod = enemy_active->getDefenseModifier();
                total_utility -= 5 * effect_prob_mult * (actual_stat_zero + defense_mod);
            }
            break;
        }
        case 283:{
            //reflect enemy attack
            // 25% for a +30 utility
            if(RNG::oneFourth())
                total_utility += 30;
            break;
        }
        case 285:{
            //-1 att spatt spd opponent
            if(!enemy_active->isPoisoned())
                break;
            unsigned int attack_mod = enemy_active->getAttackModifier();
            unsigned int special_attack_mod = enemy_active->getSpecialAttackModifier();
            unsigned int speed_mod = enemy_active->getSpeedModifier();
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + attack_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + special_attack_mod);
            total_utility += 5 * effect_prob_mult * (actual_stat_zero + speed_mod);
            break;
        }
        case 290:{
            //user dies -> avoid at all costs
            total_utility = -187;
            break;
        }
        case 292:{
            //+1 spatt spdef user
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            total_utility += 5 * (actual_stat_zero - special_attack_mod) * effect_prob_mult;
            total_utility += 5 * (actual_stat_zero - special_defense_mod) * effect_prob_mult;
            //heal status
            if(cpu_active->hasPermanentStatus())
                total_utility += 20;
            break;
        }
        case 295:{
            //-1 def spdef speed user
            unsigned int defense_mod = cpu_active->getDefenseModifier();
            unsigned int special_defense_mod = cpu_active->getSpecialDefenseModifier();
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility -= 5 * effect_prob_mult * (actual_stat_zero + defense_mod);
            total_utility -= 5 * effect_prob_mult * (actual_stat_zero + special_defense_mod);
            total_utility -= 5 * effect_prob_mult * (actual_stat_zero + speed_mod);
            break;
        }
        case 297:{
            //+2 spd +1 att user
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int speed_mod = cpu_active->getSpeedModifier();
            total_utility += 5 * (actual_stat_zero - attack_mod) * effect_prob_mult;
            total_utility += 10 * (actual_stat_zero - speed_mod) * effect_prob_mult;
            break;
        }

        default: break;
    }
    // add field utility
    if(attack->getCategory()!=STATUS){
        if(field->getTerrain() == PSYCHIC_FIELD && attack_type==PSYCHIC)
            total_utility *= 1.3;
        else if(field->getTerrain() == GRASSY_FIELD && attack_type==GRASS)
            total_utility *= 1.3;
        else if(field->getTerrain() == MISTY_FIELD && attack_type==DRAGON)
            total_utility *= 0.5;
        else if(field->getTerrain() == ELECTRIC_FIELD && attack_type==ELECTRIC)
            total_utility *= 1.3;
        if((field->getWeather() == RAIN || field->getWeather()==HEAVY_RAIN) && attack_type==WATER)
            total_utility *= 1.5;
        else if((field->getWeather() == SUN || field->getWeather()==EXTREME_SUN) && attack_type==FIRE)
            total_utility *= 1.5;
        if(field->getWeather() == RAIN && attack_type==FIRE)
            total_utility *= 0.5;
        else if(field->getWeather() == SUN && attack_type==WATER)
            total_utility *= 0.5;
        if(field->getWeather()==EXTREME_SUN && attack_type==WATER)
            total_utility = -10;
        else if(field->getWeather()==HEAVY_RAIN && attack_type==FIRE)
            total_utility = -10;
    }
    total_utility *= accuracy / 90.0;
    // add some randomicity to the utility
    total_utility *= RNG::getRandomInteger(90,110)/100.0;
    // avoid selecting status when taunted
    if(cpu_active->hasVolatileCondition(TAUNTED) && attack->getCategory()==STATUS)
        total_utility = -20;
    return total_utility;
}

unsigned int CPUAI::chooseRandomAttack(Battler* active_monster)const{
    auto available_attacks = active_monster->getAttacks();
    std::vector<unsigned int> choices;
    for(auto it = available_attacks.begin(); it != available_attacks.end(); it++){
        if(it->second == 0){continue;}
        Attack* attack = Attack::getAttack(it->first);
        if(active_monster->getDisabledAttack() == it->first){continue;}
        if((active_monster->hasVolatileCondition(TAUNTED)||active_monster->hasHeldItem(ASSULT_VEST))&&
            attack->getCategory() == STATUS){
            continue;
        }
        if(active_monster->hasVolatileCondition(TORMENTED) && 
            active_monster->getLastAttackUsed() == it->first){
            continue;
        }
        choices.push_back(it->first);
    }
    if(choices.empty()){
        return STRUGGLE_ID;
    }
    unsigned int random_choice = RNG::getRandomInteger(0,choices.size()-1);
    return choices[random_choice];
}

BattleAction CPUAI::chooseAction(Battler* active_monster,MonsterTeam* monster_team, Battler* enemy_active,Field* field,Bag*bag)const{
    BattleAction forced_action = forcedAction(OPPONENT, active_monster,field);
    if(forced_action.getActionType() != ATTACK){return forced_action;}
    if(active_monster->hasVolatileCondition(ENCORE) || (
        (active_monster->hasHeldItem(CHOICE_BAND) ||
        active_monster->hasHeldItem(CHOICE_SPECS) ||
        active_monster->hasHeldItem(CHOICE_SCARF)) && active_monster->getLastAttackUsed() != 0
    )){
        // force use of encored move
        unsigned int attack_id = active_monster->getLastAttackUsed();
        Attack *attack = Attack::getAttack(attack_id);
        if(active_monster->hasVolatileCondition(ENCORE))
            active_monster->decrementVolatileCondition(ENCORE);
        return BattleAction(
            OPPONENT,
            ATTACK,
            attack_id,
            attack->getPriorityLevel(), 
            active_monster->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active_monster->getMonster());   
    }
    if(skill==0){// unskilled opponenets never switch and always attack randomly
        return BattleAction(
            OPPONENT,
            ATTACK,
            chooseRandomAttack(active_monster), 
            0, 
            active_monster->getModifiedSpeed(), 
            0,
            NO_ITEM_TYPE,
            false,
            active_monster->getMonster());
    }
    Choice * attack_choice = getBestAttackChoice(active_monster,monster_team,enemy_active,field);
    Choice * switch_choice = getBestSwitchChoice(active_monster,monster_team,enemy_active,field);
    Choice * item_choice = getBestItemChoice(active_monster,bag);
    if(switch_choice != nullptr && active_monster->hasVolatileCondition(PERISH_SONG))
        switch_choice->utility*=2.5;//increase utility of switching perish song condamned monster
    BattleAction struggle_action(
        OPPONENT,
        ATTACK,
        STRUGGLE_ID, 
        0, 
        active_monster->getModifiedSpeed(), 
        0,
        NO_ITEM_TYPE,
        false,
        active_monster->getMonster());
    bool mega_evolution_choice = active_monster->canMegaEvolve() && !monster_team->hasMega();
    bool can_switch_out = active_monster->canSwitchOut(enemy_active) || 
        active_monster->hasHeldItem(SHED_SHELL);
    if(attack_choice == nullptr && switch_choice == nullptr){
        if(item_choice!=nullptr)
            delete item_choice;
        return struggle_action;
    }else if(attack_choice == nullptr){
        unsigned int switch_id = switch_choice->choice_id;
        delete switch_choice;
        if(item_choice!=nullptr)
            delete item_choice;
        if(!can_switch_out)
            return struggle_action;
        return BattleAction(
            OPPONENT,
            SWITCH,
            0, 
            0, 
            active_monster->getModifiedSpeed(), 
            switch_id,
            NO_ITEM_TYPE,
            false,
            active_monster->getMonster());
    }else if(switch_choice == nullptr){
        unsigned int attack_id = attack_choice->choice_id;
        int attack_utility = attack_choice->utility;
        delete attack_choice;
        if(item_choice==nullptr || attack_utility >= item_choice->utility){
            if(item_choice!=nullptr)
                delete item_choice;
            Attack* attack = Attack::getAttack(attack_id);
            return BattleAction(
                OPPONENT,
                ATTACK,
                attack_id, 
                attack->getPriorityLevel(), 
                active_monster->getModifiedSpeed(), 
                0,
                NO_ITEM_TYPE,
                mega_evolution_choice,
                active_monster->getMonster());
        }else{
            ItemType item_id = item_choice->item;
            delete item_choice;
            return BattleAction(
                OPPONENT,
                USE_ITEM,
                0, 
                0, 
                active_monster->getModifiedSpeed(), 
                0,
                item_id,
                false,
                active_monster->getMonster());
        }
    }else if(attack_choice->utility >= switch_choice->utility || !can_switch_out){
        unsigned int attack_id = attack_choice->choice_id;
        int attack_utility = attack_choice->utility;
        delete attack_choice;
        if(item_choice==nullptr || attack_utility >= item_choice->utility){
            if(item_choice!=nullptr)
                delete item_choice;
            Attack* attack = Attack::getAttack(attack_id);
            return BattleAction(
                OPPONENT,
                ATTACK,
                attack_id, 
                attack->getPriorityLevel(), 
                active_monster->getModifiedSpeed(), 
                0,
                NO_ITEM_TYPE,
                mega_evolution_choice,
                active_monster->getMonster());
        }else{
            ItemType item_id = item_choice->item;
            delete item_choice;
            return BattleAction(
                OPPONENT,
                USE_ITEM,
                0, 
                0, 
                active_monster->getModifiedSpeed(), 
                0,
                item_id,
                false,
                active_monster->getMonster());
        }
    }else{
        unsigned int switch_id = switch_choice->choice_id;
        delete switch_choice;
        delete attack_choice;
        return BattleAction(
            OPPONENT,
            SWITCH,
            0, 
            0, 
            active_monster->getModifiedSpeed(), 
            switch_id,
            NO_ITEM_TYPE,
            false,
            active_monster->getMonster());
    }
}

Choice* CPUAI::getBestAttackChoice(Battler* active_user,MonsterTeam*cpu_team,Battler*active_target,Field*field)const{
    auto choices = active_user->getAttacks();
    if(choices.empty()){
        return nullptr;
    }
    unsigned int best_choice_id = 0;
    int best_utility = -100000;
    for(auto choice: choices){
        unsigned int attack_id = choice.first;
        unsigned int curr_pp = choice.second;
        if(curr_pp==0){continue;}
        if(attack_id==0){continue;}
        if(active_user->getDisabledAttack() == attack_id){continue;}
        if((active_user->hasVolatileCondition(TORMENTED) || active_user->hasHeldItem(ASSULT_VEST)) && 
            active_user->getLastAttackUsed() == attack_id){//cannot use the same move twice in a row
            continue;
        }
        Attack* attack = Attack::getAttack(attack_id);
        if(attack->getCategory() == STATUS && active_user->hasVolatileCondition(TAUNTED))//cannot use status moves when taunted
            continue;
        int utility = computeAttackUtility(attack_id,active_user,cpu_team,active_target,field);
        if(utility > best_utility){
            best_choice_id = attack_id;
            best_utility = utility;
        }
    }
    if(best_utility == -100000)
        return nullptr;
    return new Choice(
        best_choice_id,
        best_utility,
        ATTACK
    );
}

Choice* CPUAI::getBestSwitchChoice(Battler*active_user,MonsterTeam*user_team,Battler*active_target,Field*field)const{
    if(!user_team->hasAliveBackup()){
        return nullptr;
    }
    auto choices = user_team->getMonsters();
    unsigned int best_choice_id = 0;
    int best_utility = -100000;
    for(unsigned int i=1; i<choices.size();i++){
        if(choices[i]->isFainted()){
            continue;
        }
        auto monster_attacks = choices[i]->getAttacks();
        int curr_utility = 0;
        for(auto attack: monster_attacks){
            unsigned int attack_id = attack.first;
            unsigned int curr_pps = attack.second;
            if(attack_id == 0){continue;}
            if(curr_pps == 0){continue;}
            int attack_utility = computeAttackUtility(attack_id,active_user,user_team,active_target,field);
            if(attack_utility > curr_utility){
                curr_utility = attack_utility * 0.5;
            }
        }
        if(curr_utility > best_utility){
            best_choice_id = i;
            best_utility = curr_utility;
        }
    }
    if(best_utility == -100000){
        return nullptr;
    }
    return new Choice(
        best_choice_id,
        best_utility,
        SWITCH
    );
}

unsigned int CPUAI::chooseSwitch(Battler*active_user,MonsterTeam*user_team,Battler*active_target,Field*field)const{
    Choice * choice = getBestSwitchChoice(active_user,user_team,active_target,field);
    if(choice == nullptr){
        return 0;
    }
    unsigned int choice_id = choice->choice_id;
    delete choice;
    return choice_id;
}

int CPUAI::computeItemUtility(ItemType item_id, Battler* cpu_active)const{
    if(item_id == NO_ITEM_TYPE)
        return -100000;
    if(!cpu_active->itemWouldHaveEffect(item_id))
        return -10000;
    int total_utility = 0;
    //healing items
    double heal_percent = 0;
    switch(item_id){
        case ORAN_BERRY:
            heal_percent = min(100,1000 / cpu_active->getMaxHP());
            break;
        case SITRUS_BERRY:
            heal_percent = 25;
            break;
        case POTION:
        case BERRY_JUICE:
            heal_percent = min(100,2000 / cpu_active->getMaxHP());
            break;
        case SUPER_POTION:
            heal_percent = min(100,6000 / cpu_active->getMaxHP());
            break;
        case HYPER_POTION:
            heal_percent = min(100,12000 / cpu_active->getMaxHP());
            break;
        case MAX_POTION:
        case FULL_RESTORE:
            heal_percent = 100;
            break;
        default:
            break;
    }
    double healable_HP_percent =  100.0 - (cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP());
    double actual_heal = min(heal_percent,healable_HP_percent);
    total_utility += actual_heal + (healable_HP_percent/5.0);//incentivize healing at low health
    // status healing items
    switch(item_id){
        case ANTIDOTE:
        case PECHA_BERRY:
            if(cpu_active->isPoisoned())
                total_utility += 100;
            break;
        case AWAKENING:
        case CHESTO_BERRY:
            if(cpu_active->isAsleep())
                total_utility += 100;
            break;
        case PARALYZE_HEAL:
        case CHERY_BERRY:
            if(cpu_active->isParalyzed())
                total_utility += 100;
            break;
        case ICE_HEAL:
        case ASPEAR_BERRY:
            if(cpu_active->isFrozen())
                total_utility += 100;
            break;
        case BURN_HEAL:
        case RAWST_BERRY:
            if(cpu_active->isBurned())
                total_utility += 100;
            break;
        case FULL_HEAL:
        case LUM_BERRY:
        case FULL_RESTORE:
            if(cpu_active->hasPermanentStatus() || cpu_active->hasVolatileCondition(CONFUSION))
                total_utility += 99;//1 minus of status specific in order to incentivize status specific item use
            break;
        default: break;
    }
    if(total_utility==0)
        total_utility = -50;
    return total_utility;
}

Choice* CPUAI::getBestItemChoice(Battler*active_user,Bag*bag)const{
    if(bag->getPocket(MEDICINE) == nullptr)
        return nullptr;
    auto items = bag->getItemsInPocket(MEDICINE);
    if(items.empty())
        return nullptr;
    ItemType best_item = NO_ITEM_TYPE;
    int best_utility = -100000;
    for(auto item: items){
        ItemType item_type = item.first;
        if(item_type == NO_ITEM_TYPE){continue;}
        int utility = computeItemUtility(item_type,active_user);
        if(utility > best_utility){
            best_item = item_type;
            best_utility = utility;
        }
    }
    if(best_utility == -100000)
        return nullptr;
    return new Choice(
        best_item,
        best_utility
    );
}