#include "attack.h"

// AttackType ------------------------------------------------------------------------------------------------------

AttackType stringToAttackType(std::string type) {
    // default to physical
    if(type == "PHYSICAL")
        return PHYSICAL;
    if(type == "SPECIAL")
        return SPECIAL;
    if(type == "STATUS")
        return STATUS;
    return PHYSICAL;
}

std::string attackTypeToString(AttackType type){
    // default to physical
    switch(type){
        case PHYSICAL:
            return "PHYSICAL";
        case SPECIAL:
            return "SPECIAL";
        case STATUS:
            return "STATUS";
        default:
            return "PHYSICAL";
    }
}

// Attacktarget ------------------------------------------------------------------------------------------------------

std::string attackTargetToString(AttackTarget type){
    switch(type){
        case TARGET_SELF:
            return "SELF";
        case TARGET_OPPONENT:
            return "OPPONENT";
        default:
            return "OPPONENT";
    }
}

AttackTarget stringToAttackTarget(std::string type) {
    // default to opponent
    if(type == "SELF")
        return TARGET_SELF;
    if(type == "OPPONENT")
        return TARGET_OPPONENT;
    return TARGET_OPPONENT;
}

// Attack ----------------------------------------------------------------------------------------------------------

Attack::Attack() {
    
}

Attack::~Attack() {
    #ifdef DEBUG
    std::cout << "Deleting attack " << id << std::endl;
    #endif
}

Attack::Attack(unsigned int index, std::map<std::string,std::string> data) {
    id = index;
    if (data.find("name") == data.end()){
        std::cerr << "Error: Attack " << id << " has no name!" << std::endl;
        exit(DATA_ERROR);
    }else
        name = data["name"];
    if(data.find("description") == data.end()){
        std::cerr << "Error: Attack " << id << " has no description!" << std::endl;
        exit(DATA_ERROR);
    }else
        description = data["description"];
    if(data.find("type") == data.end()){
        std::cerr << "Error: Attack " << id << " has no type!" << std::endl;
        exit(DATA_ERROR);
    }else    
        type = stringToType(data["type"]);
    if (data.find("power") == data.end())
        power = 0;
    else
        power = stringToInteger(data["power"]);
    if (data.find("accuracy") == data.end())
        accuracy = 100;
    else
        accuracy = stringToInteger(data["accuracy"]);
    if (data.find("category") == data.end()){
        std::cerr << "Error: Attack " << id << " has no category!" << std::endl;
        exit(DATA_ERROR);
    }else
        attack_type = stringToAttackType(data["category"]);
    if (data.find("effect_id") == data.end())
        effect_id = 0;
    else
        effect_id = stringToInteger(data["effect_id"]);
    if (data.find("effect_chance") == data.end()){
        if(effect_id == 0)
            effect_chance = 0;
        else
            effect_chance = ALWAYS_HITS;
    }else{
        if(effect_id == 0)
            effect_chance = 0;
        else
            effect_chance = stringToInteger(data["effect_chance"]);
    }
    if (data.find("priority_level") == data.end())
        priority_level = 0;
    else
        priority_level = stringToInteger(data["priority_level"]);
    if (data.find("target") == data.end())
        target = TARGET_OPPONENT;
    else
        target = stringToAttackTarget(data["target"]);
    if (data.find("effect_target") == data.end())
        effect_target = TARGET_OPPONENT;
    else
        effect_target = stringToAttackTarget(data["effect_target"]);
    if (data.find("max_pp") == data.end())
        max_pp = 0;
    else
        max_pp = stringToInteger(data["max_pp"]);
    is_contact = (data.find("contact") != data.end());
    // cannot_be_metronomed = (data.find("no_metronome") != data.end());
    can_be_metronomed = (data.find("no_metronome") == data.end());
    can_be_copycat = (data.find("no_copycat") == data.end());
    is_sound_based = (data.find("sound") != data.end());
    is_punching = (data.find("punching") != data.end());
    is_powder = (data.find("powder") != data.end());
    is_pulse = (data.find("pulse") != data.end());
    is_reflectable = (data.find("reflectable") != data.end());
    is_sketchable = (data.find("no_sketch") == data.end());
    is_wind = (data.find("wind") != data.end());
    is_biting = (data.find("biting") != data.end());
    is_slicing = (data.find("slicing") != data.end());
    if(target==TARGET_SELF){
        accuracy = ALWAYS_HITS;
        effect_target = TARGET_SELF;
    }
}

