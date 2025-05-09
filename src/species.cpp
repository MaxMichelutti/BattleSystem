#include "species.h"

// EvolutionMethod --------------------------------------------------------------------------------------

EvolutionMethod stringToEvolutionMethod(std::string evo_method){
    if(evo_method == "LEVEL")
        return LEVEL;
    if(evo_method == "USE_ITEM")
        return USE_EVO_ITEM;
    if(evo_method == "TRADE")
        return TRADE;
    if(evo_method == "LEVEL_NIGHT")
        return LEVEL_NIGHT;
    if(evo_method == "LEVEL_DAY")
        return LEVEL_DAY;
    if(evo_method == "FRIENDSHIP")
        return FRIENDSHIP;
    if(evo_method == "TRADE_ITEM")
        return TRADE_ITEM;
    if(evo_method == "FRIENDSHIP_NIGHT")
        return FRIENDSHIP_NIGHT;
    if(evo_method == "FRIENDSHIP_DAY")
        return FRIENDSHIP_DAY;
    if(evo_method == "USE_ITEM_DAY")
        return USE_EVO_ITEM_DAY;
    if(evo_method == "USE_ITEM_NIGHT")
        return USE_EVO_ITEM_NIGHT;
    if(evo_method=="LEVEL_ATK")
        return LEVEL_ATK;
    if(evo_method=="LEVEL_DEF")
        return LEVEL_DEF;
    if(evo_method=="LEVEL_EQUAL_ATK_DEF")
        return LEVEL_EQUAL_ATK_DEF;
    if(evo_method=="CASCOON_SILCOON" || evo_method=="SILCOON_CASCOON")
        return CASCOON_SILCOON;
    if(evo_method=="SHEDINJA")
        return SHEDINJA;
    if(evo_method=="LEVEL_MALE")
        return LEVEL_MALE;
    if(evo_method=="LEVEL_FEMALE")
        return LEVEL_FEMALE;
    if(evo_method=="USE_ITEM_MALE")
        return LEVEL_MALE;
    if(evo_method=="USE_ITEM_FEMALE")
        return LEVEL_FEMALE;
    if(evo_method=="KNOW_ATTACK")
        return KNOW_ATTACK;
    if(evo_method=="HELD_ITEM_DAY")
        return HELD_ITEM_DAY;
    if(evo_method=="HELD_ITEM_NIGHT")
        return HELD_ITEM_NIGHT;
    if(evo_method=="HAS_IN_TEAM")
        return HAS_IN_TEAM;
    if(evo_method=="TRADE_WITH_SPECIES")
        return TRADE_ITEM;
    // return LEVEL as default
    return LEVEL;
}

std::string evolutionMethodToString(EvolutionMethod evo_method){
    switch(evo_method){
        case LEVEL:
            return "LEVEL";
        case USE_EVO_ITEM:
            return "USE_ITEM";
        case TRADE:
            return "TRADE";
        case LEVEL_NIGHT:
            return "LEVEL_NIGHT";
        case LEVEL_DAY:
            return "LEVEL_DAY";
        case FRIENDSHIP:
            return "FRIENDSHIP";
        case TRADE_ITEM:
            return "TRADE_ITEM";
        case FRIENDSHIP_NIGHT:
            return "FRIENDSHIP_NIGHT";
        case FRIENDSHIP_DAY:
            return "FRIENDSHIP_DAY";
        case USE_EVO_ITEM_DAY:
            return "USE_ITEM_DAY";
        case USE_EVO_ITEM_NIGHT:
            return "USE_ITEM_NIGHT";
        case LEVEL_ATK:
            return "LEVEL_ATK";
        case LEVEL_DEF:
            return "LEVEL_DEF";
        case LEVEL_EQUAL_ATK_DEF:
            return "LEVEL_EQUAL_ATK_DEF";
        case CASCOON_SILCOON:
            return "CASCOON_SILCOON";
        case SHEDINJA:
            return "SHEDINJA";
        case LEVEL_MALE:
            return "LEVEL_MALE";
        case USE_EVO_ITEM_MALE:
            return "USE_ITEM_MALE";
        case LEVEL_FEMALE:
            return "LEVEL_FEMALE";
        case USE_EVO_ITEM_FEMALE:
            return "USE_ITEM_FEMALE";
        case KNOW_ATTACK:
            return "KNOW_ATTACK";
        case HELD_ITEM_DAY:
            return "HELD_ITEM_DAY";
        case HELD_ITEM_NIGHT:
            return "HELD_ITEM_NIGHT";
        case HAS_IN_TEAM:
            return "HAS_IN_TEAM";
        case TRADE_WITH_SPECIES:
            return "TRADE_WITH_SPECIES";
        // return LEVEL as default
        default:
            return "LEVEL";
    }
}

// Evolution --------------------------------------------------------------------------------------

Evolution::Evolution(){

}

Evolution::Evolution(unsigned int species, unsigned int form, EvolutionMethod evo_method, unsigned int evo_condition){
    target_species_id = species;
    this->evo_method = evo_method;
    this->method_condition = evo_condition;
    target_form_id = form;
}

Evolution::~Evolution(){
}

unsigned int Evolution::getMethodCondition()const{
    return method_condition;
}

unsigned int Evolution::getTargetSpeciesId()const{
    return target_species_id;
}

EvolutionMethod Evolution::getEvolutionMethod()const{
    return evo_method;
}

unsigned int Evolution::getTargetFormId()const{
    return target_form_id;
}

// Species --------------------------------------------------------------------------------------

Species::Species() {
    
}

