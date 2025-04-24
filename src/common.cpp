#include "common.h"

// Permanent Status Condition ---------------------------------------------------------------------------------------------

PermanentStatusCondition stringToPermanentStatusCondition(std::string status){
    if(status == "NO_PERMANENT_CONDITION")
        return NO_PERMANENT_CONDITION;
    if(status == "BURN")
        return BURN;
    if(status == "FREEZE")
        return FREEZE;
    if(status == "PARALYSIS")
        return PARALYSIS;
    if(status == "POISON")
        return POISONED;
    if(status == "BAD_POISON")
        return BAD_POISON;
    if(status == "SLEEP_1")
        return SLEEP_1;
    if(status == "SLEEP_2")
        return SLEEP_2;
    if(status == "SLEEP_3")
        return SLEEP_3;
    if(status == "SLEEP_4")
        return SLEEP_4;
    return KO;
}

std::string permanentStatusConditionToString(PermanentStatusCondition status){
    switch(status){
        case NO_PERMANENT_CONDITION:
            return "";
        case BURN:
            return "BRN";
        case FREEZE:
            return "FRZ";
        case PARALYSIS:
            return "PAR";
        case POISONED:
            return "PSN";
        case BAD_POISON:
            return "BAD_PSN";
        case SLEEP_1:
        case SLEEP_2:
        case SLEEP_3:
        case SLEEP_4:
            return "SLP";
        default: 
            return "KO";
    }
}

PermanentStatusCondition decrementSleep(PermanentStatusCondition status){
    switch(status){
        case SLEEP_4:
            return SLEEP_3;
        case SLEEP_3:
            return SLEEP_2;
        case SLEEP_2:
            return SLEEP_1;
        case SLEEP_1:
            return NO_PERMANENT_CONDITION;
        default:
            return status;
    }
}

// Volatile Status Condition ---------------------------------------------------------------------------------------------

// Gender ------------------------------------------------------------------------------------------------------------

Gender stringToGender(std::string gender){
    if(gender == "MALE")
        return MALE;
    if(gender == "FEMALE")
        return FEMALE;
    return GENDERLESS;
}
std::string genderToString(Gender gender){
    switch (gender){
    case MALE:
        return "MALE";
    case FEMALE:
        return "FEMALE";
    default:
        return "GENDERLESS";
    }
}
bool areMaleAndFemale(Gender g1,Gender g2){
    if(g1==MALE && g2==FEMALE)
        return true;
    if(g1==FEMALE && g2==MALE)
        return true;
    return false;
}

// Gender Rule -------------------------------------------------------------------------------------------------------

GenderRule stringToGenderRule(std::string gender_rule){
    if(gender_rule == "ALWAYS_MALE")
        return ALWAYS_MALE;
    if(gender_rule == "ALWAYS_FEMALE")
        return ALWAYS_FEMALE;
    if(gender_rule == "HALF_MALE_HALF_FEMALE")
        return HALF_MALE_HALF_FEMALE;
    if(gender_rule == "MALE_ONE_EIGHT")
        return MALE_ONE_EIGHT;
    if(gender_rule == "FEMALE_ONE_EIGHT")
        return FEMALE_ONE_EIGHT;
    if(gender_rule == "MALE_ONE_FOURTH")
        return MALE_ONE_FOURTH;
    if(gender_rule == "FEMALE_ONE_FOURTH")
        return FEMALE_ONE_FOURTH;
    return NO_GENDER;
}
std::string genderRuleToString(GenderRule gender_rule){
    switch (gender_rule){
    case ALWAYS_MALE:
        return "ALWAYS_MALE";
    case ALWAYS_FEMALE:
        return "ALWAYS_FEMALE";
    case MALE_ONE_EIGHT:
        return "MALE_ONE_EIGHT";
    case FEMALE_ONE_EIGHT:
        return "FEMALE_ONE_EIGHT";
    case HALF_MALE_HALF_FEMALE:
        return "HALF_MALE_HALF_FEMALE";
    case MALE_ONE_FOURTH:
        return "MALE_ONE_FOURTH";
    case FEMALE_ONE_FOURTH:
        return "FEMALE_ONE_FOURTH";
    default:
        return "NO_GENDER";
    }
}

// Nature ------------------------------------------------------------------------------------------------------------