unsigned int Attack::getId()const {
    return id;
}

bool Attack::isSlicing()const {
    return is_slicing;
}

Type Attack::getType()const {
    return type;
}

Type Attack::getType(Battler* user, Field* field)const{
    Type attack_type = getType();
    switch(getEffectId()){
        case 238:{
            //hidden power
            attack_type = user->getHiddenPowerType();
            break;
        }
        case 215:{
            //raging bull changes type depending on the user's form
            switch(user->getMonster()->getFormId()){
                case 46:{
                    attack_type=FIGHTING;
                    break;
                }
                case 47:{
                    attack_type=WATER;
                    break;
                }
                case 48:{
                    attack_type=FIRE;
                    break;
                }
                default:{
                    attack_type=NORMAL;
                    break;
                }
            }
            break;
        }
        case 254:{
            //weather ball
            switch(field->getWeather()){
                case RAIN:{
                    if(user->hasHeldItem(UTILITY_UMBRELLA))
                        attack_type = NORMAL;
                    else
                        attack_type = WATER;
                    break;
                }
                case SUN:{
                    if(user->hasHeldItem(UTILITY_UMBRELLA))
                        attack_type = NORMAL;
                    else
                        attack_type = FIRE;
                    break;
                }
                case SANDSTORM:{
                    attack_type = ROCK;
                    break;
                }
                case HAIL:{
                    attack_type = ICE;
                    break;
                }
                default:{
                    attack_type = NORMAL;
                    break;
                }
            }
            break;
        }
        case 294:{
            //type depends on plate
            switch(user->getHeldItem()){
                case FIST_PLATE:{
                    attack_type = FIGHTING;
                    break;
                }
                case SKY_PLATE:{
                    attack_type = FLYING;
                    break;
                }
                case TOXIC_PLATE:{
                    attack_type = POISON;
                    break;
                }
                case EARTH_PLATE:{
                    attack_type = GROUND;
                    break;
                }
                case STONE_PLATE:{
                    attack_type = ROCK;
                    break;
                }
                case INSECT_PLATE:{
                    attack_type = BUG;
                    break;
                }
                case SPOOKY_PLATE:{
                    attack_type = GHOST;
                    break;
                }
                case DRACO_PLATE:{
                    attack_type = DRAGON;
                    break;
                }
                case DREAD_PLATE:{
                    attack_type = DARK;
                    break;
                }
                case PIXIE_PLATE:{
                    attack_type = FAIRY;
                    break;
                }
                case FLAME_PLATE:{
                    attack_type = FIRE;
                    break;
                }
                case SPLASH_PLATE:{
                    attack_type = WATER;
                    break;
                }
                case ZAP_PLATE:{
                    attack_type = ELECTRIC;
                    break;
                }
                case MEADOW_PLATE:{
                    attack_type = GRASS;
                    break;
                }
                case MIND_PLATE:{
                    attack_type = PSYCHIC;
                    break;
                }
                case ICICLE_PLATE:{
                    attack_type = ICE;
                    break;
                }
                case IRON_PLATE:{
                    attack_type = STEEL;
                    break;
                }
                default:{
                    attack_type = NORMAL;
                    break;
                }
            }
            break;
        }
        case 310:{
            //techno blast type depends on held drive
            switch(user->getHeldItem()){
                case SHOCK_DRIVE:{
                    attack_type = ELECTRIC;
                    break;
                }
                case BURN_DRIVE:{
                    attack_type = FIRE;
                    break;
                }
                case CHILL_DRIVE:{
                    attack_type = ICE;
                    break;
                }
                case DOUSE_DRIVE:{
                    attack_type = WATER;
                    break;
                }
                default:{
                    attack_type = NORMAL;
                    break;
                }
            }
        }
    }
    if(user->hasAbility(GALVANIZE) && attack_type==NORMAL)
        attack_type = ELECTRIC;
    if(user->hasAbility(REFRIGERATE) && attack_type==NORMAL)
        attack_type = ICE;
    if(user->hasAbility(AERILATE) && attack_type==NORMAL)
        attack_type = FLYING;
    if(user->hasAbility(PIXILATE) && attack_type==NORMAL)
        attack_type = FAIRY;
    if(user->hasAbility(NORMALIZE))
        return NORMAL;
    return attack_type;
}