Species::Species(unsigned int index, std::map<std::string,std::string> data){
    id = index;
    if (data.find("name") == data.end()){
        std::cerr << "Error: Species " << id << " has no name!" << std::endl;
        exit(DATA_ERROR);
    }else
        name = data["name"];
    if(data.find("type1") == data.end()){
        std::cerr << "Error: Species " << id << " has no type!" << std::endl;
        exit(DATA_ERROR);
    }else
        type1 = stringToType(data["type1"]);
    if(data.find("type2") == data.end())
        type2 = Type::NO_TYPE;
    else
        type2 = stringToType(data["type2"]);
    if(data.find("ability1") == data.end()){
        std::cerr << "Error: Species " << id << " has no ability!" << std::endl;
        exit(DATA_ERROR);
    }else
        ability1 = stringToAbility(data["ability1"]);
    if(data.find("ability2") == data.end())
        ability2 = NO_ABILITY;
    else
        ability2 = stringToAbility(data["ability2"]);
    if(data.find("hidden_ability") == data.end())
        hidden_ability = NO_ABILITY;
    else
        hidden_ability = stringToAbility(data["hidden_ability"]);
    if(data.find("weight") == data.end()){
        std::cerr << "Error: Species " << id << " has no weight!" << std::endl;
        exit(DATA_ERROR);
    }else
        weight_hg = stringToInteger(data["weight"]);
    if(data.find("height") == data.end()){
        std::cerr << "Error: Species " << id << " has no height!" << std::endl;
        exit(DATA_ERROR);
    }else
        height_dm = stringToInteger(data["height"]);
    if(data.find("exp_curve") == data.end()){
        std::cerr << "Error: Species " << id << " has no exp_curve!" << std::endl;
        exit(DATA_ERROR);
    }else
        exp_curve = stringToExpCurve(data["exp_curve"]);
    if(data.find("gender") == data.end())
        gender_rule = GenderRule::HALF_MALE_HALF_FEMALE;
    else
        gender_rule = stringToGenderRule(data["gender"]);
    if(data.find("friendship") == data.end())
        base_friendship = 70;
    else
        base_friendship = stringToInteger(data["friendship"]);
    if(data.find("catch_rate") == data.end())
        catch_rate = 3;
    else
        catch_rate = stringToInteger(data["catch_rate"]);
    if(data.find("exp_yield") == data.end())
        exp_yield = 64;
    else
        exp_yield = stringToInteger(data["exp_yield"]);
    if(data.find("ev_yield") == data.end()){
        ev_yield = Stats();//all at 0
    }else
        ev_yield = Stats(data["ev_yield"]);
    if(data.find("base_stats") == data.end()){
        std::cerr << "Error: Species " << id << " has no base stats!" << std::endl;
        exit(DATA_ERROR);
    }else
        basestats = Stats(data["base_stats"]);
    if(data.find("attacks") == data.end()){
        std::cerr << "Error: Species " << id << " has no attacks!" << std::endl;
        exit(DATA_ERROR);
    }else
        parseLearnset(data["attacks"]);
    if(data.find("evolutions")!=data.end()){
        parseEvolutions(data["evolutions"]);
    }
    if(data.find("evolution_attacks")!=data.end()){
        std::stringstream ss(data["evolution_attacks"]);
        unsigned int attack_id;
        while(ss >> attack_id){
            evolution_learnset.insert(attack_id);
        }
    }
}

void Species::parseLearnset(std::string data){
    std::stringstream ss(data);
    unsigned int level;
    unsigned int attack_id;
    while(ss >> level){
        if(!(ss >> attack_id)){
            std::cerr << "Malformed Attacks for Species "<<this->id<<std::endl;
            exit(DATA_ERROR);
        }
        learnset.insert({level,attack_id});
    }
}

void Species::parseEvolutions(std::string data){
    std::stringstream ss(data);
    unsigned int species;
    EvolutionMethod evo_method;
    std::string token;
    unsigned int evo_condition;
    unsigned int form_id;
    while(ss >> species){
        if(!(ss >> form_id)){
            std::cerr << "Malformed Evolution for Species "<<this->id<<std::endl;
            exit(DATA_ERROR);
        }
        if(!(ss >> token)){
            std::cerr << "Malformed Evolution for Species "<<this->id<<std::endl;
            exit(DATA_ERROR);
        }
        evo_method = stringToEvolutionMethod(token);
        if(!(ss >> evo_condition)){
            std::cerr << "Malformed Evolution for Species "<<this->id<<std::endl;
            exit(DATA_ERROR);
        }
        evolutions.push_back(Evolution(species,form_id,evo_method,evo_condition));
    }
}

Species::~Species(){
    #ifdef DEBUG
    std::cout << "Deleting species " << id << std::endl;
    #endif
}

unsigned int Species::getId()const {
    return id;
}

void Species::checkAlternateForm(AlternateForm* form)const{
    if(form == nullptr){
        std::cerr << "Error: Alternate form not found!" << std::endl;
        exit(DATA_ERROR);
    }
    if(form->getSpeciesId() != id){
        std::cerr << "Error: Alternate form does not belong to species " << id << "!" << std::endl;
        exit(DATA_ERROR);
    }
}

Stats Species::getBaseStats(unsigned int form_id)const {
    if(form_id == 0)
        return basestats;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Stats> form_basestats = form->getBaseStats();
    if(form_basestats.has_value())
        return *form_basestats;
    return basestats;
}

Type Species::getType1(unsigned int form_id)const {
    if(form_id == 0)
        return type1;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Type> form_type1 = form->getType1();
    if(form_type1.has_value())
        return *form_type1;
    return type1;
}

Type Species::getType2(unsigned int form_id)const {
    if(form_id == 0)
        return type2;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Type> form_type2 = form->getType2();
    if(form_type2.has_value())
        return *form_type2;
    return type2;
}

std::string Species::getName()const {
    return name;
}

void Species::addAlternateFormId(unsigned int formid){
    alternate_forms_ids.push_back(formid);
}

