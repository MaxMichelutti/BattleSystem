#include "bag.h"

// Pocket ------------------------------------------------------------------------------------------------------------
Pocket::Pocket(std::string name) : name(name) {
    // Initialize the pocket with the given name
}
Pocket::~Pocket() {
    // Destructor
}
bool Pocket::isEmpty() const {
    return items.empty();
}
void Pocket::addItem(ItemType item, unsigned int amount) {
    if (amount == 0) return;
    if(items.find(item)==items.end()){
        items.insert({item,amount});
    }else{
        items[item]+=amount;
    }
}
void Pocket::removeItem(ItemType item, unsigned int amount) {
    if (amount == 0) return;
    if(items.find(item)==items.end())
        return;
    unsigned int old_amount = items[item];
    amount = min(amount,old_amount);
    items[item]-=amount;
    if(items[item]==0)
        items.erase(item);
}
unsigned int Pocket::getItemCount(ItemType item) const {
    if(items.find(item)==items.end())
        return 0;
    return items.at(item);
}
std::map<ItemType, unsigned int> Pocket::getItems() const {
    return items;
}
std::string Pocket::getName() const {
    return name;
}
bool Pocket::hasItem(ItemType item, unsigned int amount) const {
    for (const auto& it : items) {
        if (it.first == item && it.second >= amount) {
            return true;
        }
    }
    return false;
}
bool Pocket::hasItem(ItemType item) const {
    for (const auto& it : items) {
        if (it.first == item) {
            return true;
        }
    }
    return false;
}

void Pocket::removeAllItems(ItemType item) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->first == item) {
            items.erase(it);
            return;
        }
    }
}

// Bag --------------------------------------------------------------------------------------------------------------
Bag::Bag(){
    pockets = std::map<ItemCategory,Pocket*>();
}
Bag::~Bag(){
    for(auto& pocket : pockets){
        delete pocket.second;
    }
}
void Bag::addItem(ItemType item, unsigned int amount) {
    if (amount == 0) return;
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) == pockets.end()) {
        pockets[category] = new Pocket(ItemCategoryToString(category)+"s");
    }
    pockets[category]->addItem(item, amount);
}
void Bag::addItem(ItemType item) {
    addItem(item, 1);
}
void Bag::removeItem(ItemType item, unsigned int amount) {
    if (amount == 0) return;
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) != pockets.end()) {
        pockets[category]->removeItem(item, amount);
        if (pockets[category]->isEmpty()) {
            delete pockets[category];
            pockets.erase(category);
        }
    }
}
void Bag::removeItem(ItemType item) {
    removeItem(item, 1);
}
unsigned int Bag::getItemCount(ItemType item) const {
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return 0;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) != pockets.end()){
        return pockets.at(category)->getItemCount(item);
    }
    return 0;
}
void Bag::removeAllItems(ItemType item) {
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) != pockets.end()) {
        pockets[category]->removeAllItems(item);
        if (pockets[category]->isEmpty()) {
            delete pockets[category];
            pockets.erase(category);
        }
    }
}

bool Bag::hasItem(ItemType item, unsigned int amount) const {
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return false;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) != pockets.end()) {
        return pockets.at(category)->hasItem(item, amount);
    }
    return false;
}
bool Bag::hasItem(ItemType item) const {
    ItemData* item_data = ItemData::getItemData(item);
    if (item_data == nullptr) return false;
    ItemCategory category = item_data->getCategory();
    if (pockets.find(category) != pockets.end()) {
        return pockets.at(category)->hasItem(item);
    }
    return false;
}

std::vector<ItemCategory> Bag::getPockets()const{
    std::vector<ItemCategory> pocket_list;
    for (const auto& pocket : pockets) {
        pocket_list.push_back(pocket.first);
    }
    return pocket_list;
}
std::map<ItemType, unsigned int> Bag::getItemsInPocket(ItemCategory category)const{
    std::map<ItemType, unsigned int> items_in_pocket;
    if (pockets.find(category) != pockets.end()) {
        items_in_pocket = pockets.at(category)->getItems();
    }
    return items_in_pocket;
}
std::string Bag::getPocketName(ItemCategory category)const{
    if (pockets.find(category) != pockets.end()) {
        return pockets.at(category)->getName();
    }
    return "NO_POCKET";
}
bool Bag::isEmpty()const{
    return pockets.empty();
}

Pocket* Bag::getPocket(ItemCategory cat){
    if(pockets.find(cat) == pockets.end()){
        return nullptr;
    }
    return pockets[cat];
}

