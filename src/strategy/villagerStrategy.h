#ifndef VILLAGER_STRATEGY_H
#define VILLAGER_STRATEGY_H
#include "character.h"

namespace strategy
{
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap);

bool idleVillager(Character *character, float delta, Level *level);

bool moveVillager(Character *character, float delta, Level *level);

bool attackVillager(Character *character, float delta, Level *level);

bool idleGuard(Character *character, float delta, Level *level);

bool moveGuard(Character *character, float delta, Level *level);

bool attackGuard(Character *character, float delta, Level *level);

bool idleMage(Character *character, float delta, Level *level);

bool moveMage(Character *character, float delta, Level *level);

bool attackMage(Character *character, float delta, Level *level);

bool idleHero(Character *character, float delta, Level *level);

bool moveHero(Character *character, float delta, Level *level);

bool attackHero(Character *character, float delta, Level *level);

} // end namespace strategy
#endif // VILLAGER_STRATEGY_H