Ability Species::getAbility1(unsigned int form_id)const{
    if(form_id == 0)
        return ability1;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Ability> form_a1 = form->getAbility1();
    if(form_a1.has_value())
        return *form_a1;
    return ability1;
}
Ability Species::getAbility2(unsigned int form_id)const{
    if(form_id == 0)
        return ability2;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Ability> form_a2 = form->getAbility2();
    if(form_a2.has_value())
        return *form_a2;
    return ability2;
}
Ability Species::getHiddenAbility(unsigned int form_id)const{
    if(form_id == 0)
        return hidden_ability;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Ability> form_hidden = form->getHiddenAbility();
    if(form_hidden.has_value())
        return *form_hidden;
    return hidden_ability;
}

ExpCurve Species::getExpCurve(unsigned int form_id)const {
    if(form_id == 0)
        return exp_curve;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<ExpCurve> form_exp = form->getExpCurve();
    if(form_exp.has_value())
        return *form_exp;
    return exp_curve;
}

GenderRule Species::getGenderRule()const {
    return gender_rule;
}

unsigned int Species::getBaseFriendship(unsigned int form_id)const {
    if(form_id == 0)
        return base_friendship;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<unsigned int> form_friend = form->getBaseFriendship();
    if(form_friend.has_value())
        return *form_friend;
    return base_friendship;
}

unsigned int Species::getHeight(unsigned int form_id)const {
    if(form_id == 0)
        return height_dm;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<unsigned int> form_height = form->getHeight();
    if(form_height.has_value())
        return *form_height;
    return height_dm;
}

unsigned int Species::getWeight(unsigned int form_id)const {
    if(form_id == 0)
        return weight_hg;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<unsigned int> form_weight = form->getWeight();
    if(form_weight.has_value())
        return *form_weight;
    return weight_hg;
}

unsigned int Species::getCatchRate(unsigned int form_id)const {
    if(form_id == 0)
        return catch_rate;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<unsigned int> form_catch = form->getCatchRate();
    if(form_catch.has_value())
        return *form_catch;
    return catch_rate;
}

unsigned int Species::getExpYield(unsigned int form_id)const {
    if(form_id == 0)
        return exp_yield;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<unsigned int> form_ey = form->getExpYield();
    if(form_ey.has_value())
        return *form_ey;
    return exp_yield;
}

Stats Species::getEVYield(unsigned int form_id)const{
    if(form_id == 0)
        return ev_yield;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<Stats> form_ev = form->getEVYield();
    if(form_ev.has_value())
        return *form_ev;
    return ev_yield;
}

std::vector<Evolution> Species::getEvolutions(unsigned int form_id)const{
    if(form_id == 0)
        return evolutions;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<std::vector<Evolution>> form_evos = form->getEvolutions();
    if(form_evos.has_value())
        return *form_evos;
    return evolutions;
}

std::set<unsigned int> Species::getLearnsetAt(unsigned int form_id,unsigned int level) {
    if(form_id == 0)
        return getLearnsetAtInternal(level);
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<std::set<unsigned int>> form_learnset = form->getLearnsetAt(level);
    if(form_learnset.has_value())
        return *form_learnset;
    return getLearnsetAtInternal(level);
}

std::set<unsigned int> Species::getLearnsetAtInternal(unsigned int level) {
    std::set<unsigned int> attacks;
    auto range = learnset.equal_range(level);
    for(auto it = range.first; it != range.second; ++it){
        attacks.insert(it->second);
    }
    return attacks;
}

std::set<unsigned int> Species::getLearnsetUntil(unsigned int form_id,unsigned int level) {
    if(form_id == 0)
        return getLearnsetUntilInternal(level);
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<std::set<unsigned int>> form_learnset = form->getLearnsetUntil(level);
    if(form_learnset.has_value())
        return *form_learnset;
    return getLearnsetUntilInternal(level);
}

std::set<unsigned int> Species::getLearnsetUntilInternal(unsigned int level) {
    std::set<unsigned int> attacks;
    for(unsigned int i = 1; i <= level; i++){
        auto range = learnset.equal_range(i);
        for(auto it = range.first; it != range.second; ++it){
            attacks.insert(it->second);
        }
    }
    return attacks;
}

std::set<unsigned int> Species::getEvolutionLearnset(unsigned int form_id)const{
    if(form_id == 0)
        return evolution_learnset;
    AlternateForm* form = AlternateForm::getAlternateForm(form_id);
    checkAlternateForm(form);
    std::optional<std::set<unsigned int>> form_evolution_learnset = form->getEvolutionLearnset();
    if(form_evolution_learnset.has_value())
        return *form_evolution_learnset;
    return evolution_learnset;
}

Species* Species::getSpecies(unsigned int species_id) {
    auto iter = static_species.find(species_id);
    if (iter == static_species.end())
        return nullptr;
    return iter->second;
}

