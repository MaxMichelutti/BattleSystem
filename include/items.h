#ifndef __ITEMS_H__
#define __ITEMS_H__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "enums.h"
#include "constants.h"
#include <unordered_map>

std::string ItemCategoryToString(ItemCategory category);
ItemCategory StringToItemCategory(std::string category);
bool canBeStolen(ItemType item_type);
unsigned int flingPower(ItemType item_type);
bool canItemBeConsumed(ItemType item_type);
bool isDrive(ItemType item_type);

class ItemData;
static std::unordered_map<ItemType,ItemData*>  itemTypeMap;

class ItemData{
    ItemType type;
    ItemCategory category;
    std::string name;
    std::string description;
    unsigned int flavour[6];
    int price;
    static void loadItem(ItemType key,const std::unordered_map<std::string,std::string> &input_data);
    public:
    ItemData(ItemType type, ItemCategory category, std::string name, std::string description, int price)
        : type(type), category(category), name(name), description(description), price(price) {}
    ItemType getType() const;
    ItemCategory getCategory()const;
    std::string getName() const;
    std::string getDescription() const;
    void setFlavours(std::string);
    int getPrice() const;
    static void loadItems();
    static ItemData* getItemData(ItemType key);
    unsigned int getFlavour(unsigned int flavbour_type)const;
};

#endif