Nature stringToNature(std::string nature){
    // default to hardy
    if(nature == "HARDY")
        return HARDY;
    if(nature == "LONELY")
        return LONELY;
    if(nature == "BRAVE")
        return BRAVE;
    if(nature == "ADAMANT")
        return ADAMANT;
    if(nature == "NAUGHTY")
        return NAUGHTY;
    if(nature == "BOLD")
        return BOLD;
    if(nature == "DOCILE")
        return DOCILE;
    if(nature == "RELAXED")
        return RELAXED;
    if(nature == "IMPISH")
        return IMPISH;
    if(nature == "LAX")
        return LAX;
    if(nature == "TIMID")
        return TIMID;
    if(nature == "HASTY")
        return HASTY;
    if(nature == "SERIOUS")
        return SERIOUS;
    if(nature == "JOLLY")
        return JOLLY;
    if(nature == "NAIVE")
        return NAIVE;
    if(nature == "MODEST")
        return MODEST;
    if(nature == "MILD")
        return MILD;
    if(nature == "QUIET")
        return QUIET;
    if(nature == "BASHFUL")
        return BASHFUL;
    if(nature == "RASH")
        return RASH;
    if(nature == "CALM")
        return CALM;
    if(nature == "GENTLE")
        return GENTLE;
    if(nature == "SASSY")
        return SASSY;
    if(nature == "CAREFUL")
        return CAREFUL;
    if(nature == "QUIRKY")
        return QUIRKY;
    return HARDY;
}

std::string natureToString(Nature nature){
    // default to hardy
    switch(nature){
        case HARDY:
            return "HARDY";
        case LONELY:
            return "LONELY";
        case BRAVE:
            return "BRAVE";
        case ADAMANT:
            return "ADAMANT";
        case NAUGHTY:
            return "NAUGHTY";
        case BOLD:
            return "BOLD";
        case DOCILE:
            return "DOCILE";
        case RELAXED:
            return "RELAXED";
        case IMPISH:
            return "IMPISH";
        case LAX:
            return "LAX";
        case TIMID:
            return "TIMID";
        case HASTY:
            return "HASTY";
        case SERIOUS:
            return "SERIOUS";
        case JOLLY:
            return "JOLLY";
        case NAIVE:
            return "NAIVE";
        case MODEST:
            return "MODEST";
        case MILD:
            return "MILD";
        case QUIET:
            return "QUIET";
        case BASHFUL:
            return "BASHFUL";
        case RASH:
            return "RASH";
        case CALM:
            return "CALM";
        case GENTLE:
            return "GENTLE";
        case SASSY:
            return "SASSY";
        case CAREFUL:
            return "CAREFUL";
        case QUIRKY:
            return "QUIRKY";
        default:
            return "HARDY";
    }
}

Nature randomNature() {
    return static_cast<Nature>(RNG::getRandomInteger(1, 25));
}

// Type ------------------------------------------------------------------------------------------------------------

Type stringToType(std::string type) {
    if(type == "NORMAL")
        return NORMAL;
    if(type == "FIRE")
        return FIRE;
    if(type == "WATER")
        return WATER;
    if(type == "ELECTRIC")
        return ELECTRIC;
    if(type == "GRASS")
        return GRASS;
    if(type == "ICE")
        return ICE;
    if(type == "FIGHTING")
        return FIGHTING;
    if(type == "POISON")
        return POISON;
    if(type == "GROUND")
        return GROUND;
    if(type == "FLYING")
        return FLYING;
    if(type == "PSYCHIC")
        return PSYCHIC;
    if(type == "BUG")
        return BUG;
    if(type == "ROCK")
        return ROCK;
    if(type == "GHOST")
        return GHOST;
    if(type == "DRAGON")
        return DRAGON;
    if(type == "DARK")
        return DARK;
    if(type == "STEEL")
        return STEEL;
    if(type == "FAIRY")
        return FAIRY;
    return NO_TYPE;
}