void Species::loadSpecies(){
    static_species[0] = nullptr;
    std::ifstream file(SPECIES_FILE);
    if(!file.is_open()){
        std::cerr << "Error: Could not open file " << SPECIES_FILE << std::endl;
        exit(FILE_ERROR);
    }
    std::string line;
    unsigned int current_species_id = 1;
    std::map<std::string,std::string> parsed_data;
    while(std::getline(file,line)){
        if(line[0] == '%')
            continue;
        if(line[0] == '#'){
            if(parsed_data.size() > 0){
                Species* species = new Species(current_species_id,parsed_data);
                static_species.insert({current_species_id,species});
                current_species_id++;
                parsed_data.clear();
            }
        }else if(line.rfind("NAME:",0)==0){
            parsed_data["name"] = line.substr(5);
        }else if(line.rfind("TYPE1:",0)==0){
            parsed_data["type1"] = line.substr(6);
        }else if(line.rfind("TYPE2:",0)==0){
            parsed_data["type2"] = line.substr(6);
        }else if(line.rfind("BASESTATS:",0)==0){
            parsed_data["base_stats"] = line.substr(10);
        }else if(line.rfind("EXPCURVE:",0)==0){
            parsed_data["exp_curve"] = line.substr(9);
        }else if(line.rfind("ATTACKS:",0)==0){
            parsed_data["attacks"] = line.substr(8);
        }else if(line.rfind("GENDER:",0)==0){
            parsed_data["gender"] = line.substr(7);
        }else if(line.rfind("FRIENDSHIP:",0)==0){
            parsed_data["friendship"] = line.substr(11);
        }else if(line.rfind("WEIGHT:",0)==0){
            parsed_data["weight"] = line.substr(7);
        }else if(line.rfind("HEIGHT:",0)==0){
            parsed_data["height"] = line.substr(7);
        }else if(line.rfind("CATCHRATE:",0)==0){
            parsed_data["catch_rate"] = line.substr(10);
        }else if(line.rfind("EXPYIELD:",0)==0){
            parsed_data["exp_yield"] = line.substr(9);
        }else if(line.rfind("EVYIELD:",0)==0){
            parsed_data["ev_yield"] = line.substr(8);
        }else if(line.rfind("EVOLUTIONS:",0)==0){
            parsed_data["evolutions"] = line.substr(11);
        }else if(line.rfind("EVOLUTIONATTACKS:",0)==0){
            parsed_data["evolution_attacks"] = line.substr(17);
        }else if(line.rfind("ABILITY1:",0)==0){
            parsed_data["ability1"] = line.substr(9);
        }else if(line.rfind("ABILITY2:",0)==0){
            parsed_data["ability2"] = line.substr(9);
        }else if(line.rfind("HIDDENABILITY:",0)==0){
            parsed_data["hidden_ability"] = line.substr(14);
        }else{
            std::cout<<"Error: Unknown line in species file: " << line << std::endl;
        }
    }
    if(parsed_data.size() > 0){
        Species* species = new Species(current_species_id,parsed_data);
        static_species.insert({current_species_id,species});
    }
    file.close();
    AlternateForm::loadAlternateForms();
}

void Species::printSummary()const{
    std::cout<<"# "<<id<<" "<<name<<std::endl;
    std::cout<<"Type(s):       ";
    if(type1!=NO_TYPE)
        std::cout<<typeToString(type1);
    if(type2!=NO_TYPE)
        std::cout<<", "<<typeToString(type2);
    std::cout<<std::endl;
    std::cout<<"Weight:        "<<oneDecimalIntegerToString(weight_hg)<<" Kg"<<std::endl;
    std::cout<<"Height:        "<<oneDecimalIntegerToString(height_dm)<<" m"<<std::endl;
    std::cout<<"Abilities:     ";
    if(ability1!=NO_ABILITY)
        std::cout<<abilityToString(ability1);
    if(ability2!=NO_ABILITY)
        std::cout<<", "<<abilityToString(ability2);
    std::cout<<std::endl;
    if(hidden_ability!=NO_ABILITY)
        std::cout<<"Hidden ability:"<<abilityToString(hidden_ability)<<std::endl;
    std::cout<<"BASE STATS:    "<<"HP:   "<<basestats.getHp()<<std::endl;
    std::cout<<"               "<<"ATT:  "<<basestats.getAtk()<<std::endl;
    std::cout<<"               "<<"DEF:  "<<basestats.getDef()<<std::endl;
    std::cout<<"               "<<"SPATT:"<<basestats.getSpatk()<<std::endl;
    std::cout<<"               "<<"SPDEF:"<<basestats.getSpdef()<<std::endl;
    std::cout<<"               "<<"SPD:  "<<basestats.getSpd()<<std::endl;
    std::cout<<"EV YIELD:      "<<"HP:   "<<ev_yield.getHp()<<std::endl;
    std::cout<<"               "<<"ATT:  "<<ev_yield.getAtk()<<std::endl;
    std::cout<<"               "<<"DEF:  "<<ev_yield.getDef()<<std::endl;
    std::cout<<"               "<<"SPATT:"<<ev_yield.getSpatk()<<std::endl;
    std::cout<<"               "<<"SPDEF:"<<ev_yield.getSpdef()<<std::endl;
    std::cout<<"               "<<"SPD:  "<<ev_yield.getSpd()<<std::endl;
    std::cout<<"GENDER RULE:   "<<genderRuleToString(gender_rule)<<std::endl;
    std::cout<<"EXP CURVE:     "<<expCurveToString(exp_curve)<<std::endl;
    std::cout<<"BASE FRIEND.:  "<<base_friendship<<std::endl;
    std::cout<<"CATCH RATE:    "<<catch_rate<<std::endl;
    std::cout<<"LEARNSET:"<<std::endl;
    int count = 0;
    for(auto it=learnset.begin();it!=learnset.end();it++){
        Attack* attack = Attack::getAttack(it->second);
        if(next(it)!=learnset.end())
            std::cout<<it->first<<" "<<attack->getName()<<", ";
        else
        std::cout<<it->first<<" "<<attack->getName();
        count++;
        if(count%5==0)
            std::cout<<std::endl;
    }
    if(!(count%5==0))
        std::cout<<std::endl;
    if(!evolutions.empty()){
        std::cout<<"EVOLUTIONS:"<<std::endl;
        for(auto it=evolutions.begin();it!=evolutions.end();it++){
            Species* target = Species::getSpecies(it->getTargetSpeciesId());
            std::cout<<target->getName()<<" "
                <<evolutionMethodToString(it->getEvolutionMethod())<<" "
                <<it->getMethodCondition()<<std::endl;
        }
    }
    if(!evolution_learnset.empty()){
        std::cout<<"EVOLUTION ATTACKS:"<<std::endl;
        for(auto it=evolution_learnset.begin();it!=evolution_learnset.end();it++){
            Attack* attack = Attack::getAttack(*it);
            if(next(it)==evolution_learnset.end())
                std::cout<<attack->getName()<<std::endl;
            else
                std::cout<<attack->getName()<<", ";
        }
    }
}

