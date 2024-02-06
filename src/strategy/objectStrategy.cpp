#include "objectStrategy.h"
#include <iostream>
namespace strategy
{


bool idleObject(Character *obj, float delta, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    if ( distanceMaps[DistanceMapType::FIRE_DISTANCE][obj->worldBounds.x][obj->worldBounds.y] > 1 )
    {
        return false;
    }
    #if FASTBURN 
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
    #endif
    int countBurning = 0;
    int dirs[][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for ( int dx = -1; dx < 2; ++dx)
    {
        for( int dy = -1; dy < 2; ++dy)
        {
            int x = obj->worldBounds.x + dx;
            int y = obj->worldBounds.y + dy;
        if ( dx == 0 && dy == 0)
            continue;
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
    }
    if ( countBurning > 4 )
    {
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
}

bool burningObject(Character *obj, float delta, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    const float FIRE_DPS = 0.01;
    obj->stats.HP -= delta * FIRE_DPS;
    if ( obj->stats.HP <= 0 )
    {
        distanceMaps[DistanceMapType::FIRE_DISTANCE][obj->worldBounds.x][obj->worldBounds.y] = -1;
        std::cerr << "burned down " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y << "\n";
        
        return true;
    }
    return false;
}

bool startBurningObject(Character *obj, float delta, MappedDistanceMaps distanceMaps)
{
    if ( nullptr == obj )
    {
        return false;
    }
    if ( (obj->sprite->animationState.activeAnimation == CharacterState::CHAR_CATCH_FIRE )
        && (obj->sprite->animationState.currentLoop > 0) )
    {
        std::cerr << "NOW BURNING: " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y 
         << " currentLoop: " << obj->sprite->animationState.currentLoop << "\n";
        obj->sprite->animationState.activeAnimation = CharacterState::CHAR_BURNING;
        obj->sprite->animationState.activeFrame = 0;
        obj->sprite->animationState.currentLoop = 0;
        obj->sprite->animationState.frameDelta = 0;
        obj->state = CharacterState::CHAR_BURNING;
        return true;
    }

    std::cerr << "NOW still BURNING: " << obj->name << " at " << obj->worldBounds.x << " " << obj->worldBounds.y 
        << " currentLoop: " << obj->sprite->animationState.currentLoop << "\n";
    return false;
}
}
