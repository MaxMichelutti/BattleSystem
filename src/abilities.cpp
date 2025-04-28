#include "abilities.h"

// Ability ------------------------------------------------------------------------------------------------------------

std::string abilityToString(Ability ability){
    if(abilityMap.find(ability)==abilityMap.end())
        return "NO_ABILITY";
    AbilityItem* ability_item = abilityMap[ability];
    return ability_item->getIGName();
}

std::string abilityToSerialString(Ability ability){
    if(abilityMap.find(ability)==abilityMap.end())
        return "NO_ABILITY";
    AbilityItem* ability_item = abilityMap[ability];
    return ability_item->getSerialName();
}

Ability stringToAbility(std::string ability){
    auto it = strToAbility.find(ability);
    if(it!=strToAbility.end())
        return it->second;
    return NO_ABILITY;
}

bool isAbilityIgnorable(Ability ability){
    switch(ability){
        case BATTLE_ARMOR:
        case CLEAR_BODY:
        case WHITE_SMOKE:
        case DRY_SKIN:
        case DAMP:
        case FILTER:
        case SOLID_ROCK:
        case FLASH_FIRE:
        case HYPER_CUTTER:
        case INNER_FOCUS:
        case INSOMNIA:
        case KEEN_EYE:
        case LEAF_GUARD:
        case LEVITATE:
        case LIGHTNING_ROD:
        case LIMBER:
        case OBLIVIOUS:
        case OWN_TEMPO:
        case SAND_VEIL:
        case SHELL_ARMOR:
        case SHIELD_DUST:
        case SOUNDPROOF:
        case STURDY:
        case TANGLED_FEET:
        case THICK_FAT:
        case UNAWARE:
        case VITAL_SPIRIT:
        case WATER_ABSORB:
        case WATER_VEIL:
        case BIG_PECKS:
        case FRIEND_GUARD:
        case WONDER_SKIN:
        case OVERCOAT:
        case ILLUMINATE:
        case IMMUNITY:
        case MARVEL_SCALE:
        case MULTISCALE:
        case MAGIC_BOUNCE:
        case SAP_SIPPER:
        case SNOW_CLOAK:
        case MAGMA_ARMOR:
        case SUCTION_CUPS:
        case LIGHT_METAL:
        case WONDER_GUARD:
        case FLOWER_GIFT:
        case STICKY_HOLD:
            return true;
        default:return false;
    }        
    return false;
}

bool isAbilityTraceable(Ability ability){
    switch(ability){
        case TRACE:
        case IMPOSTOR:
        case NEUTRALIZING_GAS:
        case POWER_OF_ALCHEMY:
        case PARENTAL_BOND:
        case FORECAST:
        case PRIMORDIAL_SEA:
        case DESOLATE_LAND:
            return false;
        default: return true;
    }
    return true;
}

bool doesAbilityIgnoreIntimidate(Ability ability){
    switch(ability){
        case INNER_FOCUS:
        case OWN_TEMPO:
        case SCRAPPY:
        case OBLIVIOUS:
            return true;
        default:
            return false;
    }
    return false;
}

bool abilityCannotBeCopied(Ability ability){
    switch(ability){
        case TRACE:
        case IMPOSTOR:
        case NEUTRALIZING_GAS:
        case POWER_OF_ALCHEMY:
        case PARENTAL_BOND:
        case FORECAST:
        case PRIMORDIAL_SEA:
        case DESOLATE_LAND:
        case DELTA_STREAM:
            return true;
        default: return false;
    }
    return false;
}

bool abilityCannotBeChanged(Ability ability){
    switch(ability){
        case TRUANT:
        case FORECAST:
        case PRIMORDIAL_SEA:
        case DESOLATE_LAND:
        case DELTA_STREAM:
            return true;
        default: return false;
    }
    return false;
}

bool abilityCannotBeSuppressed(Ability ability){
    switch(ability){
        case FORECAST:
        case PRIMORDIAL_SEA:
        case DESOLATE_LAND:
        case DELTA_STREAM:
            return true;
        default: return false;
    }
    return false;
}

// Ability Item ------------------------------------------------------------------------------------------------------------
AbilityItem::AbilityItem(){
    igname = "";
    serial_name = "";
    description = "";
}

AbilityItem::AbilityItem(std::string igname, std::string serial_name, std::string description){
    this->igname = igname;
    this->serial_name = serial_name;
    this->description = description;
}

AbilityItem::~AbilityItem(){
    // Destructor
}

std::string AbilityItem::getIGName()const{
    return igname;
}

std::string AbilityItem::getSerialName()const{
    return serial_name;
}

std::string AbilityItem::getDescription()const{
    return description;
}

void AbilityItem::initAbilities(){
    std::ifstream file(ABILITIES_FILE);

    if(!file.is_open()){
        std::cerr << "Error opening file: " << ABILITIES_FILE << std::endl;
        return;
    }

    std::string line;
    std::string serial_name;
    std::string desc;
    int count = 0;
    while(std::getline(file, line)){
        if(count%2==0)
            serial_name = line;
        else{
            desc = line;
            Ability key = stringToAbility(serial_name);
            if(key == NO_ABILITY){
                std::cerr << "Error: Ability not found for serial name: " << serial_name << std::endl;
                count++;
                continue;
            }
            std::string ign_name = internalToDisplay(serial_name);
            AbilityItem* ability_item = new AbilityItem(ign_name, serial_name, desc);
            abilityMap.insert({key, ability_item});
        }
        count++;
    }
    file.close();
}

AbilityItem* AbilityItem::getAbilityItem(Ability ability){
    auto it = abilityMap.find(ability);
    if(it != abilityMap.end())
        return it->second;
    return nullptr;
}