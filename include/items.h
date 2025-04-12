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

class ItemData;
static std::unordered_map<ItemType,ItemData*>  itemTypeMap;

class ItemData{
    ItemType type;
    ItemCategory category;
    std::string name;
    std::string description;
    int price;
    static void loadItem(ItemType key,const std::unordered_map<std::string,std::string> &input_data);
    public:
    ItemData(ItemType type, ItemCategory category, std::string name, std::string description, int price)
        : type(type), category(category), name(name), description(description), price(price) {}
    ItemType getType() const;
    ItemCategory getCategory()const;
    std::string getName() const;
    std::string getDescription() const;
    int getPrice() const;
    static void loadItems();
    static ItemData* getItemData(ItemType key);
};

#endif