void Species::printAllSummaries(){
    for(auto it=static_species.begin();it!=static_species.end();it++){
        if(it->second != nullptr)
            it->second->printSummary();
    }
}

unsigned int Species::getFormIdOfKind(FormKind form_kind)const{
    for(auto it=alternate_forms_ids.begin();it!=alternate_forms_ids.end();it++){
        AlternateForm* form = AlternateForm::getAlternateForm(*it);
        if(form==nullptr)
            continue;
        if(form->getFormKind() == form_kind)
            return *it;
    }
    return 0;
}

// AlternateForm --------------------------------------------------------------------------------------
AlternateForm::AlternateForm(){
    species_id = 0;
    form_kind = NO_FORM_KIND;
}
AlternateForm::AlternateForm(std::map<std::string,std::string> data){
    if(data.find("species") == data.end()){
        std::cerr << "Error: Alternate form has no species id!" << std::endl;
        exit(DATA_ERROR);
    }else
        species_id = stringToInteger(data["species"]);
    if(data.find("form_kind") == data.end()){
        std::cerr << "Error: Alternate form has no form kind!" << std::endl;
        exit(DATA_ERROR);
    }else
        form_kind = stringToFormKind(data["form_kind"]);
    if(data.find("type1") != data.end())
        type1 = stringToType(data["type1"]);
    if(data.find("type2") != data.end())
        type2 = stringToType(data["type2"]);
    if(data.find("ability1") != data.end())
        ability1 = stringToAbility(data["ability1"]);
    if(data.find("ability2") != data.end())
        ability2 = stringToAbility(data["ability2"]);
    if(data.find("hidden_ability") != data.end())
        hidden_ability = stringToAbility(data["hidden_ability"]);
    if(data.find("weight") != data.end())
        weight_hg = stringToInteger(data["weight"]);
    if(data.find("height") != data.end())
        height_dm = stringToInteger(data["height"]);
    if(data.find("exp_curve") != data.end())
        exp_curve = stringToExpCurve(data["exp_curve"]);
    if(data.find("friendship") != data.end())
        base_friendship = stringToInteger(data["friendship"]);
    if(data.find("catch_rate") != data.end())
        catch_rate = stringToInteger(data["catch_rate"]);
    if(data.find("exp_yield") != data.end())
        exp_yield = stringToInteger(data["exp_yield"]);
    if(data.find("ev_yield") != data.end())
        ev_yield = Stats(data["ev_yield"]);
    if(data.find("base_stats") != data.end())
        basestats = Stats(data["base_stats"]);
    if(data.find("attacks") != data.end())
        parseLearnset(data["attacks"]);
    if(data.find("evolutions")!=data.end())
        parseEvolutions(data["evolutions"]);
    if(data.find("evolution_attacks")!=data.end()){
        evolution_learnset = std::set<unsigned int>();
        std::stringstream ss(data["evolution_attacks"]);
        unsigned int attack_id;
        while(ss >> attack_id){
            evolution_learnset->insert(attack_id);
        }
    }
    Species* species = Species::getSpecies(species_id);
    if(species == nullptr){
        std::cerr << "Error: Species for Alternate form " << species_id << " does not exist!" << std::endl;
        exit(DATA_ERROR);
    }
}
AlternateForm::~AlternateForm(){
    #ifdef DEBUG
    std::cout << "Deleting alternate form " << species_id << std::endl;
    #endif
}
unsigned int AlternateForm::getSpeciesId()const{
    return species_id;
}
FormKind AlternateForm::getFormKind()const{
    return form_kind;
}
std::optional<Type> AlternateForm::getType1()const{
    return type1;
}
std::optional<Type> AlternateForm::getType2()const{
    return type2;
}
std::optional<Ability> AlternateForm::getAbility1()const{
    return ability1;
}
std::optional<Ability> AlternateForm::getAbility2()const{
    return ability2;
}
std::optional<Ability> AlternateForm::getHiddenAbility()const{
    return hidden_ability;
}
std::optional<Stats> AlternateForm::getBaseStats()const{
    return basestats;
}
std::optional<unsigned int> AlternateForm::getBaseFriendship()const{
    return base_friendship;
}
std::optional<unsigned int> AlternateForm::getHeight()const{
    return height_dm;
}
std::optional<unsigned int> AlternateForm::getWeight()const{
    return weight_hg;
}
std::optional<unsigned int> AlternateForm::getCatchRate()const{
    return catch_rate;
}
std::optional<unsigned int> AlternateForm::getExpYield()const{
    return exp_yield;
}
std::optional<ExpCurve> AlternateForm::getExpCurve()const{
    return exp_curve;
}
std::optional<Stats> AlternateForm::getEVYield()const{
    return ev_yield;
}
std::optional<std::vector<Evolution>> AlternateForm::getEvolutions()const{
    return evolutions;
}
std::optional<std::set<unsigned int>> AlternateForm::getEvolutionLearnset()const{
    return evolution_learnset;
}

std::optional<std::set<unsigned int>> AlternateForm::getLearnsetAt(unsigned int level)const {
    if(!learnset.has_value())
        return std::optional<std::set<unsigned int>>(std::nullopt);
    std::set<unsigned int> attacks;
    auto range = learnset->equal_range(level);
    for(auto it = range.first; it != range.second; ++it){
        attacks.insert(it->second);
    }
    return std::optional<std::set<unsigned int>>(attacks);
}

