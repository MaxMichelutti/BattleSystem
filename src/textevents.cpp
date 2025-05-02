#include "textevents.h"

TextEventHandler::TextEventHandler() {}
TextEventHandler::~TextEventHandler() {}

BattleAction TextEventHandler::chooseAction(Battler *player_active, MonsterTeam *player_team, Battler *opponent_active, Field *field, Bag *bag, bool is_wild_battle){
    BattleAction forced_action = forcedAction(PLAYER, player_active, field);
    if (forced_action.getActionType() != ATTACK){
        return forced_action;
    }
    unsigned int choice = 0;
    while (true){
        displayMsg("Choose your action:");
        displayMsg("1. Attack");
        displayMsg("2. Switch");
        displayMsg("3. Item");
        displayMsg("4. Run");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if (choice == 1){
            if (player_active->hasVolatileCondition(ENCORE) ||
                ((player_active->hasHeldItem(CHOICE_BAND) || 
                    player_active->hasHeldItem(CHOICE_SCARF) ||
                    player_active->hasHeldItem(CHOICE_SPECS) ||
                    player_active->hasAbility(GORILLA_TACTICS)) && player_active->getLastAttackUsed() != 0)){
                // force use of encored move or choice band repeating move
                unsigned int attack_id = player_active->getLastAttackUsed();
                Attack *attack = Attack::getAttack(attack_id);
                if(player_active->hasVolatileCondition(ENCORE))
                    player_active->decrementVolatileCondition(ENCORE);
                return BattleAction(
                    PLAYER,
                    ATTACK,
                    attack_id,
                    attack->getPriorityLevel(),
                    player_active->getModifiedSpeed(),
                    0,
                    NO_ITEM_TYPE,
                    false,
                    player_active->getMonster());
            }
            unsigned int attack_id = chooseAttack(player_active);
            if (attack_id == 0){
                choice = 0;
                continue;
            }
            Attack *attack = Attack::getAttack(attack_id);
            bool mega_evolve = chooseWetherToMegaEvolve(player_active, player_team);
            return BattleAction(
                PLAYER,
                ATTACK,
                attack_id,
                attack->getPriorityLevel(),
                player_active->getModifiedSpeed(),
                0,
                NO_ITEM_TYPE,
                mega_evolve,
                player_active->getMonster());
        }else if (choice == 2){
            if (!player_active->canSwitchOut(opponent_active) && !player_active->hasHeldItem(SHED_SHELL)){
                displayMsg("You cannot switch! " + player_active->getNickname() + " is trapped!");
                choice = 0;
                continue;
            }
            unsigned int switch_id = chooseSwitch(player_team);
            if (switch_id == 0){
                choice = 0;
                continue;
            }
            return BattleAction(
                PLAYER,
                SWITCH,
                0,
                0,
                player_active->getModifiedSpeed(),
                switch_id,
                NO_ITEM_TYPE,
                false,
                player_active->getMonster());
        }else if (choice == 3){
            if (bag->isEmpty()){
                displayMsg("You have no items available to use!");
                choice = 0;
                continue;
            }
            auto item = chooseItem(bag, player_team, player_active,is_wild_battle);
            if (item.first == NO_ITEM_TYPE){
                choice = 0;
                continue;
            }
            unsigned int target_attack = 0;
            if(item.first == LEPPA_BERRY){
                target_attack = chooseTargetAttack(player_active,player_team,item.second);
                if(target_attack==0){
                    choice=0;
                    continue;
                }
            }
            return BattleAction(
                PLAYER,
                USE_ITEM,
                target_attack,
                0,
                player_active->getModifiedSpeed(),
                item.second,
                item.first,
                false,
                player_active->getMonster());
        }else if(choice == 4){//run
            if(!is_wild_battle){
                displayMsg("You cannot run away from a trainer battle!");
                choice = 0;
                continue;
            }
            if(!player_active->canSwitchOut(opponent_active)){
                displayMsg("You cannot run! " + player_active->getNickname() + " is trapped!");
                choice = 0;
                continue;
            }
            return BattleAction(
                PLAYER,
                ESCAPE,
                0,
                0,
                player_active->getModifiedSpeed(),
                0,
                NO_ITEM_TYPE,
                false,
                player_active->getMonster());
        }else{
            displayMsg("Invalid choice. Please try again.");
            choice = 0;
            continue;
        }
    }
}