unsigned int Attack::getPower()const {
    return power;
}

unsigned int Attack::getAccuracy()const {
    return accuracy;
}

AttackType Attack::getCategory()const {
    return attack_type;
}

unsigned int Attack::getEffectId()const {
    return effect_id;
}

unsigned int Attack::getEffectChance()const {
    return effect_chance;
}

int Attack::getPriorityLevel()const {
    return priority_level;
}

std::string Attack::getName()const {
    return name;
}

std::string Attack::getDescription()const {
    return description;
}

unsigned int Attack::getMaxPP()const {
    return max_pp;
}

AttackTarget Attack::getTarget()const {
    return target;
}

AttackTarget Attack::getEffectTarget()const {
    return effect_target;
}

bool Attack::makesContact()const {
    return is_contact;
}

bool Attack::isPulse()const {
    return is_pulse;
}

bool Attack::isReflectable()const {
    return is_reflectable;
}

bool Attack::isSoundBased()const {
    return is_sound_based;
}

bool Attack::canBeSketched()const {
    return is_sketchable;
}

bool Attack::isBiting()const {
    return is_biting;
}

bool Attack::isPunching()const {
    return is_punching;
}

bool Attack::isPowder()const {
    return is_powder;
}

void Attack::loadAttacks(){
    static_attacks[0] = nullptr;
    std::ifstream file(ATTACK_FILE);
    if(!file.is_open()){
        std::cerr << "Error opening file " << ATTACK_FILE << std::endl;
        exit(FILE_ERROR);
    }
    std::string line;
    unsigned int current_attack_id = 1;
    std::map<std::string,std::string> parsed_data;
    while(std::getline(file,line)){
        if(line[0] == '%')
            continue;
        if(line[0] == '#'){
            if(parsed_data.size() > 0){
                Attack* attack = new Attack(current_attack_id,parsed_data);
                static_attacks.insert({current_attack_id,attack});
                current_attack_id++;
                parsed_data.clear();
            }
        }else if(line.rfind("NAME:",0)==0){
            parsed_data["name"] = line.substr(5);
        }else if(line.rfind("TYPE:",0)==0){
            parsed_data["type"] = line.substr(5);
        }else if(line.rfind("POWER:",0)==0){        
            parsed_data["power"] = line.substr(6);
        }else if(line.rfind("ACCURACY:",0)==0){
            parsed_data["accuracy"] = line.substr(9);
        }else if(line.rfind("CATEGORY:",0)==0){
            parsed_data["category"] = line.substr(9);
        }else if(line.rfind("EFFECT:",0)==0){
            parsed_data["effect_id"] = line.substr(7);
        }else if(line.rfind("EFFECTCHANCE:",0)==0){
            parsed_data["effect_chance"] = line.substr(13);
        }else if(line.rfind("PRIORITY:",0)==0){
            parsed_data["priority_level"] = line.substr(9);
        }else if(line.rfind("DESCRIPTION:",0)==0){
            parsed_data["description"] = line.substr(12);
        }else if(line.rfind("PP:",0)==0){
            parsed_data["max_pp"] = line.substr(3);
        }else if(line.rfind("TARGET:",0)==0){
            parsed_data["target"] = line.substr(7);
        }else if(line.rfind("EFFECTTARGET:",0)==0){
            parsed_data["effect_target"] = line.substr(13);
        }else if(line.rfind("CONTACT",0)==0){
            parsed_data["contact"] = "true";
        }else if(line.rfind("PUNCHING",0)==0){
            parsed_data["punching"] = "true";
        }else if(line.rfind("SOUND",0)==0){
            parsed_data["sound"] = "true";
        }else if(line.rfind("NO_METRONOME",0)==0){
            parsed_data["no_metronome"] = "true";
        }else if(line.rfind("NO_COPYCAT",0)==0){
            parsed_data["no_copycat"] = "true";
        }else if(line.rfind("NO_SKETCH",0)==0){
            parsed_data["no_sketch"] = "true";
        }else if(line.rfind("POWDER",0)==0){
            parsed_data["powder"] = "true";
        }else if(line.rfind("PULSE",0)==0){
            parsed_data["pulse"] = "true";
        }else if(line.rfind("WIND",0)==0){
            parsed_data["wind"] = "true";
        }else if(line.rfind("SLICING",0)==0){
            parsed_data["slicing"] = "true";
        }else if(line.rfind("REFLECTABLE",0)==0){
            parsed_data["reflectable"] = "true";
        }else if(line.rfind("BITING",0)==0){
            parsed_data["biting"] = "true";
        }else{
            std::cout<<"Error: Unknown line in attack file: " << line << std::endl;
        }
    }
    if(parsed_data.size() > 0){
        Attack* attack = new Attack(current_attack_id,parsed_data);
        static_attacks.insert({current_attack_id,attack});
    }
    file.close();
}