std::optional<std::set<unsigned int>> AlternateForm::getLearnsetUntil(unsigned int level)const {
    if(!learnset.has_value())
        return std::optional<std::set<unsigned int>>(std::nullopt);
    std::set<unsigned int> attacks;
    for(unsigned int i = 1; i <= level; i++){
        auto range = learnset->equal_range(i);
        for(auto it = range.first; it != range.second; ++it){
            attacks.insert(it->second);
        }
    }
    return std::optional<std::set<unsigned int>>(attacks);
}

std::optional<std::multimap<unsigned int,unsigned int>> AlternateForm::getLearnset()const{
    return learnset;
}
AlternateForm* AlternateForm::getAlternateForm(unsigned int species_id) {
    auto iter = static_alternate_forms.find(species_id);
    if (iter == static_alternate_forms.end())
        return nullptr;
    return iter->second;
}

void AlternateForm::loadAlternateForms(){
    static_alternate_forms[0] = nullptr;
    std::ifstream file(FORMS_FILE);
    if(!file.is_open()){
        std::cerr << "Error: Could not open file " << FORMS_FILE << std::endl;
        exit(FILE_ERROR);
    }
    std::string line;
    unsigned int current_form_id = 1;
    std::map<std::string,std::string> parsed_data;
    while(std::getline(file,line)){
        if(line.empty() || line[0] == '%')
            continue;
        if(line[0] == '#'){
            if(parsed_data.size() > 0){
                AlternateForm* form = new AlternateForm(parsed_data);
                static_alternate_forms.insert({current_form_id,form});
                parsed_data.clear();
                Species* species = Species::getSpecies(form->getSpeciesId());
                if(species == nullptr){
                    std::cerr << "Error: Species for Form " << current_form_id << " does not exist!" << std::endl;
                    exit(DATA_ERROR);
                }
                species->addAlternateFormId(current_form_id);
                current_form_id++;
            }
        }else if(line.rfind("SPECIES:",0)==0){
            parsed_data["species"] = line.substr(8);
        }else if(line.rfind("FORMKIND:",0)==0){
            parsed_data["form_kind"] = line.substr(9);
        }else if(line.rfind("TYPE1:",0)==0){
            parsed_data["type1"] = line.substr(6);
        }else if(line.rfind("TYPE2:",0)==0){
            parsed_data["type2"] = line.substr(6);
        }else if(line.rfind("BASESTATS:",0)==0){
            parsed_data["base_stats"] = line.substr(10);
        }else if(line.rfind("EXPCURVE:",0)==0){
            parsed_data["exp_curve"] = line.substr(9);
        }else if(line.rfind("ATTACKS:",0)==0){
            parsed_data["attacks"] = line.substr(8);
        }else if(line.rfind("FRIENDSHIP:",0)==0){
            parsed_data["friendship"] = line.substr(11);
        }else if(line.rfind("WEIGHT:",0)==0){
            parsed_data["weight"] = line.substr(7);
        }else if(line.rfind("HEIGHT:",0)==0){
            parsed_data["height"] = line.substr(7);
        }else if(line.rfind("CATCHRATE:",0)==0){
            parsed_data["catch_rate"] = line.substr(10);
        }else if(line.rfind("EXPYIELD:",0)==0){
            parsed_data["exp_yield"] = line.substr(9);
        }else if(line.rfind("EVYIELD:",0)==0){
            parsed_data["ev_yield"] = line.substr(8);
        }else if(line.rfind("EVOLUTIONS:",0)==0){
            parsed_data["evolutions"] = line.substr(11);
        }else if(line.rfind("EVOLUTIONATTACKS:",0)==0){
            parsed_data["evolution_attacks"] = line.substr(17);
        }else if(line.rfind("ABILITY1:",0)==0){
            parsed_data["ability1"] = line.substr(9);
        }else if(line.rfind("ABILITY2:",0)==0){
            parsed_data["ability2"] = line.substr(9);
        }else if(line.rfind("HIDDENABILITY:",0)==0){
            parsed_data["hidden_ability"] = line.substr(14);
        }else{
            std::cout<<"Error: Unknown line in species file: " << line << std::endl;
        }
    }
    if(parsed_data.size() > 0){
        AlternateForm* form = new AlternateForm(parsed_data);
        static_alternate_forms.insert({current_form_id,form});
        Species* species = Species::getSpecies(form->getSpeciesId());
        if(species == nullptr){
            std::cerr << "Error: Species for Alternate form " << current_form_id << " does not exist!" << std::endl;
            exit(DATA_ERROR);
        }
        species->addAlternateFormId(current_form_id);
    }
    file.close();
}

void AlternateForm::parseLearnset(std::string data){
    std::stringstream ss(data);
    unsigned int level;
    unsigned int attack_id;
    while(ss >> level){
        if(!(ss >> attack_id)){
            std::cerr << "Malformed Attacks for Form "<<std::endl;
            exit(DATA_ERROR);
        }
        if(!learnset.has_value()){
            learnset = std::multimap<unsigned int,unsigned int>();
        }
        learnset->insert({level,attack_id});
    }
}

void AlternateForm::parseEvolutions(std::string data){
    std::stringstream ss(data);
    unsigned int species;
    EvolutionMethod evo_method;
    std::string token;
    unsigned int evo_condition;
    unsigned int form_id;
    while(ss >> species){
        if(!(ss >> form_id)){
            std::cerr << "Malformed Evolution for Form "<<std::endl;
            exit(DATA_ERROR);
        }
        if(!(ss >> token)){
            std::cerr << "Malformed Evolution for Form "<<std::endl;
            exit(DATA_ERROR);
        }
        evo_method = stringToEvolutionMethod(token);
        if(!(ss >> evo_condition)){
            std::cerr << "Malformed Evolution for Form "<<std::endl;
            exit(DATA_ERROR);
        }
        if(!evolutions.has_value()){
            evolutions = std::vector<Evolution>();
        }
        evolutions->push_back(Evolution(species,form_id,evo_method,evo_condition));
    }
}


