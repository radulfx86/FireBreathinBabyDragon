#ifndef OBJECT_STRATEGY_H
#define OBJECT_STRATEGY_H
#include "character.h"
namespace strategy
{

bool idleObject(Character *obj, float delta, Level *level);

bool burningObject(Character *obj, float delta, Level *level);

bool startBurningObject(Character *obj, float delta, Level *level);

} // end namespace strategy
#endif // OBJECT_STRATEGY_H
