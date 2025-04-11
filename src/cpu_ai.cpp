#include "cpu_ai.h"

// Choice

Choice::Choice(){
    choice_id = 0;
    utility = 0;
    action_type = ATTACK;
}
Choice::Choice(unsigned int choice_id, int utility, BattleActionType action_type){
    this->choice_id = choice_id;
    this->utility = utility;
    this->action_type = action_type;
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

int CPUAI::computeAttackUtility(unsigned int attack_id, Battler* cpu_active,Battler* enemy_active, Field* field)const{
    Attack* attack = Attack::getAttack(attack_id);
    unsigned int effect = attack->getEffectId();
    // if(cpu_active->isFainted()){
    //     return -10000;
    // }
    unsigned int accuracy = attack->getAccuracy();
    if(accuracy==ALWAYS_HITS)
        accuracy = 100;
    int total_utility = 10;
    int sleep_mult = 1;
    if(enemy_active->isAsleep())
        sleep_mult=10;
    double screen_mult = 1;
    if(field->hasFieldEffect(REFLECT,OPPONENT) && attack->getCategory() == PHYSICAL){
        screen_mult = 0.5;
    }
    if(field->hasFieldEffect(LIGHT_SCREEN,OPPONENT) && attack->getCategory() == SPECIAL){
        screen_mult = 0.5;
    }
    if(attack->getPower() != 0){
        bool can_hit_ghosts = (cpu_active->getAbility()==SCRAPPY);
        float effectiveness = getTypeEffectiveness(attack->getType(), 
            enemy_active->getTypes(),enemy_active->isTouchingGround(),
            can_hit_ghosts,attack->getEffectId() == 196);
        unsigned int damage = attack->getPower();
        unsigned int unmodified_damage = damage;
        if(attack->getEffectId() == 36)// triple expected damage for multi hit moves
            damage*=3;
        if(attack->getEffectId() == 37 && (cpu_active->getLastAttackUsed() == attack_id))//fury cutter
            damage *= 2; // expect double damage from multiple uses of fury cutter
        total_utility+= effectiveness * damage * accuracy / 90 * sleep_mult * screen_mult;
        if(unmodified_damage > enemy_active->getCurrentHP()){
            // I see a kill!!!
            total_utility *= 50;
        }
    }
    double status_mult = 1;
    unsigned int effect_probability = attack->getEffectChance();
    if(attack->getCategory() == STATUS){
        total_utility = 37;
        status_mult = 8.3;
    }
    switch(effect){
        case 1: { // lower attack by 1
            int atk_modifier = enemy_active->getAttackModifier();
            if(atk_modifier == -6)
                total_utility -= 3*status_mult;
            else
                total_utility += (20 + atk_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 46: { // lower attack by 2
            int atk_modifier = enemy_active->getAttackModifier();
            if(atk_modifier == -6)
                total_utility -= 3*status_mult;
            else
                total_utility += (20 + atk_modifier * 100 / 6)*0.66*status_mult;
            break;
        }
        case 2: { // increase Att SpAtk by 1
            int atk_modifier = cpu_active->getAttackModifier();
            int sp_atk_modifier = cpu_active->getSpecialAttackModifier();
            if(atk_modifier == 6 || sp_atk_modifier == 6)
                total_utility -= 10 * status_mult;
            else
                total_utility = (20 - atk_modifier * 100 / 6 - sp_atk_modifier * 100 / 6) * status_mult * 0.25;
            break;
        }
        case 3: { // leech seed
            if((enemy_active->hasType(GRASS))||(enemy_active->hasVolatileCondition(LEECH_SEED)))
                return -150;
            else 
                return 120;
            break;
        }
        case 4: case 76: { // poison & bad poison
            if(!enemy_active->canBePoisoned()||
            (field->hasFieldEffect(SAFEGUARD,PLAYER) &&
            cpu_active->getAbility()!=INFILTRATOR))
                total_utility -= 15 * status_mult;
            else 
                total_utility += 20 * status_mult;
            if(effect== 76)
                total_utility += 2 * status_mult;
            break;
        }
        case 5: { // sleep
            Type move_type = attack->getType();
            if((move_type==GRASS && enemy_active->hasType(GRASS)) || 
                !enemy_active->canFallAsleep() ||
                (field->hasFieldEffect(SAFEGUARD,PLAYER) &&
                cpu_active->getAbility()!=INFILTRATOR))
                total_utility -= 5 * status_mult;
            else 
                total_utility += 5 * status_mult;
            break;
        }
        case 6: {//recoil 1/4
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 50)
                total_utility -= 5;
            else if(health_percent < 80)
                total_utility -=20;
            break;
        }
        case 7: { // decrease evasion by 2
            int evasion_modifier = enemy_active->getEvasionModifier();
            if(evasion_modifier == -6)
                total_utility-=10*status_mult;
            else 
                total_utility += (10 + evasion_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 8: { // synthesis
            unsigned int remaining_hp_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            Weather weather = field->getWeather();
            float wheater_multiplier = 1.0;
            if((weather != CLEAR)&&(weather!=SUN))
                wheater_multiplier = 0.4;
            else if(weather == SUN)
                wheater_multiplier = 1.25;
            if(remaining_hp_percent < 50)
                total_utility += 50 * wheater_multiplier;
            else if(remaining_hp_percent < 75)
                total_utility += 25 * wheater_multiplier;
            else
                total_utility -= 30;
            break;
        }
        case 13:{// Worry Seed
            if(enemy_active->getAbility()!=INSOMNIA)
                total_utility += 15;
            else
                total_utility -= 30;
            break;
        }
        case 14:{//Burn
            if(!enemy_active->canBeBurned() ||
                (attack->getType() == FIRE && enemy_active->hasType(FIRE))||
                (field->hasFieldEffect(SAFEGUARD,PLAYER) &&
                cpu_active->getAbility()!=INFILTRATOR))
                total_utility -= 10 * status_mult;
            else
                total_utility += 20 * status_mult;
            break;
        }
        case 15:{//accuracy -1
            int acc_modifier = enemy_active->getAccuracyModifier();
            if(acc_modifier == -6)
                total_utility-= 4 * status_mult;
            else 
                total_utility += (5 + acc_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 16:{// paralisis
            if( !enemy_active->canBeParalyzed() ||
                (enemy_active->hasType(GRASS) && attack->getType() == GRASS) ||
                (field->hasFieldEffect(SAFEGUARD,PLAYER) &&
                cpu_active->getAbility()!=INFILTRATOR))
                total_utility -= 4 * status_mult;
            else
                total_utility += 10 * status_mult + effect_probability / 5;
            break;
        }
        case 17:{ //increased crit
            total_utility *= 1.25;
            break;
        }
        case 18:{// speed -2 opponent
            int spd_modifier = enemy_active->getSpeedModifier();
            if(spd_modifier == -6)
                total_utility-= 10 * status_mult;
            else 
                total_utility += (10 + spd_modifier * 100 / 6)*0.66*status_mult;
            break;
        }
        case 19:{//Fire spin
            if(!enemy_active->hasVolatileCondition(FIRESPIN))
                total_utility += 30;
            break;
        }
        case 74:{//Sand Tomb
            if(!enemy_active->hasVolatileCondition(SANDTOMB))
                total_utility += 30;
            break;
        }
        case 20:{//recoil + burn
            if(enemy_active->hasPermanentStatus() ||
            (attack->getType() == FIRE && enemy_active->hasType(FIRE)))
                total_utility -= 10;
            else
                total_utility += 20;
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 50)
                total_utility -= 20;
            else if(health_percent < 80)
                total_utility -= 40;
            break;
        }
        case 21:case 141:{//flinch
            total_utility += 7 * effect_probability/10;
        }
        case 22:{// -1 def opponent
            int def_modifier = enemy_active->getDefenseModifier();
            if(def_modifier == -6)
                total_utility-= 5 * status_mult;
            else 
                total_utility += (10 + def_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 23:{// +1 def user
            int def_modifier = cpu_active->getDefenseModifier();
            if(def_modifier == -6)
                total_utility -= 3*status_mult;
            else
                total_utility += (20 + def_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 24:{ // rapid spin
            int spd_modifier = cpu_active->getSpeedModifier();
            if(spd_modifier == 6)
                total_utility -= 3*status_mult;
            else
                total_utility += (20 - spd_modifier * 100 / 6)*0.25*status_mult;
            if(field->hasFieldEffectsSuchThat(&isFieldEffectClearedByRapidSpin,OPPONENT))
                total_utility += 50;
            if(cpu_active->hasVolatilesSuchThat(&isVolatileConditionClearedByRapidSpin))
                total_utility += 50;
            break;
        }
        case 25:case 77:{ // confuse
            if(!enemy_active->hasVolatileCondition(CONFUSION))
                total_utility += 10 * status_mult;
            else
                total_utility -= 10 * status_mult;
            if(effect==77)//confuse while raising spatt
                total_utility -= 10;
            break;
        }
        case 26:{ // PROTECT
            if(cpu_active->getConsecutiveProtects() != 0){
                total_utility -= 50;
            }else{
                total_utility += 50;
                if(enemy_active->isBurned() || enemy_active->isPoisoned() ||
                    enemy_active->hasVolatileCondition(LEECH_SEED) || 
                    enemy_active->hasVolatileCondition(WRAP) ||
                    enemy_active->hasVolatileCondition(FIRESPIN) ||
                    enemy_active->hasVolatileCondition(SANDTOMB) ||
                    enemy_active->hasVolatileCondition(UNDERGROUND) ||
                    enemy_active->hasVolatileCondition(ROLLINGOUT) ||
                    enemy_active->hasVolatileCondition(OUTRAGING) ||
                    enemy_active->hasVolatileCondition(THRASHING) ||
                    enemy_active->hasVolatileCondition(UNDERWATER) ||
                    enemy_active->hasVolatileCondition(FLYING_HIGH))
                    total_utility+=100;
            }
            break;
        }
        case 27:{// set rain
            if(field->getWeather()==RAIN)
                total_utility-=30*status_mult;
            else
                total_utility+=50*status_mult;
            break;
        }
        case 28:{// +2 ATT SPATT SPD, -1 DEF SPDEF
            unsigned int attack_mod = cpu_active->getAttackModifier();
            unsigned int special_attack_mod = cpu_active->getSpecialAttackModifier();
            if(attack_mod <= 1 || special_attack_mod <= 1)
                total_utility += 25 * status_mult;
            else
                total_utility -= 50 * status_mult;
            break;
        }
        case 29:{// +2 def user
            int def_modifier = cpu_active->getDefenseModifier();
            if(def_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (20 - def_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 30:{//+1 speed user, 33% recoil
            int spd_modifier = cpu_active->getSpeedModifier();
            if(spd_modifier == 6)
                total_utility -= 3*status_mult;
            else
                total_utility += (20 - spd_modifier * 100 / 6)*0.25*status_mult;
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 50)
                total_utility -= 20;
            else if(health_percent < 80)
                total_utility -= 40;
            break;
        }
        case 31:{// -1 sp def opponent
            int spdef_modifier = enemy_active->getSpecialDefenseModifier();
            if(spdef_modifier == -6)
                total_utility-= 5 * status_mult;
            else 
                total_utility += (10 + spdef_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 32:case 123:{//force switch
            std::set<Type> enemy_types = enemy_active->getTypes();
            bool has_effective_type = false;
            for(auto enemy_type: enemy_types){
                float effectiveness = getTypeEffectiveness(enemy_type,cpu_active->getTypes(),enemy_active->isTouchingGround(),false,false);
                if(effectiveness>1.1)
                    has_effective_type = true;
            }
            if(has_effective_type)
                total_utility += 20;
            else
                total_utility -= 100;
        }
        case 33:{//safeguard
            if(field->hasFieldEffect(SAFEGUARD,OPPONENT))
                total_utility -= 120;
            else
                total_utility += 30;
        }
        case 34:{//tailwind
            if(field->hasFieldEffect(TAILWIND,OPPONENT))
                total_utility -= 120;
            else
                total_utility += 35;
        }
        case 35:{// +1 spatt spdef speed user
            int spdef_modifier = cpu_active->getSpecialDefenseModifier();
            int spatt_modifier = cpu_active->getSpecialAttackModifier();
            int speed_modifier = cpu_active->getSpeedModifier();
            if(spdef_modifier == 6 || spatt_modifier == 6 || speed_modifier==6)
                total_utility -= 10 * status_mult;
            else
                total_utility = (40 - spdef_modifier * 100 / 6 - spatt_modifier * 100 / 6 - spdef_modifier*100/6 ) * status_mult * 0.8;
            break;
        }
        case 37:{//fury cutter
            total_utility += 20; // help set up fury cutter combo
        }
        case 38:{// laser focus
            if(cpu_active->hasVolatileCondition(LASER_FOCUS))
                total_utility -= 100;
            else
                total_utility += RNG::getRandomInteger(50,150);
            break;
        }
        case 39:{// focus energy
            if(cpu_active->hasVolatileCondition(FOCUS_ENERGY))
                total_utility -= 156;
            else
                total_utility += 120;
            break;
        }
        case 40:{//venoshock
            if(enemy_active->isPoisoned())
                total_utility *= 2;
        }
        case 41:{//Toxic spikes
            if(field->hasFieldEffect(BAD_TOXIC_SPIKES,OPPONENT))
                total_utility -= 150;
            else if (field->hasFieldEffect(TOXIC_SPIKES,OPPONENT))
                total_utility += 75;
            else
                total_utility += 150;
            break;
        }
        case 42:{// +2 SPEED user
            int spd_modifier = cpu_active->getSpeedModifier();
            if(spd_modifier == 6)
                total_utility -= 20*status_mult;
            else
                total_utility += (20 - spd_modifier * 100 / 6)*0.2*status_mult;
            break;
        }
        case 43:{ // Endeavor
            unsigned int current_hp_active = cpu_active->getCurrentHP();
            unsigned int current_hp_enemy = enemy_active->getCurrentHP();
            if(current_hp_enemy <= current_hp_active)
                total_utility -= 120;
            else
                total_utility += 2 * (current_hp_enemy - current_hp_active);
            break;
        }
        case 44:{ // gain 2 ATT if kills
            unsigned int current_hp_enemy = enemy_active->getCurrentHP();
            unsigned int total_hp_enemy = enemy_active->getMaxHP();
            if(current_hp_enemy < total_hp_enemy / 10)
                total_utility += 100;
            else
                total_utility -= 10;
            break;
        }
        case 45:{//hits monster that is using FLY, BOUNCE or SKY DROP + 20% flinch
            total_utility += 15;// add utility for the chance of flinching
            break;
        }
        case 47:{//roost
            if(cpu_active->isAtFullHP()){
                total_utility -= 100;
            }else{
                if(cpu_active->getCurrentHP() < cpu_active->getMaxHP() * 0.25)
                    total_utility += 100;
                else if(cpu_active->getCurrentHP() < cpu_active->getMaxHP() * 0.5)
                    total_utility += 55;
                else
                    total_utility -= 20;
            }
            break;
        }
        case 50:{//halves target HP
            unsigned int current_hp_enemy = enemy_active->getCurrentHP();
            total_utility += current_hp_enemy * accuracy / 200;
            break;
        }
        case 51:{// recoil 1/3
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 50)
                total_utility -= 20;
            else if(health_percent < 80)
                total_utility -= 40;
            break;
        }
        case 52:{ // +2 att
            int att_modifier = cpu_active->getAttackModifier();
            if(att_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (40 - att_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 53:{//wrap
            if(!enemy_active->hasVolatileCondition(WRAP))
                total_utility += 20;
            else
                total_utility -= 30;
            break;
        }
        case 54:{//-2 SPDEF opponent
            int spdef_modifier = enemy_active->getSpecialDefenseModifier();
            if(spdef_modifier == -6)
                total_utility-= 5 * status_mult;
            else 
                total_utility += (10 + spdef_modifier * 100 / 6)*0.45*status_mult;
            break;
        }
        case 55:{//Stockpile
            if(cpu_active->getStockpiles() == 3)
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 56:{//swallow
            double health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(cpu_active->getStockpiles() == 0)
                total_utility -= 100;
            else if(cpu_active->getStockpiles() == 1)
                total_utility += 1 * (100-health_percent);
            else if(cpu_active->getStockpiles() == 2)
                total_utility += 7 * (100-health_percent);
            else
                total_utility += 20 * (100-health_percent);
        }
        case 57:{//spit up
            if(cpu_active->getStockpiles() == 0)
                total_utility -= 100; 
            else if(cpu_active->getStockpiles() == 2)
                total_utility*=2;
            else if(cpu_active->getStockpiles() == 3)
                total_utility*=3;
            break;
        }
        case 58:{//suppress ability
            if(enemy_active->isAbilitySuppressed())
                total_utility -= 200;
            else
                total_utility += 30;
            break;
        }
        case 59:{//remove stat changes
            int total_stat_changes_cpu = cpu_active->getAttackModifier() + 
                cpu_active->getSpecialAttackModifier() +
                cpu_active->getDefenseModifier() +
                cpu_active->getSpecialDefenseModifier() +
                cpu_active->getSpeedModifier();
            int total_stat_changes_enemy = enemy_active->getAttackModifier() +
                enemy_active->getSpecialAttackModifier() +
                enemy_active->getDefenseModifier() +
                enemy_active->getSpecialDefenseModifier() +
                enemy_active->getSpeedModifier();
            int diff = total_stat_changes_enemy - total_stat_changes_cpu;
            total_utility += 25*diff;
            break;
        }
        case 60:{// +1 ATT DEF ACC user
            int att_modifier = cpu_active->getAttackModifier();
            int def_modifier = cpu_active->getDefenseModifier();
            int acc_modifier = cpu_active->getAccuracyModifier();
            if(att_modifier == 6 || def_modifier == 6 || acc_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (60 - att_modifier * 100 / 6 - def_modifier * 100 / 6 - acc_modifier*100/6 ) * status_mult * 0.35;
            break;
        }
        case 61:{//FREEZE OPPONENT
            if(attack->getCategory() == STATUS){
                if(enemy_active->canBeFrozen() && 
                (!field->hasFieldEffect(SAFEGUARD,PLAYER) ||
                cpu_active->getAbility()!=INFILTRATOR)){
                    total_utility += 110 * accuracy / 100;
                }else{
                    total_utility -= 100;
                }
            }else
                total_utility += 7;
            break;
        }
        case 62:{// +2 spatt user
            int spatt_modifier = cpu_active->getSpecialAttackModifier();
            if(spatt_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (40 - spatt_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 63:{//+1 evasiveness user
            int evasion_modifier = cpu_active->getEvasionModifier();
            if(evasion_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (40 - evasion_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 64:{// damage depends on speed delta
            int speed_delta = cpu_active->getModifiedSpeed() - enemy_active->getModifiedSpeed();
            if(speed_delta > 0)
                total_utility += 20 * speed_delta;
            else
                total_utility += 20;
            break;
        }
        case 66:{//light screen
            if(field->hasFieldEffect(LIGHT_SCREEN,OPPONENT))
                total_utility -= 120;
            else
                total_utility += 160;
            break;
        }
        case 67:{//paralyze + accuracy under rain
            if(enemy_active->hasPermanentStatus() || enemy_active->hasType(ELECTRIC))
                total_utility -= 10;
            else
                total_utility += 10;
            if(field->getWeather() == RAIN)
                total_utility *= 1.5;
            else if(field->getWeather() == SUN)
                total_utility /= 4.0;
            break;
        }
        case 68:{// rollout
            break;
        }
        case 69:{//- 1 spped opponent    
            int spd_modifier = enemy_active->getSpeedModifier();
            if(spd_modifier == -6)
                total_utility-= 10 * status_mult;
            else 
                total_utility += (10 + spd_modifier * 100 / 6)*0.25*status_mult;
            if(field->getTerrain()==GRASSY_FIELD)
                total_utility *= 0.5;
            break;        
        }
        case 70:{//dig
            break;
        }
        case 71:{//gyro ball
            int speed_delta = enemy_active->getModifiedSpeed() - cpu_active->getModifiedSpeed();
            if(speed_delta > 0)
                total_utility += 20 * speed_delta;
            else
                total_utility += 20;
            break;
        }
        case 72:{//start sandstorm
            if(field->getWeather()==SANDSTORM)
                total_utility -= 170;
            else
                total_utility += 120;
            break;
        }
        case 73:{// +1 sp def user
            if(field->getTerrain()==GRASSY_FIELD)
                total_utility *= 0.5;
            break;
        }
        case 75:{//two hits
            total_utility *= 2;
            break;
        }
        case 78:{// -1 ATT DEF user
            int att_modifier = cpu_active->getAttackModifier();
            int def_modifier = cpu_active->getDefenseModifier();
            total_utility -= (10 - att_modifier * 100 / 6 - def_modifier * 100 / 6) * status_mult * 0.1;
            break;
        }
        case 79:{//do nothing
            total_utility -= 200;
            break;
        }
        case 80:{// copycat
            total_utility += 10;
        }
        case 81:{// power depends on stat changes
            int total_stat_changes = 1;//for base power
            if(cpu_active->getAttackModifier() > 0)
                total_stat_changes += cpu_active->getAttackModifier();
            if(cpu_active->getSpecialAttackModifier() > 0)
                total_stat_changes += cpu_active->getSpecialAttackModifier();
            if(cpu_active->getDefenseModifier() > 0)
                total_stat_changes += cpu_active->getDefenseModifier();
            if(cpu_active->getSpecialDefenseModifier() > 0)
                total_stat_changes += cpu_active->getSpecialDefenseModifier();
            if(cpu_active->getSpeedModifier() > 0)
                total_stat_changes += cpu_active->getSpeedModifier();
            total_utility *= total_stat_changes;
            break;
        }
        case 82:{// encore enemy
            if(enemy_active->hasVolatileCondition(ENCORE))
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 83:{//heal 25%
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 25)
                total_utility += 25;
            else if(health_percent < 50)
                total_utility += 12;
            else
                total_utility -= 5;
            break;
        }
        case 84:{//metronome
            total_utility += 100;
            break;
        }
        case 85:{//gravity
            if(field->hasFullFieldEffect(GRAVITY))
                total_utility -= 120;
            else
                total_utility += 70;
            break;
        }
        case 86:{// -1 ATT user
            int att_modifier = cpu_active->getAttackModifier();
            total_utility -= (20 + att_modifier * 60 / 6)*0.2*status_mult;
            break;
        }
        case 87:{//+1 DEF SPEF user
            int def_modifier = cpu_active->getDefenseModifier();
            int spdef_modifier = cpu_active->getSpecialDefenseModifier();
            if(def_modifier == 6 || spdef_modifier == 6)
                total_utility -= 10 * status_mult;
            else
                total_utility = (40 - def_modifier * 100 / 6 - spdef_modifier * 100 / 6) * status_mult * 0.35;
            break;
        }
        case 88:{//-1 spatt opponent
            int spatt_modifier = enemy_active->getSpecialAttackModifier();
            if(spatt_modifier == -6)
                total_utility-= 5 * status_mult;
            else 
                total_utility += (10 + spatt_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 89:{// user dies and wheìat comes in is fully healed
            total_utility -= 50;
            break;
        }
        case 90:{//disable move
            if(enemy_active->hasDiabledAttack() || enemy_active->getLastAttackUsed() == 0)
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 91:{// - 4 PP opponent last move
            total_utility += 20;
            break;
        }
        case 92:{// imprison
            if(enemy_active->hasVolatileCondition(IMPRISON))
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 93:{//mimic
            total_utility+=40;
            break;
        }
        case 95:{//rest
            if(cpu_active->isAtFullHP()){
                total_utility -= 100;
            }else{
                if(cpu_active->getCurrentHP() < cpu_active->getMaxHP() * 0.25)
                    total_utility += 100;
                else if(cpu_active->getCurrentHP() < cpu_active->getMaxHP() * 0.5)
                    total_utility += 55;
                else
                    total_utility -= 20;
            }
            break;
        }
        case 96:{//mean look
            if(enemy_active->hasVolatileCondition(MEAN_LOOK))
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 97:{//heal 50% damage dealt to opponent
            total_utility *= 1.25;
            break;
        }
        case 98:{// quick guard
            total_utility += 15;
            break;
        }
        case 99:{//set up grassy terrain
            if(field->getTerrain()==GRASSY_FIELD)
                total_utility -= 170;
            else
                total_utility += 120;
            break;
        }
        case 100:{//user dies
            double hp_percent = cpu_active->getCurrentHP() * 100.0 / cpu_active->getMaxHP();
            if(hp_percent > 80)
                total_utility *= -1;
            else if(hp_percent > 50)
                total_utility *= 0.1;
            else if(hp_percent > 10)
                total_utility *= 0.33;
            else
                total_utility *= 2;
            break;
        }
        case 101:{//heal status
            if(cpu_active->hasPermanentStatus())
                total_utility += 70;
            else
                total_utility -= 200;
            break;
        }
        case 102:{//transform
            total_utility += 65;
            break;
        }
        case 103:{//user dies
            double hp_percent = cpu_active->getCurrentHP() * 100.0 / cpu_active->getMaxHP();
            if(hp_percent > 80)
                total_utility *= 0.3;
            else if(hp_percent > 50)
                total_utility *= 0.7;
            else if(hp_percent > 10)
                total_utility *= 1.1;
            else
                total_utility *= 2;
            break;
        }
        case 104:{// tri attack
            if(!enemy_active->hasPermanentStatus() && 
                (!field->hasFieldEffect(SAFEGUARD,OPPONENT) || cpu_active->getAbility() == INFILTRATOR))
                total_utility += 20;
            break;
        }
        case 105:{//fake out
            if(cpu_active->getTurnsInBattle() < 1)
                total_utility *= 10;
            else
                total_utility *= -1;
            break;
        }
        case 106:{//taunt
            if(!enemy_active->hasVolatileCondition(TAUNTED))
                total_utility+=50;
            else
                total_utility -= 130;
            break;
        }
        case 107:{
            // opponent becomes water type
            if(enemy_active->hasType(WATER))
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 108:{
            //copy target stat changes
            int total_stat_changes_taget = 0;//for base power
            total_stat_changes_taget += enemy_active->getAttackModifier();
            total_stat_changes_taget += enemy_active->getSpecialAttackModifier();
            total_stat_changes_taget += enemy_active->getDefenseModifier();
            total_stat_changes_taget += enemy_active->getSpecialDefenseModifier();
            total_stat_changes_taget += enemy_active->getSpeedModifier();
            int total_stat_changes_cpu = 0;//for base power
            total_stat_changes_cpu += cpu_active->getAttackModifier();
            total_stat_changes_cpu += cpu_active->getSpecialAttackModifier();
            total_stat_changes_cpu += cpu_active->getDefenseModifier();
            total_stat_changes_cpu += cpu_active->getSpecialDefenseModifier();
            total_stat_changes_cpu += cpu_active->getSpeedModifier();
            int diff = total_stat_changes_taget - total_stat_changes_cpu;
            total_utility += 20*diff;
            break;
        }
        case 109:{
            // +2 spdef user
            int spdef_modifier = cpu_active->getSpecialDefenseModifier();
            if(spdef_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (40 - spdef_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 110:{
            //wonder room
            if(field->hasFullFieldEffect(WONDER_ROOM))
                total_utility -= 47;
            else
                total_utility += 30;
            break;
        }
        case 111:{//power depends on opp weight
            unsigned int weight = cpu_active->getWeight();
            if(weight > 200)
                total_utility *= 10;
            else if(weight > 100)
                total_utility *= 5;
            else
                total_utility *= 2;
            break;
        }   
        case 112:{
            //damage = user level
            unsigned int level = cpu_active->getLevel();
            total_utility = level;
            break;
        }
        case 113:{
            // confuse opponent and +2 ATT opponent
            if(!enemy_active->hasVolatileCondition(CONFUSION))
                total_utility += 10 * status_mult;
            else
                total_utility -= 10 * status_mult;
            if(enemy_active->getAttackModifier() == 6)
                total_utility += 10*status_mult;
            else
                total_utility -= (40 + enemy_active->getAttackModifier() * 100 / 6)*0.21*status_mult;
            break;
        }
        case 115:{
            //- 1 def spdef user
            int def_modifier = cpu_active->getDefenseModifier();
            int spdef_modifier = cpu_active->getSpecialDefenseModifier();
            total_utility -= (10 - def_modifier * 100 / 6 - spdef_modifier * 100 / 6) * status_mult * 0.1;
            break;
        }
        case 116:{
            // power doubles if last move fails
            if(cpu_active->lastAttackFailed())
                total_utility *= 2;
            break;
        }
        case 117:{
            // damage equal current HP, then dies
            total_utility = cpu_active->getCurrentHP();
            total_utility -= 200;
            break;
        }
        case 118:{
            //power is higher if user was hit
            auto hits_taken = cpu_active->numberOfHitsTaken();
            if(hits_taken > 0)
                total_utility *= min((hits_taken + 1),6);
            break;
        }
        case 120:{
            // power depends on current % oh hp
            unsigned int current_hp = cpu_active->getCurrentHP();
            unsigned int max_hp = cpu_active->getMaxHP();
            unsigned int hp_percent = current_hp * 100 / max_hp;
            if(hp_percent > 80)
                total_utility *= 0.5;
            else if(hp_percent > 50)
                total_utility *= 1.1;
            else if(hp_percent > 10)
                total_utility *= 2;
            else
                total_utility *= 3;
            break;
        }
        case 121:{
            //belly drum
            if((cpu_active->getCurrentHP() < cpu_active->getMaxHP() * 0.5)||
                (cpu_active->getAttackModifier() >= 3))
                total_utility -= 150;
            else
                total_utility += 70;
            break;
        }
        case 122:{
            //-1 speed opponent
            int spd_modifier = enemy_active->getSpeedModifier();
            if(spd_modifier == -6)
                total_utility-= 5 * status_mult;
            else 
                total_utility += (10 + spd_modifier * 100 / 6)*0.25*status_mult;
            break;
        }
        case 124:{
            // user switches out
            total_utility += 30;
            break;
        }
        case 125:{
            //reflect
            if(field->hasFieldEffect(REFLECT,OPPONENT))
                total_utility -= 120;
            else
                total_utility += 160;
            break;
        }
        case 126:{
            //heal 50% HP
            unsigned int health_percent = cpu_active->getCurrentHP() * 100 / cpu_active->getMaxHP();
            if(health_percent < 25)
                total_utility *= 3;
            else if(health_percent < 50)
                total_utility *= 1.5;
            else
                total_utility -= 50;
            break;
        }
        case 127:{
            // copy target ability
            if(cpu_active->getAbility() == enemy_active->getAbility())
                total_utility -= 100;
            else
                total_utility += 50;
        }
        case 129:{
            //+1 def spdef user
            int def_modifier = cpu_active->getDefenseModifier();
            int spdef_modifier = cpu_active->getSpecialDefenseModifier();
            if(def_modifier == 6 || spdef_modifier == 6)
                total_utility -= 10 * status_mult;
            else
                total_utility = (40 - def_modifier * 100 / 6 - spdef_modifier * 100 / 6) * status_mult * 0.35;
            break;
        }
        case 130:{
            //future sight
            total_utility /= 3.0;
            break;
        }
        case 132:{
            //+1 att def user
            int att_modifier = cpu_active->getAttackModifier();
            int def_modifier = cpu_active->getDefenseModifier();
            if(att_modifier == 6 || def_modifier == 6)
                total_utility -= 10 * status_mult;
            else
                total_utility += (40 - att_modifier * 100 / 6 - def_modifier * 100 / 6) * status_mult * 0.35;
            break;
        }
        case 133:{
            //-2 spatt user
            int spatt_modifier = cpu_active->getSpecialAttackModifier();
            total_utility -= (40 + spatt_modifier * 100 / 6)*0.1*status_mult;
            break;
        }
        case 134:{
            // power doubles if target has perm status
            if(enemy_active->hasPermanentStatus())
                total_utility *= 2;
            break;
        }
        case 137:{
            //user becomes same type as target
            std::set<Type> enemy_types = enemy_active->getTypes();
            std::set<Type> cpu_types = cpu_active->getTypes();
            bool has_effective_type = false;
            for(auto enemy_type: enemy_types){
                float effectiveness = getTypeEffectiveness(enemy_type,cpu_types,enemy_active->isTouchingGround(),false,false);
                if(effectiveness>1.1)
                    has_effective_type = true;
            }
            if(has_effective_type)
                total_utility += 20;
            else
                total_utility -= 20;
            break;
        }
        case 138:{
            //smack target on ground
            if(enemy_active->isTouchingGround())
                total_utility += 20;
            break;
        }
        case 140:{
            //power depends on weight %
            double weight_percentage = (double)enemy_active->getWeight() / (double)cpu_active->getWeight() * 100;
            if(weight_percentage > 50){
                total_utility *= 4;
            }else if(weight_percentage > 33.34){
                total_utility *= 6;
            }else if(weight_percentage > 25){
                total_utility *= 8;
            }else if(weight_percentage > 20){
                total_utility *= 10;
            }else{
                total_utility *= 12;
            }
            break;
        }
        case 142:{
            // +1 spd user
            int spd_modifier = cpu_active->getSpeedModifier();
            if(spd_modifier == 6)
                total_utility -= 10*status_mult;
            else
                total_utility += (40 - spd_modifier * 100 / 6)*0.7*status_mult;
            break;
        }
        case 144:{
            //curse
            if(cpu_active->hasType(GHOST)){
                //curse opponent
                unsigned int current_hp = enemy_active->getCurrentHP();
                unsigned int max_hp = enemy_active->getMaxHP();
                unsigned int hp_percent = current_hp * 100 / max_hp;
                if(hp_percent < 50)
                    total_utility -= 200;
                if(enemy_active->hasVolatileCondition(CURSED))
                    total_utility -= 100;
                else
                    total_utility += 50;
            }else{
                // -1 speed, +1 ATT +a DEF user
                int spd_modifier = cpu_active->getSpeedModifier();
                int att_modifier = cpu_active->getAttackModifier();
                int def_modifier = cpu_active->getDefenseModifier();
                if(att_modifier == 6 || def_modifier == 6)
                    total_utility -= 10*status_mult;
                else
                    total_utility += (40 + spd_modifier * 100 / 6 - att_modifier * 100 / 6 - def_modifier*100/6 ) * status_mult * 0.35;
            }
        }
        case 145:{
            // heal target
            total_utility = -120;
        }
        case 146:{
            Gender cpu_gender = cpu_active->getGender();
            Gender enemy_gender = enemy_active->getGender();
            if(areMaleAndFemale(cpu_gender,enemy_gender) && !enemy_active->hasVolatileCondition(INFATUATION)){
                total_utility += 80;
            }else{
                total_utility -= 100;
            }
            break;
        }
        case 147:{
            //lock on target
            if(!cpu_active->hasLockOn(enemy_active->getMonster())){
                total_utility += 50;
            }else{
                total_utility -= 100;
            }
            break;
        }
        case 148:{
            //set up electric terrain
            if(field->getTerrain()==ELECTRIC_FIELD)
                total_utility -= 170;
            else
                total_utility += 120;
            break;
        }
        case 149:{
            // magnet rise
            if(cpu_active->hasVolatileCondition(MAGNET_RISE))
                total_utility -= 100;
            else
                total_utility += 50;
            break;
        }
        case 150:{
            //attack never kills, avoid it if possible
            total_utility *= 0.1;
            break;
        }
        case 151:{
            // avoid using this when opponent is asleep
            if(enemy_active->isAsleep())
                total_utility -= 20;
            break;
        }
        default: break;
    }
    return total_utility;
}

unsigned int CPUAI::chooseRandomAttack(Battler* active_monster)const{
    auto available_attacks = active_monster->getAttacks();
    std::vector<unsigned int> choices;
    for(auto it = available_attacks.begin(); it != available_attacks.end(); it++){
        if(it->second == 0){continue;}
        choices.push_back(it->first);
    }
    if(choices.empty()){
        return STRUGGLE_ID;
    }
    unsigned int random_choice = RNG::getRandomInteger(0,choices.size()-1);
    return choices[random_choice];
}

BattleAction CPUAI::chooseAction(Battler* active_monster,MonsterTeam* monster_team, Battler* enemy_active,Field* field)const{
    BattleAction forced_action = forcedAction(OPPONENT, active_monster,field);
    if(forced_action.getActionType() != ATTACK){return forced_action;}
    if(active_monster->hasVolatileCondition(ENCORE)){
        // force use of encored move
        unsigned int attack_id = active_monster->getLastAttackUsed();
        Attack *attack = Attack::getAttack(attack_id);
        active_monster->decrementVolatileCondition(ENCORE);
        return BattleAction(
            OPPONENT,
            ATTACK,
            attack_id,
            attack->getPriorityLevel(), 
            active_monster->getModifiedSpeed(), 
            0);   
    }
    if(skill==0){// unskilled opponenets never switch and always attack randomly
        return BattleAction(
            OPPONENT,
            ATTACK,
            chooseRandomAttack(active_monster), 
            0, 
            active_monster->getModifiedSpeed(), 
            0);
    }
    Choice * attack_choice = getBestAttackChoice(active_monster,enemy_active,field);
    Choice * switch_choice = getBestSwitchChoice(active_monster,monster_team,enemy_active,field);
    BattleAction struggle_action(
        OPPONENT,
        ATTACK,
        STRUGGLE_ID, 
        0, 
        active_monster->getModifiedSpeed(), 
        0);
    if(attack_choice == nullptr && switch_choice == nullptr){
        return struggle_action;
    }else if(attack_choice == nullptr){
        unsigned int switch_id = switch_choice->choice_id;
        delete switch_choice;
        if(!active_monster->canSwitchOut(enemy_active))
            return struggle_action;
        return BattleAction(
            OPPONENT,
            SWITCH,
            0, 
            0, 
            active_monster->getModifiedSpeed(), 
            switch_id);
    }else if(switch_choice == nullptr){
        unsigned int attack_id = attack_choice->choice_id;
        delete attack_choice;
        Attack* attack = Attack::getAttack(attack_id);
        return BattleAction(
            OPPONENT,
            ATTACK,
            attack_id, 
            attack->getPriorityLevel(), 
            active_monster->getModifiedSpeed(), 
            0);
    }else if(attack_choice->utility >= switch_choice->utility || !active_monster->canSwitchOut(enemy_active)){
        unsigned int attack_id = attack_choice->choice_id;
        delete attack_choice;
        delete switch_choice;
        Attack* attack = Attack::getAttack(attack_id);
        return BattleAction(
            OPPONENT,
            ATTACK,
            attack_id, 
            attack->getPriorityLevel(), 
            active_monster->getModifiedSpeed(), 
            0);
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
            switch_id);
    }
}

Choice* CPUAI::getBestAttackChoice(Battler* active_user,Battler*active_target,Field*field)const{
    auto choices = active_user->getAttacks();
    if(choices.empty()){
        return nullptr;
    }
    unsigned int best_choice_id = 0;
    int best_utility = -100000;
    for(auto choice: choices){
        unsigned int attack_id = choice.first;
        unsigned int curr_pp = choice.second;
        if(choice.second == 0){continue;}
        if(attack_id==0){continue;}
        int utility = computeAttackUtility(attack_id,active_user,active_target,field);
        if(utility > best_utility){
            best_choice_id = attack_id;
            best_utility = utility;
        }
    }
    if(best_utility == -100000){
        return nullptr;
    }
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
    for(int i=1; i<choices.size();i++){
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
            int attack_utility = computeAttackUtility(attack_id,active_user,active_target,field);
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