bool Species::canMegaEvolve(unsigned int curr_form, ItemType mega_item, bool knows_dragon_ascent)const{
    switch(id){
        case 3:{//venusaur
            return curr_form==0 && mega_item==VENUSAURITE;
        }
        case 6:{//charizard
            return curr_form==0 && (mega_item==CHARIZARDITE_X || mega_item==CHARIZARDITE_Y);
        }
        case 9:{//blastoise
            return curr_form==0 && mega_item==BLASTOISINITE;
        }
        case 15:{//beedrill
            return curr_form==0 && mega_item==BEEDRILLITE;
        }
        case 18:{//pidgeot
            return curr_form==0 && mega_item==PIDGEOTITE;
        }
        case 65:{//alakazam
            return curr_form==0 && mega_item==ALAKAZITE;
        }
        case 80:{//slowbro
            return curr_form==0 && mega_item==SLOWBRONITE;
        }
        case 94:{//gengar
            return curr_form==0 && mega_item==GENGARITE;
        }
        case 115:{//kangaskhan
            return curr_form==0 && mega_item==KANGASKHANTITE;
        }
        case 127:{//pinsir
            return curr_form==0 && mega_item==PINSIRITE;
        }
        case 130:{//gyarados
            return curr_form==0 && mega_item==GYARADOSITE;
        }
        case 142:{//aerodactyl
            return curr_form==0 && mega_item==AERODACTYLITE;
        }
        case 150:{//mewtwo
            return curr_form==0 && (mega_item==MEWTWONITE_X || mega_item==MEWTWONITE_Y);
        }
        case 181:{//ampharos
            return curr_form==0 && mega_item==AMPHAROSITE;
        }
        case 208:{//steelix
            return curr_form==0 && mega_item==STEELIXITE;
        }
        case 212:{//scizor
            return curr_form==0 && mega_item==SCIZORITE;
        }
        case 214:{//heracross
            return curr_form==0 && mega_item==HERACRONITE;
        }
        case 229:{//houndoom
            return curr_form==0 && mega_item==HOUNDOOMINITE;
        }
        case 248:{//tyranitar
            return curr_form==0 && mega_item==TYRANITARITE;
        }
        case 254:{//sceptile
            return curr_form==0 && mega_item==SCEPTILITE;
        }
        case 257:{//blaziken
            return curr_form==0 && mega_item==BLAZIKENITE;
        }
        case 260:{//swampert
            return curr_form==0 && mega_item==SWAMPERTITE;
        }
        case 282:{//gardevoir
            return curr_form==0 && mega_item==GARDEVOIRITE;
        }
        case 302:{//sableye
            return curr_form==0 && mega_item==SABLENITE;
        }
        case 303:{//mawile
            return curr_form==0 && mega_item==MAWILITE;
        }
        case 306:{//aggron
            return curr_form==0 && mega_item==AGGRONITE;
        }
        case 308:{//medicham
            return curr_form==0 && mega_item==MEDICHAMITE;
        }
        case 310:{//manectric
            return curr_form==0 && mega_item==MANECTITE;
        }
        case 319:{//sharpedo
            return curr_form==0 && mega_item==SHARPEDONITE;
        }
        case 323:{//camerupt
            return curr_form==0 && mega_item==CAMERUPTITE;
        }
        case 334:{//altaria
            return curr_form==0 && mega_item==ALTARIANITE;
        }
        case 354:{//banette
            return curr_form==0 && mega_item==BANETTETITE;
        }
        case 359:{//absol
            return curr_form==0 && mega_item==ABSOLITE;
        }
        case 362:{//glalie
            return curr_form==0 && mega_item==GLALITITE;
        }
        case 373:{//salamence
            return curr_form==0 && mega_item==SALAMENCITE;
        }
        case 376:{//metagross
            return curr_form==0 && mega_item==METAGROSSITE;
        }
        case 380:{//latias
            return curr_form==0 && mega_item==LATIASITE;
        }
        case 381:{//latios
            return curr_form==0 && mega_item==LATIOSITE;
        }
        case 384:{//rayquaza
            return curr_form==0 && knows_dragon_ascent;
        }
        case 428:{//lopunny
            return curr_form==0 && mega_item==LOPUNNITE;
        }
        case 445:{//garchomp
            return curr_form==0 && mega_item==GARCHOMPITE;
        }
        case 448:{//lucario
            return curr_form==0 && mega_item==LUCARIONITE;
        }
        case 460:{//abomasnow
            return curr_form==0 && mega_item==ABOMASITE;
        }
        case 475:{//gallade
            return curr_form==0 && mega_item==GALLADITE;
        }
        case 531:{//audino
            return curr_form==0 && mega_item==AUDINITE;
        }
        default:return false;
    }
}

