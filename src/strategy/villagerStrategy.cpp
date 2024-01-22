#include "villagerStrategy.h"

namespace strategy
{
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap)
{
    const int maxX = distanceMap.size()-1;
    const int maxY = distanceMap[0].size()-1;
    x = (x < 0 ) ? 0 : x > maxX ? maxX : x;
    y = (y < 0 ) ? 0 : y > maxY ? maxY : y;
    int dist = distanceMap[x][y];
    bool update = false;
    // left
    if ( x > 0 && distanceMap[x-1][y] < dist )
    {
        dist = distanceMap[x-1][y];
        dir = CharacterState::CHAR_WALK_W;
        TraceLog(LOG_DEBUG, "%s, left dist: %d", __func__, dist);
        update = true;
    } // right
    if ( x < maxX && distanceMap[x+1][y] < dist )
    {
        dist = distanceMap[x+1][y];
        dir = CharacterState::CHAR_WALK_E;
        TraceLog(LOG_DEBUG, "%s, right dist: %d", __func__, dist);
        update = true;
    } // up
    if ( y > 0 && distanceMap[x][y-1] < dist )
    {
        dist = distanceMap[x][y-1];
        dir = CharacterState::CHAR_WALK_N;
        TraceLog(LOG_DEBUG, "%s, up dist: %d", __func__, dist);
        update = true;
    } // down
    if ( y < maxY && distanceMap[x][y+1] < dist )
    {
        dist = distanceMap[x][y+1];
        dir = CharacterState::CHAR_WALK_S;
        TraceLog(LOG_DEBUG, "%s, down dist: %d", __func__, dist);
        update = true;
    }
    if ( !update )
    {
        TraceLog(LOG_DEBUG, "%s did not update position - distance at %d, {%d, %d, %d, %d}",
            __func__, dist,
            distanceMap[x][y-1], distanceMap[x+1][y],
            distanceMap[x][y+1], distanceMap[x-1][y]);
    }
    return true;
}

bool idleCharacter(Character *character, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == character )
    {
        return false;
    }
    CharacterState newState = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x);
    int y = static_cast<int>(character->worldBounds.y);
    if ( distanceMaps.count(0) )
    {
        getNextState(x, y, newState, distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
    }
    TraceLog(LOG_DEBUG,"%s (%s) state %d -> %d", __func__, character->name,
     static_cast<int>(character->state), static_cast<int>(newState));
    if ( newState != CharacterState::CHAR_IDLE )
    {
        character->state = newState;
        character->sprite->animationState.activeAnimation = newState;
    }
    return true;
}

bool moveCharacter(Character *character, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == character )
    {
        return false;
    }
    CharacterState newState = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x);
    int y = static_cast<int>(character->worldBounds.y);
    if ( distanceMaps.count(0) )
    {
        getNextState(x, y, newState, distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
    }
    TraceLog(LOG_DEBUG,"%s (%s) state %d -> %d", __func__, character->name,
     static_cast<int>(character->state), static_cast<int>(newState));
    character->state = newState;
    character->sprite->animationState.activeAnimation = newState;
    return true;
}

} // namespace strategy