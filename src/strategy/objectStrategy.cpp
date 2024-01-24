#include "objectStrategy.h"
#include <iostream>
namespace strategy
{


bool idleObject(Character *obj, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    if ( distanceMaps[DistanceMapType::FIRE_DISTANCE][obj->worldBounds.x][obj->worldBounds.y] > 1 )
    {
        return true;
    }
    
    if ( distanceMaps[DistanceMapType::FIRE_DISTANCE][obj->worldBounds.x][obj->worldBounds.y] == 1 )
    {
        /// FASTBURN
        /// BUUUUUUUUUUUUUUUUUUUUUUUUUUUUURN
        obj->state = CharacterState::CHAR_CATCH_FIRE;
        obj->sprite->animationState.activeAnimation = CharacterState::CHAR_CATCH_FIRE;
        obj->sprite->animationState.activeFrame = 0;
        obj->sprite->animationState.currentLoop = 0;
        obj->sprite->animationState.frameDelta = 0;
        std::cerr << "catching FIRE: " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y << "\n";
        return true;
    }
    return false;
    // cannot be reached - might be slow
    int countBurning = 0;
    int dirs[][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for ( int i = 0; i < 4; ++i )
    {
        int x = obj->worldBounds.x + dirs[i][0];
        int y = obj->worldBounds.y + dirs[i][1];
        if ( x < 0 || x >= distanceMaps[DistanceMapType::FIRE_DISTANCE].size()
            || y < 0 || y >= distanceMaps[DistanceMapType::FIRE_DISTANCE][0].size() )
        {
            continue;
        }
        if ( distanceMaps[DistanceMapType::FIRE_DISTANCE][x][y] == 0 )
        {
            ++countBurning;
        }
    }
    if ( countBurning > 2 )
    {
        /// BUUUUUUUUUUUUUUUUUUUUUUUUUUUUURN
        obj->state = CharacterState::CHAR_CATCH_FIRE;
        obj->sprite->animationState.activeAnimation = CharacterState::CHAR_CATCH_FIRE;
        obj->sprite->animationState.activeFrame = 0;
        obj->sprite->animationState.currentLoop = 0;
        obj->sprite->animationState.frameDelta = 0;
        std::cerr << "catching FIRE: " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y << "\n";
    }
    return true;
}

bool burningObject(Character *obj, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    return true;
}

bool startBurningObject(Character *obj, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    if ( (obj->sprite->animationState.activeAnimation == CharacterState::CHAR_CATCH_FIRE )
        && (obj->sprite->animationState.currentLoop > 0) )
    {
        obj->sprite->animationState.activeAnimation = CharacterState::CHAR_BURNING;
        obj->sprite->animationState.activeFrame = 0;
        obj->sprite->animationState.currentLoop = 0;
        obj->sprite->animationState.frameDelta = 0;
        obj->state = CharacterState::CHAR_BURNING;
        std::cerr << "NOW BURNING: " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y << "\n";
    }
    return true;
}
}