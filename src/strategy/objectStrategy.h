#ifndef OBJECT_STRATEGY_H
#define OBJECT_STRATEGY_H
#include "character.h"
namespace strategy
{

bool idleObject(Character *obj, MappedDistanceMaps distanceMaps);

bool burningObject(Character *obj, MappedDistanceMaps distanceMaps);

bool startBurningObject(Character *obj, MappedDistanceMaps distanceMaps);

} // end namespace strategy
#endif // OBJECT_STRATEGY_H