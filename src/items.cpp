#include "items.h"

std::string ItemCategoryToString(ItemCategory category){
    switch (category) {
        case NO_CATEGORY: return "No Category";
        case KEY_ITEM: return "Key Item";
        case BERRY: return "Berry";
        case BALL: return "Ball";
        case MEDICINE: return "Medicine";
        case EVOLUTION_ITEM: return "Evolution Item";
        case TM: return "TM";
        case HELD_ITEM: return "Held Item";
        case GENERAL_ITEM: return "General Item";
        default: return "Unknown Category";
    }
}
ItemCategory StringToItemCategory(std::string category){
    if (category == "NO_CATEGORY") return NO_CATEGORY;
    else if (category == "KEY") return KEY_ITEM;
    else if (category == "BERRY") return BERRY;
    else if (category == "BALL") return BALL;
    else if (category == "MEDICINE") return MEDICINE;
    else if (category == "EVOLUTION") return EVOLUTION_ITEM;
    else if (category == "TM") return TM;
    else if (category == "HELD") return HELD_ITEM;
    else if (category == "GENERAL") return GENERAL_ITEM;
    else return NO_CATEGORY; // Default case
}

ItemType ItemData::getType() const {
    return type;
}
ItemCategory ItemData::getCategory() const {
    return category;
}
std::string ItemData::getName() const {
    return name;
}
std::string ItemData::getDescription() const {
    return description;
}
int ItemData::getPrice() const {
    return price;
}

void ItemData::loadItem(ItemType key,const std::unordered_map<std::string,std::string> &input_data){
    if(input_data.find("name") == input_data.end()){
        std::cerr << "Error: Item " << key << " has no name!" << std::endl;
        return;
    }
    if(input_data.find("description") == input_data.end()){
        std::cerr << "Error: Item " << key << " has no description!" << std::endl;
        return;
    }
    if(input_data.find("category") == input_data.end()){
        std::cerr << "Error: Item " << key << " has no category!" << std::endl;
        return;
    }
    if(input_data.find("price") == input_data.end()){
        std::cerr << "Error: Item " << key << " has no price!" << std::endl;
        return;
    }
    ItemCategory category = StringToItemCategory(input_data.at("category"));
    std::string name = input_data.at("name");
    std::string description = input_data.at("description");
    int price = std::stoi(input_data.at("price"));
    itemTypeMap[key] = new ItemData(key, category, name, description, price);
    if(input_data.find("flavour") != input_data.end())
        itemTypeMap[key]->setFlavours(input_data.at("flavour"));
    else{//set all flavours to 0
        for(int i=0;i<6;i++)
            itemTypeMap[key]->flavour[i] = 0;
    }
}

void ItemData::setFlavours(std::string flavour_string){
    std::istringstream iss(flavour_string);
    std::string flavour_value;
    int i = 0;
    while (iss>>flavour_value) {
        if(i >= 6) break; // Prevent overflow
        flavour[i++] = std::stoi(flavour_value);
    }
    if(i!=6){
        std::cerr << "Error: Item " << type << " has invalid number of flavours!" << std::endl;
        return;
    }
}

void ItemData::loadItems() {
    // Load items from file
    std::ifstream file(ITEMS_FILE);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << ITEMS_FILE << std::endl;
        return;
    }
    std::unordered_map<std::string,std::string> input_data;
    std::string line;
    int count = 1;
    while (std::getline(file, line)) {
        if(line.empty() || line[0] == '%') continue; // Skip empty lines and comments
        if(line[0] == '#') {
            if(input_data.empty()){continue;}
            ItemType key = static_cast<ItemType>(count);
            loadItem(key, input_data);
            input_data.clear();
            count++;
        }else if(line.rfind("NAME:",0)==0){
            input_data["name"] = line.substr(5);
        }else if(line.rfind("DESCRIPTION:",0)==0){
            input_data["description"] = line.substr(12);
        }else if(line.rfind("PRICE:",0)==0){        
            input_data["price"] = line.substr(6);
        }else if(line.rfind("CATEGORY:",0)==0){        
            input_data["category"] = line.substr(9);
        }else if(line.rfind("FLAVOUR:",0)==0){        
            input_data["flavour"] = line.substr(8);
        }else{
            std::cerr << "Error: Unknown line in item file: " << line << std::endl;
            continue;
        }
    }
    file.close();
    if(!input_data.empty()){
        ItemType key = static_cast<ItemType>(count);
        loadItem(key, input_data);
    }
}

ItemData* ItemData::getItemData(ItemType key){
    auto iter = itemTypeMap.find(key);
    if (iter == itemTypeMap.end())
        return nullptr;
    return iter->second;
}

bool canBeStolen(ItemType item_type){
    switch (item_type){
        default:
            return true;
    }
}

unsigned int flingPower(ItemType item_type){
    switch(item_type){
        case POTION:
        case SUPER_POTION:
        case HYPER_POTION:
        case MAX_POTION:
        case FULL_RESTORE:
        case ANTIDOTE:
        case AWAKENING:
        case PARALYZE_HEAL:
        case BURN_HEAL:
        case ICE_HEAL:
        case FULL_HEAL:
            return 30;
        default:
            return 10;
    }
}

bool canItemBeConsumed(ItemType item_type){
    if(item_type == NO_ITEM_TYPE)
        return false;
    ItemData* item_data = ItemData::getItemData(item_type);
    if(item_data->getCategory() == BERRY){
        return true;
    }
    switch(item_type){
        default:
            return false;
    }
}

unsigned int ItemData::getFlavour(unsigned int flavour_type)const{
    if(flavour_type > 5)
        return 0;
    return flavour[flavour_type];
}