unsigned int TextEventHandler::chooseTargetAttack(Battler*active,MonsterTeam*team,unsigned int target){
    std::map<unsigned int,unsigned int> attacks;
    if(target==0)
        attacks = active->getAttacks();
    else
        attacks = team->getMonster(target)->getAttacks();
    std::map<unsigned int,unsigned int> choices;
    unsigned int choice = 0;
    unsigned int count=1;
    for(auto attack:attacks){
        if(attack.first == 0 || attack.first == STRUGGLE_ID)
            continue;
        choices.insert({count,attack.first});
        count++;
    }
    while(true){
        displayMsg("Choose the Attack on which to use the item: ");
        for (auto it = choices.begin(); it != choices.end(); it++){
            Attack *attack = Attack::getAttack(it->second);
            std::string msg = std::to_string(it->first) + ". " +
                              attack->getName() + "\t" +
                              std::to_string(attacks[it->second]) + "/";
            std::string max_pp_string;
            if(target==0){
                max_pp_string = std::to_string(active->getMaxPPForAttack(it->second));
            }else{
                max_pp_string = std::to_string(team->getMonster(target)->getMaxPPForAttack(it->second));
            }              
            msg += max_pp_string;
            displayMsg(msg);
        }
        displayMsg(std::to_string(count++) + ". Info");
        displayMsg(std::to_string(count) + ". Cancel");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if(choice == count){
            return 0;
        }else if(choice == count-1){
            for(auto it = choices.begin(); it != choices.end(); it++){
                Attack *attack = Attack::getAttack(it->second);
                // std::string msg = std::to_string(it->first) + ". " +
                //                   attack->getName() + "\t" +
                //                   std::to_string(attacks[it->second]) + "/" +
                //                   std::to_string(active->getMaxPPForAttack(it->second));
                // displayMsg(msg);
                // displayMsg("Power: " + ((attack->getPower()==0)?"-":std::to_string(attack->getPower())));
                // displayMsg("Accuracy: " + ((attack->getAccuracy()==ALWAYS_HITS)?"-":std::to_string(attack->getAccuracy())));
                // displayMsg("Category: " + attackTypeToString(attack->getCategory()));
                // displayMsg(attack->getDescription());
                attack->printSummary(attacks[it->second]);
            }
            choice = 0;
            continue;
        }else if(choice == 0 || choice > count){
            displayMsg("Invalid choice. Please try again.");
            choice = 0;
            continue;
        }else{
            //check if attack has max amount of pp
            unsigned int maxPP;
            if(target == 0){
                maxPP = active->getMaxPPForAttack(choices[choice]);
            }else{
                maxPP = team->getMonster(target)->getMaxPPForAttack(choices[choice]);
            }
            if(maxPP <= attacks[choices[choice]]){
                displayMsg("This attack already has the maximum amount of PP! Please choose a different attack.");
                choice = 0;
                continue;
            }
            return choices[choice];
        }
    }
}

unsigned int TextEventHandler::chooseAttack(Battler *player_active){
    auto available_attacks = player_active->getAttacks();
    bool has_usable_attack = false;
    int counter = 1;
    // collect choices
    std::map<unsigned int, std::pair<unsigned int, unsigned int>> choices;
    // check if at least one choice can be selected
    for (auto it = available_attacks.begin(); it != available_attacks.end(); it++){
        choices.insert({counter++, {it->first, it->second}});
        Attack *attack = Attack::getAttack(it->first);
        if (it->second == 0)
            continue;
        if(player_active->isAttackDisabled(it->first))//disable check
            continue;
        if(attack->getCategory() == STATUS && (player_active->hasVolatileCondition(TAUNTED) || player_active->hasHeldItem(ASSULT_VEST)))//taunt check
            continue;
        if(player_active->hasVolatileCondition(TORMENTED) && player_active->getLastAttackUsed() == it->first){//cannot use the same move twice in a row when tormented
            continue;
        }
        has_usable_attack = true;
    }
    // force struggle if no choice can be selected
    if (!has_usable_attack){
        displayMsg("You have no usable attacks! Your monster will use Struggle!");
        return STRUGGLE_ID;
    }
    // print choices
    unsigned int choice = 0;
    // get choice
    while (true){
        displayMsg("Available attacks: ");
        for (auto it = choices.begin(); it != choices.end(); it++){
            Attack *attack = Attack::getAttack(it->second.first);
            std::string msg = std::to_string(it->first) + ". " +
                              attack->getName() + "\t" +
                              std::to_string(it->second.second) + "/" +
                              std::to_string(player_active->getMaxPPForAttack(it->second.first));
            displayMsg(msg);
        }
        displayMsg(std::to_string(choices.size() + 1) + ". Info");
        displayMsg(std::to_string(choices.size() + 2) + ". Go Back");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if (choice < 1 || choice > choices.size() + 2){
            displayMsg("Invalid choice. Please try again.");
            choice = 0;
        }else if (choice == choices.size() + 2){
            return 0;
        }else if(choice == choices.size()+1){
            for(auto it = choices.begin(); it != choices.end(); it++){
                Attack *attack = Attack::getAttack(it->second.first);
                attack->printSummary(it->second.second);
                // displayMsg(attack->getDescription());
            }
            choice = 0;
            continue;
        }else if (player_active->isAttackDisabled(choices[choice].first)){//disable check
            displayMsg("This attack is currently disabled! Please choose a different attack.");
            choice = 0;
            continue;
        }else{
            auto it = choices.find(choice);
            if (it != choices.end()){
                unsigned int attack_id = it->second.first;
                unsigned int current_pp = it->second.second;
                Attack *attack = Attack::getAttack(attack_id);
                if(attack->getCategory() == STATUS && player_active->hasVolatileCondition(TAUNTED)){
                    displayMsg("You cannot use a status move while taunted! Please choose a different attack.");
                    choice = 0;
                }else if(attack->getCategory() == STATUS && player_active->hasHeldItem(ASSULT_VEST)){
                    displayMsg("Assault Vest prevents the use of Status moves! Please choose a different attack.");
                    choice = 0;
                }else if(player_active->hasVolatileCondition(TORMENTED) && player_active->getLastAttackUsed()==attack_id){//torment
                    displayMsg("Torment does not allow your Pokèmon to use the same attack twice in a row! Please choose a different attack.");
                    choice = 0;
                }else if (current_pp == 0){
                    displayMsg("there is no PP left for this attack! Please choose a different attack.");
                    choice = 0;
                }else{
                    return attack_id;
                }
            }else{
                choice = 0;
            }
        }
    }
}

