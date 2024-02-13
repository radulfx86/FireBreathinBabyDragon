#include "villagerStrategy.h"
#include "levelScreen.h"
#include "datastore.h"
#include "level.h"
#include <cmath>

namespace strategy
{

GridPos getNextGridPos(GridPos pos, DistanceMap distanceMap, bool isWalk)
{
    GridPos dir = {0,0};
    GridPos zero = {0,0};
    GridPos mapSize = {distanceMap.size()-1, distanceMap[0].size()-1};
    int dist = distanceMap[pos.x][pos.y];
    for ( int dx = -1; dx < 2; ++dx )
    {
        for ( int dy = -1; dy < 2; ++dy )
        {
            if ( isWalk && (abs(dx) + abs(dy)) > 1)
            {
                continue;
            }
            GridPos test = pos + (GridPos){dx, dy};
            int tdist = distanceMap[test.x][test.y];
            if ( isInside(test, zero, mapSize) && tdist < dist )
            {
                dist = tdist;
                dir = test;
            }
        }
    }
    return dir;
}

Direction getNextDir(GridPos pos, DistanceMap distanceMap)
{
    const int maxX = distanceMap.size()-1;
    const int maxY = distanceMap[0].size()-1;
    pos.x = (pos.x < 0 ) ? 0 : pos.x > maxX ? maxX : pos.x;
    pos.y = (pos.y < 0 ) ? 0 : pos.y > maxY ? maxY : pos.y;
    int dist = distanceMap[pos.x][pos.y];
    Direction dir = Direction::DIR_NONE;
    // left
    if ( pos.x > 0 && distanceMap[pos.x-1][pos.y] >= 0 && distanceMap[pos.x-1][pos.y] < dist )
    {
        dist = distanceMap[pos.x-1][pos.y];
        dir = Direction::DIR_W;
    } // right
    if ( pos.x < maxX && distanceMap[pos.x+1][pos.y] >= 0 && distanceMap[pos.x+1][pos.y] < dist )
    {
        dist = distanceMap[pos.x+1][pos.y];
        dir = Direction::DIR_W;
    } // up
    if ( pos.y > 0 && distanceMap[pos.x][pos.y-1] >= 0 && distanceMap[pos.x][pos.y-1] < dist )
    {
        dist = distanceMap[pos.x][pos.y-1];
        dir = Direction::DIR_W;
    } // down
    if ( pos.y < maxY && distanceMap[pos.x][pos.y+1] >= 0 && distanceMap[pos.x][pos.y+1] < dist )
    {
        dist = distanceMap[pos.x][pos.y+1];
        dir = Direction::DIR_W;
    }
    return dir;
}
bool getNextState(int &x, int &y, CharacterState &dir, DistanceMap distanceMap)
{
    const int maxX = distanceMap.size()-1;
    const int maxY = distanceMap[0].size()-1;
    x = (x < 0 ) ? 0 : x > maxX ? maxX : x;
    y = (y < 0 ) ? 0 : y > maxY ? maxY : y;
    int dist = distanceMap[x][y];
    bool update = false;
    // left
    if ( x > 0 && distanceMap[x-1][y] >= 0 && distanceMap[x-1][y] < dist )
    {
        dist = distanceMap[x-1][y];
        dir = CharacterState::CHAR_WALK_W;
        TraceLog(LOG_DEBUG, "%s, left dist: %d", __func__, dist);
        update = true;
    } // right
    if ( x < maxX && distanceMap[x+1][y] >= 0 && distanceMap[x+1][y] < dist )
    {
        dist = distanceMap[x+1][y];
        dir = CharacterState::CHAR_WALK_E;
        TraceLog(LOG_DEBUG, "%s, right dist: %d", __func__, dist);
        update = true;
    } // up
    if ( y > 0 && distanceMap[x][y-1] >= 0 && distanceMap[x][y-1] < dist )
    {
        dist = distanceMap[x][y-1];
        dir = CharacterState::CHAR_WALK_N;
        TraceLog(LOG_DEBUG, "%s, up dist: %d", __func__, dist);
        update = true;
    } // down
    if ( y < maxY && distanceMap[x][y+1] >= 0 && distanceMap[x][y+1] < dist )
    {
        dist = distanceMap[x][y+1];
        dir = CharacterState::CHAR_WALK_S;
        TraceLog(LOG_DEBUG, "%s, down dist: %d", __func__, dist);
        update = true;
    }
    if ( !update )
    {
        dir = CharacterState::CHAR_IDLE;
        TraceLog(LOG_DEBUG, "%s did not update position - distance at %d, {%d, %d, %d, %d}",
            __func__, dist,
            distanceMap[x][y-1], distanceMap[x+1][y],
            distanceMap[x][y+1], distanceMap[x-1][y]);
    }
    return true;
}

bool idleVillager(Character *character, float delta, Level *level)
{
    if ( nullptr == character )
    {
        return false;
    }
    CharacterState newState = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x+0.5f);
    int y = static_cast<int>(character->worldBounds.y+0.5f);
    if ( level->distanceMaps.count(0) )
    {
        getNextState(x, y, newState, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
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

bool moveVillager(Character *character, float delta, Level *level)
{
    if ( nullptr == character )
    {
        return false;
    }
    CharacterState newState = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x+.5f);
    int y = static_cast<int>(character->worldBounds.y+.5f);
    if ( level->distanceMaps.count(0) )
    {
        getNextState(x, y, newState, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
    }
    TraceLog(LOG_DEBUG,"%s (%s) state %d -> %d", __func__, character->name,
     static_cast<int>(character->state), static_cast<int>(newState));
    character->state = newState;
    character->sprite->animationState.activeAnimation = newState;
    return true;
}

bool attackVillager(Character *character, float delta, Level *level)
{
    return moveVillager(character, delta, level);
}

bool idleGuard(Character *character, float delta, Level *level)
{
    const float GUARD_ENGAGE_DIST = 2.0f;

    if ( nullptr == character )
    {
        return false;
    }
    CharacterState newState = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x+.5f);
    int y = static_cast<int>(character->worldBounds.y+.5f);
    int playerDist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
    if ((playerDist >= 0) && (playerDist < GUARD_ENGAGE_DIST )
        && (character->stats.EP > 0 ))
    {
        getNextState(x, y, newState, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
        switch (newState)
        {
            case CharacterState::CHAR_WALK_E:
                newState = CharacterState::CHAR_ATTACK_E;
                break;
            case CharacterState::CHAR_WALK_W:
                newState = CharacterState::CHAR_ATTACK_W;
                break;
            case CharacterState::CHAR_WALK_N:
                newState = CharacterState::CHAR_ATTACK_N;
                break;
            case CharacterState::CHAR_WALK_S:
                newState = CharacterState::CHAR_ATTACK_S;
                break;
            default:
                break;
        }
    }
    else
    {
        getNextState(x, y, newState, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
    }
    TraceLog(LOG_DEBUG,"%s (%s) state %d -> %d", __func__, character->name,
     static_cast<int>(character->state), static_cast<int>(newState));
    character->state = newState;
    character->sprite->animationState.activeAnimation = newState;
    return true;
}

bool moveGuard(Character *character, float delta, Level *level)
{
    return idleGuard(character, delta, level);
}

bool attackGuard(Character *character, float delta, Level *level)
{
    const float GUARD_ENGAGE_DIST = 2.0f;
    int x = static_cast<int>(character->worldBounds.x+0.5f);
    int y = static_cast<int>(character->worldBounds.y+0.5f);
    std::cerr << "mage at " << x << " " << y << " in attack state\n";
    int playerDist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
    if ((playerDist >= 0) && (playerDist < GUARD_ENGAGE_DIST )
        && (character->stats.EP > 0 ))
    {
        std::cerr << "mage at " << x << " " << y << " attacking from distance " << level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y] << "\n";
        Vector2 dir = {level->player->worldBounds.x - x, level->player->worldBounds.y - y};
        float dist = sqrt(dir.x * dir.x + dir.y * dir.y)/2.0;
        //GridPos pDir =  getNextGridPos({x,y}, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE], false);
        //dir.x = pDir.x; dir.y = pDir.y;
        //float dist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
        dir.x /= dist;
        dir.y /= dist;
        Damage dmg = {10, DamageType::STEEL, 0};
        character->stats.EP -= dmg.value/19;
        Direction attackDirection = getNextDir({x,y}, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE]);
        if ( attackDirection < Direction::DIR_NONE )
        {
            std::cerr << "Guard at " << x << " " << y << " attacks in dir: " << attackDirection << "\n";
            level->applyDmgPattern(dmg, {x,y}, &guardAttackPattern[attackDirection], true);
        }
        return true;
    }
    return idleGuard(character, delta, level);
}

bool idleMage(Character *character, float delta, Level *level)
{
    const float MAGE_ENGAGE_DIST = 20.0f;
    int x = static_cast<int>(character->worldBounds.x+0.5f);
    int y = static_cast<int>(character->worldBounds.y+0.5f);
    int playerDist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
    if ((playerDist >= 0) && (playerDist < MAGE_ENGAGE_DIST )
        && (character->stats.EP > 0 ))
    {
        std::cerr << "mage at " << x << " " << y << " going into attack state\n";
        character->state = CharacterState::CHAR_SPECIAL_1;
        return true;
    }
    return false;
}

bool moveMage(Character *character, float delta, Level *level)
{
    return moveVillager(character, delta, level);
}

bool attackMage(Character *character, float delta, Level *level)
{ 
    const float MAGE_ENGAGE_DIST = 20.0f;
    int x = static_cast<int>(character->worldBounds.x+0.5f);
    int y = static_cast<int>(character->worldBounds.y+0.5f);
    std::cerr << "mage at " << x << " " << y << " in attack state\n";
    int playerDist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
    if ((playerDist >= 0) && (playerDist < MAGE_ENGAGE_DIST )
        && (character->stats.EP > 0 ))
    {
        std::cerr << "mage at " << x << " " << y << " attacking from distance " << level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y] << "\n";
        Vector2 dir = {level->player->worldBounds.x - x, level->player->worldBounds.y - y};
        float dist = sqrt(dir.x * dir.x + dir.y * dir.y)/2.0;
        //GridPos pDir =  getNextGridPos({x,y}, level->distanceMaps[DistanceMapType::PLAYER_DISTANCE], false);
        //dir.x = pDir.x; dir.y = pDir.y;
        //float dist = level->distanceMaps[DistanceMapType::PLAYER_DISTANCE][x][y];
        dir.x /= dist;
        dir.y /= dist;
        Damage dmg = {10, DamageType::WATER, 0};
        character->stats.EP -= dmg.value/19;
        level->launchProjectile(dmg, {x,y,1,1}, dir, dist, new AnimatedSprite(
                    // texture bounds
                    {0.0,16.0,16.0,16.0},
                    // screen bounds
                    LevelScreen::WorldToScreen(level->screen, character->worldBounds),
                    Datastore::getInstance().getTexture("images/ui.png"),
                    {{CharacterState::CHAR_IDLE,
                    (Animation){-1, {},
                    {   {0.2, {0.0,16.0,16.0,16.0}},
                        {0.2, {16.0,16.0,16.0,16.0}},
                        {0.2, {32.0,16.0,16.0,16.0}},
                        {0.2, {48.0,16.0,16.0,16.0}},
                        {0.2, {64.0,16.0,16.0,16.0}},
                        {0.2, {80.0,16.0,16.0,16.0}}
                    }}}}));
        return true;
    }
    return idleMage(character, delta, level);
}

bool idleHero(Character *character, float delta, Level *level)
{
    return idleVillager(character, delta, level);
}

bool moveHero(Character *character, float delta, Level *level)
{
    return moveVillager(character, delta, level);
}

bool attackHero(Character *character, float delta, Level *level)
{
    return attackVillager(character, delta, level);
}

} // namespace strategy
