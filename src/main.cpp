#include "attack.h"
#include "species.h"
#include "monster.h"
#include "team.h"
#include "battle.h"
#include "textevents.h"
#include "abilities.h"
#include "player.h"
#include "items.h"

int main(int argc, char** argv) {
    Player::createPlayer();
    Player* player = Player::getPlayer();
    player->setName("Player");
    player->addMoney(10000);
    Attack::loadAttacks();
    Species::loadSpecies();
    AbilityItem::initAbilities();
    ItemData::loadItems();
    Monster* monster_p1 = Monster::generateRandomMonster(493,50);
    monster_p1->setHeldItem(FLAME_PLATE);
    monster_p1->learnAttackForced(560,1);
    Monster* monster_p2 = Monster::generateRandomMonster(383,50);
    monster_p2->setHeldItem(RED_ORB);
    Monster* monster_p3 = Monster::generateRandomMonster(384,50);
    monster_p3->learnAttackForced(DRAGON_ASCENT_ID,1);
    Monster* monster_o1 = Monster::generateRandomMonster(351,50);
    Monster* monster_o2 = Monster::generateRandomMonster(382,50);
    monster_o2->setHeldItem(BLUE_ORB);
    Monster* monster_o3 = Monster::generateRandomMonster(384,50);
    monster_o3->learnAttackForced(DRAGON_ASCENT_ID,1);
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
    battle->setBattleGivesExp();
    battle->startBattle();
    // Species::printAllSummaries();
    // Attack::printAllSummaries();
    return 0;
}