unsigned int TextEventHandler::chooseSwitch(MonsterTeam *player_team)
{
    auto available_monsters = player_team->getMonsters();
    // get choice
    unsigned int choice = 0;
    while (true){
        // print choices
        displayMsg("Available monsters:");
        for (unsigned int i = 0; i < available_monsters.size(); i++){
            int currentHP = available_monsters[i]->getCurrentHP();
            int maxHP = available_monsters[i]->getMaxHP();
            PermanentStatusCondition status = available_monsters[i]->getPermanentStatus();
            // std::cout<<i+1<<". "
            //     <<available_monsters[i]->getNickname()<<"\t"
            //     <<currentHP<<"/"<<maxHP<<"\t"
            //     <<permanentStatusConditionToString(status)<<std::endl;
            std::string msg = std::to_string(i + 1) + ". " +
                              available_monsters[i]->getNickname() + "\t" +
                              std::to_string(currentHP) + "/" +
                              std::to_string(maxHP) + "\t" +
                              permanentStatusConditionToString(status);
            displayMsg(msg);
        }
        displayMsg(std::to_string(available_monsters.size() + 1) + ". Info");
        displayMsg(std::to_string(available_monsters.size() + 2) + ". Back");
        // get choice
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if (choice < 1 || choice > available_monsters.size() + 2){
            displayMsg("Invalid choice. Please try again.");
        }else if (choice == available_monsters.size() + 2){
            return 0;
        }else if(choice == available_monsters.size()+1){
            for(unsigned int i=0;i<player_team->getSize();i++){
                player_team->getMonster(i)->printSummary();
            }
            choice = 0;
            continue;
        }else{
            if (choice == 1){
                displayMsg("You cannot switch to the active monster!");
                choice = 0;
            }else{
                unsigned int result = choice - 1;
                Monster *switch_monster = available_monsters[result];
                if (switch_monster->isFainted()){
                    displayMsg("You cannot switch to a fainted monster!");
                    choice = 0;
                }else{
                    return result;
                }
            }
        }
    }
}

unsigned int TextEventHandler::chooseSwitchForced(MonsterTeam *player_team){
    auto available_monsters = player_team->getMonsters();
    unsigned int choice = 0;
    while (true){
        choice = chooseSwitch(player_team);
        if (choice == 0){
            displayMsg("You cannot go back!");
        }else{
            break;
        }
    }
    return choice;
}

void TextEventHandler::displayMsg(const std::string &msg){
    std::cout << msg << std::endl;
}

void TextEventHandler::displayMsgNoEndl(const std::string &msg){
    std::cout << msg;
}

unsigned int TextEventHandler::getNumberFromCin(){
    std::string input;
    std::cin >> input;
    if (is_number(input)){
        return stringToInteger(input);
    }else{
        return 0;
    }
}

