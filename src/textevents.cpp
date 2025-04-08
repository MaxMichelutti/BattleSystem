#include "textevents.h"

TextEventHandler::TextEventHandler() {}
TextEventHandler::~TextEventHandler() {}

BattleAction TextEventHandler::chooseAction(Battler* player_active,MonsterTeam* player_team, Battler* opponent_active, Field* field){
    BattleAction forced_action = forcedAction(PLAYER, player_active,field);
    if(forced_action.getActionType() != ATTACK){return forced_action;}
    unsigned int choice=0;
    while(choice!=1 && choice!=2){
        displayMsg("Choose your action:");
        displayMsg("1. Attack");
        displayMsg("2. Switch");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if(choice==1){
            if(player_active->hasVolatileCondition(ENCORE)){
                // force use of encored move
                unsigned int attack_id = player_active->getLastAttackUsed();
                Attack *attack = Attack::getAttack(attack_id);
                player_active->decrementVolatileCondition(ENCORE);
                return BattleAction(
                    PLAYER,
                    ATTACK,
                    attack_id,
                    attack->getPriorityLevel(), 
                    player_active->getModifiedSpeed(), 
                    0);
            }
            unsigned int attack_id = chooseAttack(player_active);
            if(attack_id == 0){
                choice = 0;
                continue;
            }
            Attack* attack = Attack::getAttack(attack_id);
            return BattleAction(
                PLAYER,
                ATTACK,
                attack_id, 
                attack->getPriorityLevel(), 
                player_active->getModifiedSpeed(), 
                0);
        }else if(choice==2){
            if(!player_active->canSwitchOut(opponent_active)){
                displayMsg("You cannot switch! "+player_active->getNickname()+" is trapped!");
                choice = 0;
                continue;
            }
            unsigned int switch_id = chooseSwitch(player_team);
            if(switch_id == 0){
                choice = 0;
                continue;
            }
            return BattleAction(
                PLAYER,
                SWITCH,
                0, 
                0, 
                player_active->getModifiedSpeed(), 
                switch_id);
        }else{
            displayMsg("Invalid choice. Please try again.");
        }
    }
}

unsigned int TextEventHandler::chooseAttack(Battler* player_active){
    auto available_attacks = player_active->getAttacks();
    bool has_usable_attack = false;
    int counter = 1;
    // collect choices
    std::map<unsigned int,std::pair<unsigned int, unsigned int>> choices;
    // check if at least one choice can be selected
    for(auto it = available_attacks.begin(); it != available_attacks.end(); it++){
        choices.insert({counter++,{it->first, it->second}});
        Attack * attack = Attack::getAttack(it->first);
        if(it->second > 0 && 
            !player_active->isAttackDisabled(it->first) &&
            (attack->getCategory()!= STATUS || !player_active->hasVolatileCondition(TAUNTED))){
            has_usable_attack = true;
        }
    }
    // force struggle if no choice can be selected
    if(!has_usable_attack){
        displayMsg("You have no usable attacks! Your monster will use Struggle!");
        return STRUGGLE_ID;
    }
    // print choices
    unsigned int choice = 0;
    // get choice
    while(choice < 1 || choice > choices.size() + 1){
        displayMsg("Available attacks: ");
        for(auto it = choices.begin(); it != choices.end(); it++){
            Attack* attack = Attack::getAttack(it->second.first);
            std::string msg = std::to_string(it->first) + ". " + 
                attack->getName() + "\t" + 
                std::to_string(it->second.second) + "/" + 
                std::to_string(player_active->getMaxPPForAttack(it->second.first));
            displayMsg(msg);
        }
        displayMsg(std::to_string(choices.size()+1) + ". Go Back");
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if(choice < 1 || choice > choices.size() + 1){
            displayMsg("Invalid choice. Please try again.");
        }else if (choice == choices.size()+1){
            return 0;
        }else if(player_active->isAttackDisabled(choices[choice].first)){
            displayMsg("this attack is currently disabled! Please choose another one.");
            choice = 0;
        }else{
            auto it = choices.find(choice);
            if(it != choices.end()){
                unsigned int attack_id = it->second.first;
                unsigned int current_pp = it->second.second;
                if(current_pp > 0){
                    return attack_id;
                }else{
                    displayMsg("You have no PP left for this attack! Please choose another one.");
                    choice = 0;
                }
            }
        }
    }
}

