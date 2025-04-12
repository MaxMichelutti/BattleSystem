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
    if (category == "No Category") return NO_CATEGORY;
    else if (category == "Key Item") return KEY_ITEM;
    else if (category == "Berry") return BERRY;
    else if (category == "Ball") return BALL;
    else if (category == "Medicine") return MEDICINE;
    else if (category == "Evolution Item") return EVOLUTION_ITEM;
    else if (category == "TM") return TM;
    else if (category == "Held Item") return HELD_ITEM;
    else if (category == "General Item") return GENERAL_ITEM;
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