std::string typeToString(Type type){
    switch(type){
        case NORMAL:
            return "NORMAL";
        case FIRE:
            return "FIRE";
        case WATER:
            return "WATER";
        case ELECTRIC:
            return "ELECTRIC";
        case GRASS:
            return "GRASS";
        case ICE:
            return "ICE";
        case FIGHTING:
            return "FIGHTING";
        case POISON:
            return "POISON";
        case GROUND:
            return "GROUND";
        case FLYING:
            return "FLYING";
        case PSYCHIC:
            return "PSYCHIC";
        case BUG:
            return "BUG";
        case ROCK:
            return "ROCK";
        case GHOST:
            return "GHOST";
        case DRAGON:
            return "DRAGON";
        case DARK:
            return "DARK";
        case STEEL:
            return "STEEL";
        case FAIRY:
            return "FAIRY";
        default:
            return "NONE";
    }
}

std::set<Type> getAllTypesSet(){
    std::set<Type> all_types;
    all_types.insert(NORMAL);
    all_types.insert(FIRE);
    all_types.insert(WATER);
    all_types.insert(ELECTRIC);
    all_types.insert(GRASS);
    all_types.insert(ICE);
    all_types.insert(FIGHTING);
    all_types.insert(POISON);
    all_types.insert(GROUND);
    all_types.insert(FLYING);
    all_types.insert(PSYCHIC);
    all_types.insert(BUG);
    all_types.insert(ROCK);
    all_types.insert(GHOST);
    all_types.insert(DRAGON);
    all_types.insert(DARK);
    all_types.insert(STEEL);
    all_types.insert(FAIRY);
    return all_types;
}

float getTypeEffectiveness(Type attacker, Type defender_1, Type defender_2, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry){
    return getTypeEffectiveness(attacker, defender_1,is_touching_ground,can_hit_ghosts,is_freeze_dry) * 
            getTypeEffectiveness(attacker, defender_2,is_touching_ground,can_hit_ghosts,is_freeze_dry);
}

float getTypeEffectiveness(Type attacker, std::set<Type> defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry){
    float final_result = 1;
    for(auto it = defender.begin();it!=defender.end();it++){
        final_result *= getTypeEffectiveness(attacker,*it,is_touching_ground, can_hit_ghosts,is_freeze_dry);
    }
    return final_result;
}