void TextEventHandler::displayBattleSituation(Battler *user_active, MonsterTeam *user_team, Battler *enemy_active, MonsterTeam *enemy_team){
    std::string user_name = user_active->getNickname();
    std::string enemy_name = enemy_active->getNickname();
    std::cout << "Player:\t" << user_name << ": "
              << user_active->getCurrentHP() << "/" << user_active->getMaxHP() << "\t"
              << permanentStatusConditionToString(user_active->getPermanentStatus())
              << std::endl;
    auto monsters = user_team->getMonsters();
    unsigned int alive_amount = 0;
    unsigned int fainted_amount = 0;
    for (unsigned int i = 0; i < monsters.size(); i++){
        Monster *mon = user_team->getMonster(i);
        if (mon->isFainted()){
            fainted_amount++;
        }else{
            alive_amount++;
        }
    }
    for (unsigned int a = 0; a < alive_amount; a++){
        std::cout << "O ";
    }
    for (unsigned int b = 0; b < fainted_amount; b++){
        std::cout << "X ";
    }
    std::cout << std::endl;
    std::cout << "Opp.:\t" << enemy_name << ": "
              << enemy_active->getCurrentHP() << "/" << enemy_active->getMaxHP() << "\t"
              << permanentStatusConditionToString(enemy_active->getPermanentStatus())
              << std::endl;
    monsters = enemy_team->getMonsters();
    alive_amount = 0;
    fainted_amount = 0;
    for (unsigned int i = 0; i < monsters.size(); i++){
        Monster *mon = enemy_team->getMonster(i);
        if (mon->isFainted()){
            fainted_amount++;
        }else{
            alive_amount++;
        }
    }
    for (unsigned int a = 0; a < alive_amount; a++){
        std::cout << "O ";
    }
    for (unsigned int b = 0; b < fainted_amount; b++){
        std::cout << "X ";
    }
    std::cout << std::endl;
}

std::pair<ItemType,unsigned int> TextEventHandler::chooseItem(Bag *bag, MonsterTeam* team,Battler* active_monster,bool is_wild_battle){
    if (bag->isEmpty())
        return std::make_pair(NO_ITEM_TYPE,0);
    auto pockets = bag->getPockets();
    unsigned int pocket_choice = 0;
    std::map<unsigned int, ItemCategory> pocket_choices;
    unsigned int count = 1;
    for (auto pocket : pockets)
        pocket_choices.insert({count++, pocket});
    while (true){
        displayMsg("Choose the pocket:");
        for (auto it : pocket_choices){
            displayMsg(std::to_string(it.first) + ". " + ItemCategoryToString(it.second));
        }
        displayMsg(std::to_string(count) + ". Back");
        displayMsgNoEndl("Enter your choice:");
        pocket_choice = getNumberFromCin();
        if (pocket_choice == count){
            return std::make_pair(NO_ITEM_TYPE,0);
        }else if (pocket_choices.find(pocket_choice) == pocket_choices.end()){
            pocket_choice = 0;
            displayMsg("Invalid pocket!");
            continue;
        }else{
            ItemCategory category = pocket_choices[pocket_choice];
            auto result = chooseItemFromPocket(bag->getPocket(category),team, active_monster, is_wild_battle);
            if(result.first == NO_ITEM_TYPE){//means "go back"
                pocket_choice = 0;
                continue;
            }
            return result;
        }
    }
}

std::pair<ItemType,unsigned int> TextEventHandler::chooseItemFromPocket(Pocket * pocket,MonsterTeam* team,Battler* active_monster,bool is_wild_battle){
    if(pocket->isEmpty())
        return std::make_pair(NO_ITEM_TYPE,0);
    auto items = pocket->getItems();
    unsigned int item_choice = 0;
    std::map<unsigned int, ItemType> item_choices;
    unsigned int count = 1;
    for (auto item : items)
        item_choices.insert({count++, item.first});
    while(true){
        displayMsg("Select the item to use:");
        for(auto it: item_choices){
            ItemData* item_data = ItemData::getItemData(it.second);
            if(item_data==nullptr)// return to avoid seg faults
                return std::make_pair(NO_ITEM_TYPE,0);
            unsigned int quantity = pocket->getItemCount(it.second);
            displayMsg(std::to_string(it.first)+". "+item_data->getName()+"(x"+std::to_string(quantity)+")");
        }
        displayMsg(std::to_string(count++) + ". Info");
        displayMsg(std::to_string(count) + ". Back");
        displayMsgNoEndl("Enter your choice:");
        item_choice = getNumberFromCin();
        if(item_choice==count){
            return std::make_pair(NO_ITEM_TYPE,0);
        }else if(item_choice==count-1){
            for(auto item:item_choices){
                if(item.second==NO_ITEM_TYPE)
                    continue;
                ItemData* item_data = ItemData::getItemData(item.second);
                displayMsg(item_data->getName()+":\t"+item_data->getDescription());
            }
            item_choice=0;
            continue;
        }else if(item_choices.find(item_choice)==item_choices.end()){
            item_choice = 0;
            displayMsg("Invalid item!");
            continue;
        }else{
            ItemType res = item_choices[item_choice];
            ItemData* item_data = ItemData::getItemData(res);
            unsigned int target;
            ItemCategory item_category = item_data->getCategory();
            if(item_category == BERRY || item_category==MEDICINE){//items used on a monster
                target = chooseItemTarget(res,team,active_monster);
                if(target == 10){//10 equals go back, 0 equals active monster
                    item_choice = 0;
                    continue;
                }
            }else if(item_category==BALL){// items used in wild battles on opponent
                if(!is_wild_battle){
                    displayMsg("You cannot use a ball in a trainer battle!");
                    item_choice = 0;
                    continue;
                }
                target = 0;
            }else{//unusable items in battle
                displayMsg("You cannot use this item in battle!");
                item_choice = 0;
                continue;
            }
            return std::make_pair(res,target);
        }
    }
}