bool Attack::canBeCopycat()const{
    return can_be_copycat;
}

bool Attack::isWind()const{
    return is_wind;
}

Attack* Attack::getRandomMetronomeAttack() {
    int count = 0;
    while(true){
        if(count++ > 100)
            return static_attacks[1];//tackle is chosen if after 100 tries no attack is found
        unsigned int random_index = RNG::getRandomInteger(1, static_attacks.size() - 1);
        auto iter = static_attacks.find(random_index);
        if(!iter->second->canBeMetronomed())
            continue;
        return iter->second;
    }
}

bool Attack::canBeMetronomed()const{
    return can_be_metronomed;
}

Attack* Attack::getAttack(unsigned int attack_id) {
    auto iter = static_attacks.find(attack_id);
    if (iter == static_attacks.end())
        return nullptr;
    return iter->second;
}

void Attack::printSummary()const{
    std::cout<<"# "<<getId()<<" "<<getName()<<std::endl;
    std::cout<<"Category:   "<<attackTypeToString(getCategory())<<std::endl;
    std::cout<<"Type:       "<<typeToString(getType())<<std::endl;
    std::cout<<"MaxPP:      "<<getMaxPP()<<std::endl;
    std::cout<<"Desc.:      "<<getDescription()<<std::endl;
    std::cout<<"Target:     "<<attackTargetToString(getTarget())<<std::endl;
    std::cout<<"Priority:   "<<getPriorityLevel()<<std::endl;
    if(effect_id!=0){
        std::cout<<"Effect:     "<<getEffectId()<<std::endl;
        std::cout<<"Eff. Target:"<<attackTargetToString(getTarget())<<std::endl;
    }
    if(is_contact)
        std::cout<<"CONTACT   ";
    if(can_be_metronomed)
        std::cout<<"METRONOME   ";
    if(is_pulse)
        std::cout<<"PULSE   ";
    if(is_punching)
        std::cout<<"PUNCHING   ";
    if(is_powder)
        std::cout<<"POWDER   ";
    if(is_sound_based)
        std::cout<<"SOUND_BASED   ";
    if(is_reflectable)
        std::cout<<"REFLECTABLE   ";
    std::cout<<std::endl;
    std::cout.flush();
}

void Attack::printSummary(unsigned int curr_pp){
    std::cout << getName() << " -> " << getDescription() << std::endl;
    std::cout << "Type: " << typeToString(getType());
    unsigned int power = getPower();
    if(power != 0)
        std::cout << " Power: " << power;
    else
        std::cout << " Power: -";
    unsigned int accuracy = getAccuracy();
    if(accuracy != ALWAYS_HITS)
        std::cout << " Accuracy: " << accuracy;
    else
        std::cout << " Accuracy: -";
    std::cout << " PP: " <<curr_pp<<"/"<< getMaxPP()
    // << " Priority: " << getPriorityLevel() 
    << " Category: "<< attackTypeToString(getCategory())<<std::endl;
}

void Attack::printAllSummaries(){
    for(auto it = static_attacks.begin(); it!=static_attacks.end();it++){
        if(it->second == nullptr)
            continue;
        it->second->printSummary();
    }
}