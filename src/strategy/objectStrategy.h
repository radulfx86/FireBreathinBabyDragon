#ifndef OBJECT_STRATEGY_H
#define OBJECT_STRATEGY_H
#include "character.h"
namespace strategy
{

bool idleObject(Character *obj, float delta, MappedDistanceMaps distanceMaps);

bool burningObject(Character *obj, float delta, MappedDistanceMaps distanceMaps);

bool startBurningObject(Character *obj, float delta, MappedDistanceMaps distanceMaps);

} // end namespace strategy
#endif // OBJECT_STRATEGY_H
