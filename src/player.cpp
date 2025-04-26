#include "player.h"

Player::Player(){
    name = "Player";
    team = new MonsterTeam();
    bag = new Bag();
    money = 0;
    event_handler = new TextEventHandler();
}
void Player::createPlayer(){
    if(global_player == nullptr){
        global_player = new Player();
    }
}
Player* Player::getPlayer(){
    if(global_player == nullptr){
        global_player = new Player();
    }
    return global_player;
}
Player::~Player(){
    delete team;
    delete bag;
}
void Player::setName(std::string name){
    this->name = name;
}
std::string Player::getName()const{
    return name;
}
MonsterTeam* Player::getTeam()const{
    return team;
}
Bag* Player::getBag()const{
    return bag;
}
void Player::addMoney(unsigned int money){
    this->money += money;
}
void Player::removeMoney(unsigned int money){
    if(hasMoney(money)){
        this->money -= money;
    }else{
        this->money = 0;
    }
}
bool Player::hasMoney(unsigned int money)const{
    return this->money >= money;
}

EventHandler* Player::getEventHandler()const{
    return event_handler;
}
void Player::setEventHandler(EventHandler* event_handler){
    if(event_handler == nullptr){
        std::cerr << "Error: Event handler is null!" << std::endl;
        exit(DATA_ERROR);
    }
    this->event_handler = event_handler;
}