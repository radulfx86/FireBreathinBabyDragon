#ifndef VILLAGER_STRATEGY_H
#define VILLAGER_STRATEGY_H
#include "character.h"
#include <vector>
#include <map>
namespace strategy
{
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap);

bool idleCharacter(Character *character, MappedDistanceMaps distanceMaps);

bool moveCharacter(Character *character, MappedDistanceMaps distanceMaps);


} // end namespace strategy
#endif // VILLAGER_STRATEGY_H