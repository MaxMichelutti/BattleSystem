#ifndef __BAG_H__
#define __BAG_H__
#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <iostream>
#include "enums.h"
#include "items.h"
#include "constants.h"
#include "util.h"


class ItemData;


class Pocket{
    std::string name;
    std::map<ItemType,unsigned int> items;
    public:
    Pocket(std::string name);
    ~Pocket();
    bool isEmpty()const;
    void addItem(ItemType item,unsigned int amount);
    void removeItem(ItemType item,unsigned int amount);
    unsigned int getItemCount(ItemType item)const;
    std::map<ItemType, unsigned int> getItems()const;
    std::string getName()const;
    bool hasItem(ItemType item,unsigned int amount)const;
    bool hasItem(ItemType item)const;
    void removeAllItems(ItemType item);
};

class Bag{
    std::map<ItemCategory,Pocket*> pockets;
    public:
    Bag();
    ~Bag();
    void addItem(ItemType item,unsigned int amount);
    void addItem(ItemType item);
    void removeItem(ItemType item,unsigned int amount);
    void removeItem(ItemType item);
    void removeAllItems(ItemType item);
    bool hasItem(ItemType item,unsigned int amount)const;
    bool hasItem(ItemType item)const;
    unsigned int getItemCount(ItemType item)const;
    std::vector<ItemCategory> getPockets()const;
    std::map<ItemType, unsigned int> getItemsInPocket(ItemCategory category)const;
    std::string getPocketName(ItemCategory category)const;
    bool isEmpty()const;
    Pocket * getPocket(ItemCategory);
};

#endif