float getTypeEffectiveness(Type attacker, Type defender, bool is_touching_ground, bool can_hit_ghosts, bool is_freeze_dry){
    // std::cout << "Attacker: " << typeToString(attacker) << ", Defender: " << typeToString(defender) << std::endl;
    if(defender == WATER && is_freeze_dry)
        return 2.0;
    switch(attacker){
        case NORMAL:
            if(defender == GHOST && !can_hit_ghosts)
                return 0.0;
            if((defender == ROCK)||(defender == STEEL))
                return 0.5;
            return 1.0;
        case FIRE:
            if((defender == GRASS)||(defender == BUG)||(defender == ICE)||(defender == STEEL))
                return 2.0;
            if((defender == WATER)||(defender == FIRE)||(defender == ROCK)||(defender == DRAGON))
                return 0.5;
            return 1.0;
        case WATER:
            if((defender == FIRE)||(defender == GROUND)||(defender == ROCK))
                return 2.0;
            if((defender == WATER)||(defender == GRASS)||(defender == DRAGON))
                return 0.5;
            return 1.0;
        case ELECTRIC:
            if((defender == WATER)||(defender == FLYING))
                return 2;
            if((defender == ELECTRIC)||(defender == GRASS)||(defender == DRAGON))
                return 0.5;
            if(defender == GROUND)
                return 0;
            return 1;
        case GRASS:
            if((defender == WATER)||(defender == GROUND)||(defender == ROCK))
                return 2.0;
            if((defender == FIRE)||(defender == GRASS)||(defender == POISON)||(defender == FLYING)||
                (defender == BUG)||(defender == DRAGON)||(defender==STEEL))
                return 0.5;
            return 1.0;
        case ICE:
            if((defender == GRASS)||(defender == GROUND)||(defender == FLYING)||(defender == DRAGON))
                return 2;
            if((defender == FIRE)||(defender == WATER)||(defender == ICE)||(defender == STEEL))
                return 0.5;
            return 1;
        case FIGHTING:
            if((defender == NORMAL)||(defender == ROCK)||(defender == ICE)||(defender == DARK)||
                (defender == STEEL))
                return 2;
            if((defender == FLYING)||(defender == POISON)||(defender == BUG)||(defender == PSYCHIC)||
                (defender == FAIRY))
                return 0.5;
            if(defender == GHOST && !can_hit_ghosts)
                return 0;
            return 1;
        case POISON:
            if((defender == GRASS)||(defender == FAIRY))
                return 2;
            if((defender == POISON)||(defender == ROCK)||(defender == GHOST)||(defender == GROUND))
                return 0.5;
            if(defender == STEEL)
                return 0;
            return 1;
        case GROUND:
            if((defender == FIRE)||(defender == ELECTRIC)||(defender == POISON)||
                (defender == ROCK)||(defender == STEEL))
                return 2;
            if((defender == GRASS)||(defender == BUG))
                return 0.5;
            if(!is_touching_ground)
                return 0;
            return 1;
        case FLYING:
            if((defender == GRASS)||(defender == FIGHTING)||(defender == BUG))
                return 2;
            if((defender == ELECTRIC)||(defender == ROCK)||(defender == STEEL))
                return 0.5;
            return 1;
        case PSYCHIC:
            if((defender == FIGHTING)||(defender == POISON))
                return 2;
            if((defender == PSYCHIC)||(defender == STEEL))
                return 0.5;
            if(defender == DARK)
                return 0;
            return 1;
        case BUG:
            if((defender == GRASS)||(defender == PSYCHIC)||(defender == DARK))
                return 2;
            if((defender == FIRE)||(defender == FIGHTING)||(defender == POISON)||(defender==FAIRY)||
                (defender == FLYING)||(defender == GHOST)||(defender == STEEL))
                return 0.5;
            return 1;
        case ROCK:
            if((defender == FIRE)||(defender == FLYING)||(defender == BUG)||(defender == ICE))
                return 2;
            if((defender == FIGHTING)||(defender == GROUND)||(defender == STEEL))
                return 0.5;
            return 1;
        case GHOST:
            if((defender == GHOST)||(defender == PSYCHIC))
                return 2;
            if(defender == DARK)
                return 0.5;
            if(defender == NORMAL)
                return 0;
            return 1;
        case DRAGON:
            if(defender == DRAGON)
                return 2;
            if(defender == STEEL)
                return 0.5;
            if(defender == FAIRY)
                return 0;
            return 1;
        case DARK:
            if((defender == GHOST)||(defender == PSYCHIC))
                return 2;
            if((defender == FAIRY)||(defender==DARK)||(defender==FIGHTING))
                return 0.5;
            return 1;
        case STEEL:
            if((defender == ICE)||(defender == ROCK)||(defender == FAIRY))
                return 2;
            if((defender == FIRE)||(defender == WATER)||(defender == ELECTRIC)||
                (defender == STEEL))
                return 0.5;
            return 1;
        case FAIRY:
            if((defender == FIGHTING)||(defender == DARK)||(defender == DRAGON))
                return 2;
            if((defender == FIRE)||(defender == POISON)||(defender == STEEL))
                return 0.5;
            return 1;
        default:
            return 1;
    }
}

// Stats ------------------------------------------------------------------------------------------------------------

Stats::Stats() {
    hp = 0;
    atk = 0;
    spatk = 0;
    def = 0;
    spdef = 0;
    spd = 0;
}

void statsParsingError(){
    std::cerr << "Malformed stats found" <<std::endl;
    exit(DATA_ERROR);
}

Stats::Stats(std::string stats_string){
    // BASESTATS ARE GIVEN IN THE FOLLOWING ORDER: 
    // HP, ATK, DEF, SPATK, SPDEF, SPEED
    std::stringstream ss(stats_string);
    unsigned int token;
    // std::cout<<"Stats string: " << stats_string << std::endl;
    if(!(ss >> token))
        statsParsingError();
    setHp(token);
    if(!(ss >> token))
        statsParsingError();
    setAtk(token);
    if(!(ss >> token))
        statsParsingError();
    setDef(token);
    if(!(ss >> token))
        statsParsingError();
    setSpatk(token);
    if(!(ss >> token))
        statsParsingError();
    setSpdef(token);
    if(!(ss >> token))
        statsParsingError();
    setSpd(token);
}

Stats::~Stats() {
    
}

unsigned int Stats::getHp()const {
    return hp;
}

unsigned int Stats::getAtk()const {
    return atk;
}

unsigned int Stats::getSpatk()const {
    return spatk;
}

unsigned int Stats::getDef()const {
    return def;
}

unsigned int Stats::getSpdef()const {
    return spdef;
}