unsigned int TextEventHandler::chooseItemTarget(ItemType item_type,MonsterTeam* team,Battler* active_monster){
    // returns 10 if go back
    if(team->isEmpty())
        return 10;
    if(item_type == NO_ITEM_TYPE)
        return 10;
    unsigned int team_size = team->getSize();
    unsigned int choice = 0;
    while(true){
        displayMsg("Select the target:");
        for(unsigned int i = 0; i < team_size; i++){
            Monster* mon = team->getMonster(i);
            // if(mon->isFainted()){
            //     continue;
            // }
            std::string msg = std::to_string(i+1)+". "+mon->getNickname();
            displayMsg(msg);
        }
        displayMsg(std::to_string(team_size+1)+". Back");
        displayMsgNoEndl("Enter your choice:");
        choice = getNumberFromCin();
        if(choice == team_size+1){//choice is go back
            return 10;
        }else if(choice < 1 || choice > team_size+1){
            choice = 0;
            displayMsg("Invalid choice! Please choose again!");
            continue;
        }else{
            unsigned int result = choice-1;
            if(result == 0){//use on active
                Battler* target = active_monster;
                if(target->itemWouldHaveEffect(item_type)){
                    return result;
                }else{
                    displayMsg("The item would have no effect on " + target->getNickname() + "!");
                    displayMsg("Please choose another target!");
                    choice = 0;
                    continue;
                }
            }else{//use on bench
                Monster* target = team->getMonster(result);
                if(target->itemWouldHaveEffect(item_type)){
                    return result;
                }else{
                    displayMsg("The item would have no effect on " + target->getNickname() + "!");
                    displayMsg("Please choose another target!");
                    choice = 0;
                    continue;
                }
            }
        }
    }
}

bool TextEventHandler::chooseWetherToMegaEvolve(Battler* active,MonsterTeam*team){
    if(!active->canMegaEvolve() || team->hasMega()){
        return false;
    }
    std::string msg = "Do you want to mega evolve " + active->getNickname() + "? (y/n)";
    while(true){
        displayMsgNoEndl(msg);
        std::string input;
        std::cin >> input;
        if (input == "y" || input == "Y"){
            return true;
        }else if (input == "n" || input == "N"){
            return false;
        }else{
            displayMsg("Invalid choice. Please enter 'y' or 'n'.");
            continue;
        }
    }
}

unsigned int TextEventHandler::chooseRotomForm(){
    unsigned int choice = 0;
    while (true){
        displayMsg("Choose the appliance where Rotom should enter:");
        displayMsg("1. Light Bulb");
        displayMsg("2. Microwave Oven");
        displayMsg("3. Washing Machine");
        displayMsg("4. Refrigerator");
        displayMsg("5. Electric Fan");
        displayMsg("6. Lawn Mower");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if (choice < 1 || choice > 6){
            displayMsg("Invalid choice. Please try again.");
            continue;
        }else{
            return choice;
        }
    }
}

void TextEventHandler::displayDmgDealt(unsigned int dmg, Battler* target){
    std::string msg = target->getNickname() + " took " + std::to_string(dmg) + " damage!";
    std::cout << msg << std::endl;
}

void TextEventHandler::displayDmgDealt(unsigned int dmg, Battler* target,std::string reason){
    std::string msg = target->getNickname() + " took " + std::to_string(dmg) + " damage from " + reason+ "!";
    std::cout << msg << std::endl;
}