unsigned int TextEventHandler::chooseSwitch(MonsterTeam* player_team){
    auto available_monsters = player_team->getMonsters();
    // get choice
    unsigned int choice = 0;
    while(choice < 1 || choice > available_monsters.size() + 1){
        // print choices
        displayMsg("Available monsters:");
        for(unsigned int i=0;i<available_monsters.size();i++){
            int currentHP = available_monsters[i]->getCurrentHP();
            int maxHP = available_monsters[i]->getMaxHP();
            PermanentStatusCondition status = available_monsters[i]->getPermanentStatus();
            // std::cout<<i+1<<". "
            //     <<available_monsters[i]->getNickname()<<"\t"
            //     <<currentHP<<"/"<<maxHP<<"\t"
            //     <<permanentStatusConditionToString(status)<<std::endl;
            std::string msg = std::to_string(i+1) + ". " +
                available_monsters[i]->getNickname() + "\t" +
                std::to_string(currentHP) + "/" + 
                std::to_string(maxHP) + "\t" +
                permanentStatusConditionToString(status);
            displayMsg(msg);
        }
        displayMsg(std::to_string(available_monsters.size()+1) + ". Back");
        // get choice
        displayMsgNoEndl("Enter your choice: ");
        choice = getNumberFromCin();
        if(choice < 1 || choice > available_monsters.size() + 1){
            displayMsg("Invalid choice. Please try again.");
        }else if (choice == available_monsters.size() + 1){
            return 0;
        }else{
            if(choice==1){
                displayMsg("You cannot switch to the active monster!");
                choice = 0;
            }else{
                unsigned int result = choice - 1;
                Monster* switch_monster = available_monsters[result];
                if(switch_monster->isFainted()){
                    displayMsg("You cannot switch to a fainted monster!");
                    choice = 0;
                }else{
                    return result;
                }
            }
        }
    }
}

unsigned int TextEventHandler::chooseSwitchForced(MonsterTeam* player_team){
    auto available_monsters = player_team->getMonsters();
    unsigned int choice = 0;
    while(true){
        choice = chooseSwitch(player_team);
        if(choice == 0){
            displayMsg("You cannot go back!");
        }else{
            break;
        }
    }
    return choice;
}

void TextEventHandler::displayMsg(const std::string& msg){
    std::cout<<msg<<std::endl;
}

void TextEventHandler::displayMsgNoEndl(const std::string& msg){
    std::cout<<msg;
}

unsigned int TextEventHandler::getNumberFromCin(){
    std::string input;
    std::cin >> input;
    if(is_number(input)){
        return stringToInteger(input);
    }else{
        return 0;
    }
}

void TextEventHandler::displayBattleSituation(Battler* user_active,MonsterTeam*user_team,Battler*enemy_active,MonsterTeam*enemy_team){
    std::string user_name = user_active->getNickname();
    std::string enemy_name = enemy_active->getNickname();
    std::cout<<"Player:\t"<<user_name<<": "<<user_active->getCurrentHP()<<"/"<<user_active->getMaxHP()<<std::endl;
    auto monsters = user_team->getMonsters();
    unsigned int alive_amount = 0;
    unsigned int fainted_amount = 0;
    for(int i=0; i<monsters.size(); i++){
        Monster* mon = user_team->getMonster(i);
        if(mon->isFainted()){
            fainted_amount++;
        }else{
            alive_amount++;
        }
    }
    for(int a=0; a<alive_amount; a++){
        std::cout<<"O ";
    }
    for(int b=0; b<fainted_amount; b++){
        std::cout<<"X ";
    }
    std::cout<<std::endl;
    std::cout<<"Opp.:\t"<<enemy_name<<": "<<enemy_active->getCurrentHP()<<"/"<<enemy_active->getMaxHP()<<std::endl;
    monsters = enemy_team->getMonsters();
    alive_amount = 0;
    fainted_amount = 0;
    for(int i=0; i<monsters.size(); i++){
        Monster* mon = enemy_team->getMonster(i);
        if(mon->isFainted()){
            fainted_amount++;
        }else{
            alive_amount++;
        }
    }
    for(int a=0; a<alive_amount; a++){
        std::cout<<"O ";
    }
    for(int b=0; b<fainted_amount; b++){
        std::cout<<"X ";
    }
    std::cout<<std::endl;
}