#ifndef VILLAGER_STRATEGY_H
#define VILLAGER_STRATEGY_H
#include "character.h"
#include <vector>
#include <map>
namespace strategy
{
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap);

bool idleVillager(Character *character, MappedDistanceMaps distanceMaps);

bool moveVillager(Character *character, MappedDistanceMaps distanceMaps);

bool attackVillager(Character *character, MappedDistanceMaps distanceMaps);

bool idleGuard(Character *character, MappedDistanceMaps distanceMaps);

bool moveGuard(Character *character, MappedDistanceMaps distanceMaps);

bool attackGuard(Character *character, MappedDistanceMaps distanceMaps);

bool idleMage(Character *character, MappedDistanceMaps distanceMaps);

bool moveMage(Character *character, MappedDistanceMaps distanceMaps);

bool attackMage(Character *character, MappedDistanceMaps distanceMaps);

bool idleHero(Character *character, MappedDistanceMaps distanceMaps);

bool moveHero(Character *character, MappedDistanceMaps distanceMaps);

bool attackHero(Character *character, MappedDistanceMaps distanceMaps);

} // end namespace strategy
#endif // VILLAGER_STRATEGY_H