unsigned int Stats::getSpd()const {
    return spd;
}

void Stats::setHp(unsigned int hp) {
    this->hp = hp;
}

void Stats::setAtk(unsigned int atk) {
    this->atk = atk;
}

void Stats::setSpatk(unsigned int spatk) {
    this->spatk = spatk;
}

void Stats::setDef(unsigned int def) {
    this->def = def;
}

void Stats::setSpdef(unsigned int spdef) {
    this->spdef = spdef;
}

void Stats::setSpd(unsigned int spd) {
    this->spd = spd;
}

void Stats::applyNatureBoosts(Nature nature){
    switch(nature){
        case HARDY:
            break;
        case LONELY:
            atk = atk * 11 / 10;
            def = max(def * 9 / 10,1);
            break;
        case BRAVE:
            atk = atk * 11 / 10;
            spd = max(spd * 9 / 10,1);
            break;
        case ADAMANT:
            atk = atk * 11 / 10;
            spatk = max(spatk * 9 / 10,1);
            break;
        case NAUGHTY:
            atk = atk * 11 / 10;
            spdef = max(spdef * 9 / 10,1);
            break;
        case BOLD:
            def = def * 11 / 10;
            atk = max(atk * 9 / 10,1);
            break;
        case DOCILE:
            break;
        case RELAXED:
            def = def * 11 / 10;
            spd = max(spd * 9 / 10,1);
            break;
        case IMPISH:
            def = def * 11 / 10;
            spatk = max(spatk * 9 / 10,1);
            break;
        case LAX:
            def = def * 11 / 10;
            spdef = max(spdef * 9 / 10,1);
            break;
        case TIMID:
            spd = spd * 11 / 10;
            atk = max(atk * 9 / 10,1);
            break;
        case HASTY:
            spd = spd * 11 / 10;
            def = max(def * 9 / 10,1);
            break;
        case SERIOUS:
            break;
        case JOLLY:
            spd = spd * 11 / 10;
            spatk = max(spatk * 9 / 10,1);
            break;
        case NAIVE:
            spd = spd * 11 / 10;
            spdef = max(spdef * 9 / 10,1);
            break;
        case MODEST:
            spatk = spatk * 11 / 10;
            atk = max(atk * 9 / 10,1);
            break;
        case MILD:
            spatk = spatk * 11 / 10;
            def = max(def * 9 / 10,1);
            break;
        case QUIET:
            spatk = spatk * 11 / 10;
            spd = max(spd * 9 / 10,1);
            break;
        case BASHFUL:
            break;
        case RASH:
            spatk = spatk * 11 / 10;
            spdef = max(spdef * 9 / 10,1);
            break;
        case CALM:
            spdef = spdef * 11 / 10;
            atk = max(atk * 9 / 10,1);
            break;
        case GENTLE:
            spdef = spdef * 11 / 10;
            def = max(def * 9 / 10,1);
            break;
        case SASSY:
            spdef = spdef * 11 / 10;
            spd = max(spd * 9 / 10,1);
            break;
        case CAREFUL:
            spdef = spdef * 11 / 10;
            spatk = max(spatk * 9 / 10,1);
            break;
        case QUIRKY:
            break;
    }
}

// EVs ------------------------------------------------------------------------------------------------------------

EVs::EVs():Stats() {
    
}

EVs::~EVs() {
    
}

unsigned int EVs::get_total() {
    return hp + atk + def + spatk + spdef + spd;
}

bool EVs::changeAtk(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(atk + amount > MAX_EVS){
            if(atk == MAX_EVS)
                return false;
            amount = MAX_EVS - atk;
        }
        atk += amount;
        return true;
    }else if(amount < 0){
        if(atk < -amount)
            amount = -atk;
        atk += amount;
        return true;
    }
    return false;
}

bool EVs::changeDef(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(def + amount > MAX_EVS){
            if(def == MAX_EVS)
                return false;
            amount = MAX_EVS - def;
        }
        def += amount;
        return true;
    }else if(amount < 0){
        if(def < -amount)
            amount = -def;
        def += amount;
        return true;
    }
    return false;
} 

bool EVs::changeSpatk(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(spatk + amount > MAX_EVS){
            if(spatk == MAX_EVS)
                return false;
            amount = MAX_EVS - spatk;
        }
        spatk += amount;
        return true;
    }else if(amount < 0){
        if(spatk < -amount)
            amount = -spatk;
        spatk += amount;
        return true;
    }
    return false;
}

