#include "attack.h"
#include "species.h"
#include "monster.h"
#include "team.h"
#include "battle.h"
#include "textevents.h"
#include "abilities.h"
#include "items.h"

int main(int argc, char** argv) {
    Attack::loadAttacks();
    Species::loadSpecies();
    AbilityItem::initAbilities();
    ItemData::loadItems();
    Monster* monster_p1 = Monster::generateRandomMonster(144,50,51);
    // monster_p1->setHeldItem(KANGASKHANTITE);
    // for(int i=0;i<100;i++){
    //     monster_p1->forgetAttack(i);
    // }
    // monster_p1->learnAttack(MIMIC_ID);
    // monster_p1->learnAttack(226);
    // monster_p1->learnAttack(161);
    // monster_p1->printSummary();
    Monster* monster_p2 = Monster::generateRandomMonster(145,50,52);
    // monster_p2->printSummary();
    Monster* monster_p3 = Monster::generateRandomMonster(146,50,53);
    // monster_p3->printSummary();
    Monster* monster_o1 = Monster::generateRandomMonster(144,50);
    // monster_o1->setHeldItem(PINSIRITE);
    Monster* monster_o2 = Monster::generateRandomMonster(145,50);
    Monster* monster_o3 = Monster::generateRandomMonster(146,50);
    MonsterTeam* player_team = new MonsterTeam();
    player_team->addMonster(monster_p1);
    player_team->addMonster(monster_p2);
    player_team->addMonster(monster_p3);
    MonsterTeam* opponent_team = new MonsterTeam();
    opponent_team->addMonster(monster_o1);
    opponent_team->addMonster(monster_o2);
    opponent_team->addMonster(monster_o3);
    TextEventHandler* handler = new TextEventHandler();
    Bag * user_bag = new Bag();
    user_bag->addItem(POTION,5);
    user_bag->addItem(SUPER_POTION,3);
    user_bag->addItem(POTION,1);
    user_bag->addItem(LEPPA_BERRY,5);
    Bag * opponent_bag = new Bag();
    opponent_bag->addItem(HYPER_POTION,1);
    Battle* battle = new Battle(1,handler,player_team, opponent_team, user_bag, opponent_bag);
    battle->startBattle();
    // Species::printAllSummaries();
    // Attack::printAllSummaries();
    return 0;
}