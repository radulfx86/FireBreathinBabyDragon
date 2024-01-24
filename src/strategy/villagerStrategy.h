#ifndef VILLAGER_STRATEGY_H
#define VILLAGER_STRATEGY_H
#include "character.h"

namespace strategy
{
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap);

bool idleVillager(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool moveVillager(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool attackVillager(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool idleGuard(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool moveGuard(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool attackGuard(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool idleMage(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool moveMage(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool attackMage(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool idleHero(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool moveHero(Character *character, float delta, MappedDistanceMaps distanceMaps);

bool attackHero(Character *character, float delta, MappedDistanceMaps distanceMaps);

} // end namespace strategy
#endif // VILLAGER_STRATEGY_H