unsigned int Species::getMegaForm(unsigned int curr_form,ItemType mega_item, bool knows_dragon_ascent)const{
    if(!canMegaEvolve(curr_form,mega_item,knows_dragon_ascent)){
        return 0;//not actual mega
    }
    switch(id){
        case 3:{//venusaur
            if(curr_form==0 && mega_item==VENUSAURITE)
                return 3;
            break;
        }
        case 6:{//charizard
            if(curr_form==0 && mega_item==CHARIZARDITE_X)
                return 1;
            if(curr_form==0 && mega_item==CHARIZARDITE_Y)
                return 2;
            break;
        }
        case 9:{//blastoise
            if(curr_form==0 && mega_item==BLASTOISINITE)
                return 4;
            break;
        }
        case 15:{//beedrill
            if(curr_form==0 && mega_item==BEEDRILLITE)
                return 5;
            break;
        }
        case 18:{//pidgeot
            if(curr_form==0 && mega_item==PIDGEOTITE)
                return 6;
            break;
        }
        case 65:{//alakazam
            if(curr_form==0 && mega_item==ALAKAZITE)
                return 22;
            break;
        }
        case 80:{//slowbro
            if(curr_form==0 && mega_item==SLOWBRONITE)
                return 30;
            break;
        }
        case 94:{//gengar
            if(curr_form==0 && mega_item==GENGARITE)
                return 34;
            break;
        }
        case 115:{//kangaskhan
            if(curr_form==0 && mega_item==KANGASKHANTITE)
                return 43;
            break;
        }
        case 127:{//pinsir
            if(curr_form==0 && mega_item==PINSIRITE)
                return 45;
            break;
        }
        case 130:{//gyarados
            if(curr_form==0 && mega_item==GYARADOSITE)
                return 49;
            break;
        }
        case 142:{//aerodactyl
            if(curr_form==0 && mega_item==AERODACTYLITE)
                return 50;
            break;
        }
        case 150:{//mewtwo
            if(curr_form==0 && mega_item==MEWTWONITE_X)
                return 54;
            if(curr_form==0 && mega_item==MEWTWONITE_Y)
                return 55;
            break;
        }
        case 181:{//ampharos
            if(curr_form==0 && mega_item==AMPHAROSITE)
                return 61;
            break;
        }
        case 208:{//steelix
            if(curr_form==0 && mega_item==STEELIXITE)
                return 91;
            break;
        }
        case 212:{//scizor
            if(curr_form==0 && mega_item==SCIZORITE)
                return 93;
            break;
        }
        case 214:{//heracross
            if(curr_form==0 && mega_item==HERACRONITE)
                return 94;
            break;
        }
        case 229:{//houndoom
            if(curr_form==0 && mega_item==HOUNDOOMINITE)
                return 97;
            break;
        }
        case 248:{//tyranitar
            if(curr_form==0 && mega_item==TYRANITARITE)
                return 98;
            break;
        }
        case 254:{//sceptile
            if(curr_form==0 && mega_item==SCEPTILITE)
                return 99;
            break;
        }
        case 257:{//blaziken
            if(curr_form==0 && mega_item==BLAZIKENITE)
                return 100;
            break;
        }
        case 260:{//swampert
            if(curr_form==0 && mega_item==SWAMPERTITE)
                return 101;
            break;
        }
        case 282:{//gardevoir
            if(curr_form==0 && mega_item==GARDEVOIRITE)
                return 104;
            break;
        }
        case 302:{//sableye
            if(curr_form==0 && mega_item==SABLENITE)
                return 105;
            break;
        }
        case 303:{//mawile
            if(curr_form==0 && mega_item==MAWILITE)
                return 106;
            break;
        }
        case 306:{//aggron
            if(curr_form==0 && mega_item==AGGRONITE)
                return 107;
            break;
        }
        case 308:{//medicham
            if(curr_form==0 && mega_item==MEDICHAMITE)
                return 108;
            break;
        }
        case 310:{//manectric
            if(curr_form==0 && mega_item==MANECTITE)
                return 109;
            break;
        }
        case 319:{//sharpedo
            if(curr_form==0 && mega_item==SHARPEDONITE)
                return 110;
            break;
        }
        case 323:{//camerupt
            if(curr_form==0 && mega_item==CAMERUPTITE)
                return 111;
            break;
        }
        case 334:{//altaria
            if(curr_form==0 && mega_item==ALTARIANITE)
                return 112;
            break;
        }
        case 354:{//banette
            if(curr_form==0 && mega_item==BANETTETITE)
                return 116;
            break;
        }
        case 359:{//absol
            if(curr_form==0 && mega_item==ABSOLITE)
                return 117;
            break;
        }
        case 362:{//glalie
            if(curr_form==0 && mega_item==GLALITITE)
                return 118;
            break;
        }
        case 373:{//salamence
            if(curr_form==0 && mega_item==SALAMENCITE)
                return 119;
            break;
        }
        case 376:{//metagross
            if(curr_form==0 && mega_item==METAGROSSITE)
                return 120;
            break;
        }
        case 381:{//latios
            if(curr_form==0 && mega_item==LATIOSITE)
                return 121;
            break;
        }
        case 380:{//latias
            if(curr_form==0 && mega_item==LATIASITE)
                return 122;
            break;
        }
        case 384:{//rayquaza
            if(curr_form==0 && knows_dragon_ascent)
                return 125;
            break;
        }
        case 428:{//lopunny
            if(curr_form==0 && mega_item==LOPUNNITE)
                return 136;
            break;
        }
        case 445:{//garchomp
            if(curr_form==0 && mega_item==GARCHOMPITE)
                return 138;
            break;
        }
        case 448:{//lucario
            if(curr_form==0 && mega_item==LUCARIONITE)
                return 139;
            break;
        }
        case 460:{//abomasnow
            if(curr_form==0 && mega_item==ABOMASITE)
                return 140;
            break;
        }
        case 475:{//gallade
            if(curr_form==0 && mega_item==GALLADITE)
                return 141;
            break;
        }
        case 531:{//audino
            if(curr_form==0 && mega_item==AUDINITE)
                return 171;
            break;
        }
        default:return 0;
    }
    return 0;
}

unsigned int Species::getNonMegaForm(unsigned int curr_form)const{
    // if any alternate form receives a mega evo, 
    // here we return the standard non mega eveolved 
    // form which may be non-0 
    return 0;
}

bool Species::hasForm(unsigned int form_id)const{
    for(unsigned int i=0;i<alternate_forms_ids.size();i++){
        if(alternate_forms_ids[i] == form_id)
            return true;
    }
    return false;
}