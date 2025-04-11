#include "attack.h"
#include "species.h"
#include "monster.h"
#include "team.h"
#include "battle.h"
#include "textevents.h"

int main(int argc, char** argv) {
    Attack::loadAttacks();
    Species::loadSpecies();
    Monster* monster_p1 = Monster::generateRandomMonster(9,50);
    // for(int i=0;i<100;i++){
    //     monster_p1->forgetAttack(i);
    // }
    // monster_p1->learnAttack(MIMIC_ID);
    // monster_p1->learnAttack(226);
    // monster_p1->learnAttack(161);
    // monster_p1->printSummary();
    Monster* monster_p2 = Monster::generateRandomMonster(20,50);
    // monster_p2->printSummary();
    Monster* monster_p3 = Monster::generateRandomMonster(151,50);
    // monster_p3->printSummary();
    Monster* monster_o1 = Monster::generateRandomMonster(40,50);
    Monster* monster_o2 = Monster::generateRandomMonster(51,50);
    Monster* monster_o3 = Monster::generateRandomMonster(62,50);
    MonsterTeam* player_team = new MonsterTeam();
    player_team->addMonster(monster_p1);
    player_team->addMonster(monster_p2);
    player_team->addMonster(monster_p3);
    MonsterTeam* opponent_team = new MonsterTeam();
    opponent_team->addMonster(monster_o1);
    opponent_team->addMonster(monster_o2);
    opponent_team->addMonster(monster_o3);
    TextEventHandler* handler = new TextEventHandler();
    Battle* battle = new Battle(1,handler,player_team, opponent_team);
    battle->startBattle();
    // Species::printAllSummaries();
    // Attack::printAllSummaries();
    return 0;
}