bool EVs::changeSpdef(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(spdef + amount > MAX_EVS){
            if(spdef == MAX_EVS)
                return false;
            amount = MAX_EVS - spdef;
        }
        spdef += amount;
        return true;
    }else if(amount < 0){
        if(spdef < -amount)
            amount = -spdef;
        spdef += amount;
        return true;
    }
    return false;
}


bool EVs::changeSpd(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(spd + amount > MAX_EVS){
            if(spd == MAX_EVS)
                return false;
            amount = MAX_EVS - spd;
        }
        spd += amount;
        return true;
    }else if(amount < 0){
        if(spd < -amount)
            amount = -spd;
        spd += amount;
        return true;
    }
    return false;
}


bool EVs::changeHp(int amount) {
    unsigned int total = get_total();
    if(amount > 0){
        if(total + amount > MAX_TOTAL_EVS){
            if(total == MAX_TOTAL_EVS)
                return false;
            amount = MAX_TOTAL_EVS - total;
        }
        if(hp + amount > MAX_EVS){
            if(hp == MAX_EVS)
                return false;
            amount = MAX_EVS - hp;
        }
        hp += amount;
        return true;
    }else if(amount < 0){
        if(hp < -amount)
            amount = -hp;
        hp += amount;
        return true;
    }
    return false;
}

// IVs ------------------------------------------------------------------------------------------------------------

IVs::IVs():Stats() {
    
}

IVs::~IVs() {
    
}

IVs IVs::generateRandomIVs() {
    IVs ivs;
    ivs.setHp(RNG::getRandomInteger(0, MAX_IVS));
    ivs.setAtk(RNG::getRandomInteger(0, MAX_IVS));
    ivs.setSpatk(RNG::getRandomInteger(0, MAX_IVS));
    ivs.setDef(RNG::getRandomInteger(0, MAX_IVS));
    ivs.setSpdef(RNG::getRandomInteger(0, MAX_IVS));
    ivs.setSpd(RNG::getRandomInteger(0, MAX_IVS));
    return ivs;
}

// FormKind ------------------------------------------------------------------------------------------------------------

std::string formKindToString(FormKind fk){
    switch(fk){
        case NO_FORM_KIND:
            return "NO_FORM";
        case ALTERNATE_FORM:
            return "ALTERNATE_FORM";
        case MEGA_EVOLUTION:
            return "MEGA_EVOLUTION";
        case ALOLAN_FORM:
            return "ALOLAN_FORM";
        case GALARIAN_FORM:
            return "GALARIAN_FORM";
        case PALDEAN_FORM:
            return "PALDEAN_FORM";
        case HISUIAN_FORM:
            return "HISUIAN_FORM";
        default:
            return "NO_FORM";
    }
}

FormKind stringToFormKind(std::string formkind){
    if(formkind == "NO_FORM")
        return NO_FORM_KIND;
    if(formkind == "ALTERNATE_FORM")
        return ALTERNATE_FORM;
    if(formkind == "MEGA_EVOLUTION")
        return MEGA_EVOLUTION;
    if(formkind == "ALOLAN_FORM")
        return ALOLAN_FORM;
    if(formkind == "GALARIAN_FORM")
        return GALARIAN_FORM;
    if(formkind == "PALDEAN_FORM")
        return PALDEAN_FORM;
    if(formkind == "HISUIAN_FORM")
        return HISUIAN_FORM;
    return NO_FORM_KIND;
}

// Experience ------------------------------------------------------------------------------------------------------------

unsigned int computeExperience(
    unsigned int base,unsigned int level,bool active_participation, 
    unsigned int level_active, bool is_traded,bool uses_lucky_egg, 
    bool is_past_evo_level){
    double participation_value = active_participation ? 1 : 2;
    double power_arg = (double)(2*level+10)/(double)(level+level_active+10);
    unsigned int exp_gained = ((base*level/5)*(1/participation_value)*pow(power_arg,2.5)+1);
    if(is_traded)
        exp_gained*=1.5;
    if(uses_lucky_egg)
        exp_gained*=1.5;
    if(is_past_evo_level)
        exp_gained = exp_gained*4915/4096;
    return exp_gained;
}
