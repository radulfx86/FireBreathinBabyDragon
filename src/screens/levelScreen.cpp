#include "levelScreen.h"
#include "datastore.h"
#include "characterAnimations.h"
#include "villagerStrategy.h"
#include "objectStrategy.h"
#include "debugStats.h"
#include <sstream>
#include <cmath>
#include <execution>
#include <queue>
#include <cstring>

void InfoScreen::draw(float delta)
{
    sumDelta += delta;
    ++frameCnt;
    if ( sumDelta > 1.0 )
    {
        std::stringstream fpsStr;
        fpsStr << "FPS: " << frameCnt << "\n";
        this->fpsText.assign(fpsStr.str());
        
        sumDelta -= 1.0;
        frameCnt = 0;
    }
    /// TODO make this stuff dynamic, liek the demo-list
    DrawText(fpsText.c_str(), this->origin.x + 0, this->origin.y + 0,30,ORANGE);
    DrawText(scaleText.c_str(), this->origin.x + 150, this->origin.y + 0,30,ORANGE);
    DrawText(this->tileText.c_str(), this->origin.x + 0, this->origin.y + 30, 30, BLUE);
    DrawText(this->distanceMapText.c_str(), this->origin.x + 0, this->origin.y + 60, 30, WHITE);
}
void InfoScreen::setActiveDistanceMap(DistanceMapType selectedDebugDistanceMap){
    this->selectedDebugDistanceMap = selectedDebugDistanceMap;
    std::stringstream sstr;
    sstr << "selected distance map >" << static_cast<int>(selectedDebugDistanceMap) << "<";
    this->distanceMapText = sstr.str();
}
void InfoScreen::setNumTiles(int numTiles)
{
    if ( this->numTiles != numTiles )
    {
        std::stringstream tileStr;
        tileStr << "#tiles: " << numTiles;
        this->tileText.assign(tileStr.str());
    }
}

void LevelScreen::loadSounds()
{
    this->fireBreath = Datastore::getInstance().getSound("audio/Firebreath_Level_1.mp3");

}

void LevelScreen::loadLevel()
{
    /// hard-coded level-name
    Image levelImageData = LoadImage("images/testLevel.png");
    
    this->levelSize = {static_cast<float>(levelImageData.width),
                        static_cast<float>(levelImageData.height)};

    this->objectTexture = Datastore::getInstance().getTexture("images/assets.png");
    this->npcTexture = Datastore::getInstance().getTexture("images/villagers.png");
     
    const unsigned int tilesX = this->levelSize.x;
    const unsigned int tilesY = this->levelSize.y;
    this->tiles = TileMap(Datastore::getInstance().getTexture("images/tileMap.png"),{16.0f,16.0f},{4.0f,4.0f});
    this->distanceMaps[DistanceMapType::PLAYER_DISTANCE].resize(tilesX,std::vector<int>(tilesY));
    float tileScaleFactor = 1.0;
    
    for ( unsigned int y = 0; y < levelImageData.height; ++y )
    {
        for ( unsigned int x = 0; x < levelImageData.width; ++x )
        {
            Color levelColor = GetImageColor(levelImageData, x, y);
            if ( levelColor.r == 255 && levelColor.g == 0 && levelColor.b == 0 )
            {
                /// add Player
                addCharacter(CharacterType::PLAYER, x, y);
            }
            if ( levelColor.r == 0 && levelColor.g == 0 && levelColor.b == 255 )
            {
                /// add Mage
                addCharacter(CharacterType::MAGE, x, y);
            }
            if ( levelColor.r == 255 && levelColor.g == 255 && levelColor.b == 0 )
            {
                /// add Villager
                addCharacter(CharacterType::VILLAGER, x, y);
            }
            if ( levelColor.r == 0 && levelColor.g == 149 && levelColor.b == 255 )
            {
                /// add Guard
                addCharacter(CharacterType::GUARD, x, y);
            }
            if ( levelColor.r == 75 && levelColor.g == 105 && levelColor.b == 47 )
            {
                /// add Tree
                addObject(ObjectType::TREE, x, y);
            }
            if ( levelColor.r == 203 && levelColor.g == 219 && levelColor.b == 252 )
            {
                /// add House
                addObject(ObjectType::HOUSE, x, y);
            }
            /// TILES:
            Vector2 defaultTile = {(float)(x%1),0};//(float)(y%2)};
            /// source rectangle in source-px-coordinates
            this->tiles.coords.push_back((std::pair<Rectangle, Rectangle>){{
                defaultTile.x * this->tiles.tileSize.x,
                defaultTile.y * this->tiles.tileSize.y,
                this->tiles.tileSize.x,
                this->tiles.tileSize.y},
            /// target rectangle in target-px-coordinates (scaled)
            {
                x * this->tiles.tileSize.x * tileScaleFactor,
                y * this->tiles.tileSize.y * tileScaleFactor,
                this->tiles.tileSize.x * tileScaleFactor,
                this->tiles.tileSize.y * tileScaleFactor}});
        }
    }
    UnloadImage(levelImageData);

    for (int type = 0; type < 4; ++type )
    {
        distanceMaps[static_cast<DistanceMapType>(type)].resize(levelSize.x,std::vector<int>(levelSize.y));
        //updateDistanceMap(static_cast<DistanceMapType>(i), {this->levelSize.x/2, this->levelSize.y/2});
                std::for_each(std::execution::par_unseq,
                    this->distanceMaps[type].begin(),
                    this->distanceMaps[type].end(), 
                    [](std::vector<int> &dist){
//                        std::memset(&dist[0], -1, sizeof(int)*dist.size());
                        std::for_each(std::execution::par_unseq,
                                        dist.begin(),
                                        dist.end(),
                                        [](int &n) { n = -1; });
        });
    }
    // find a better solution to find villages !!!
    for ( auto tmp : this->objects )
    {
        Character *obj = tmp.second;
        if ( obj->sprite->textureBounds.y == 0  // HOUSE
            || obj->sprite->textureBounds.y == 1 ) // TOWER
        {
            updateDistanceMap(DistanceMapType::VILLAGE_DISTANCE, {obj->worldBounds.x+1, obj->worldBounds.y+1}, false, true, 10);
        }
    }
}

void LevelScreen::addCharacter(CharacterType charType, int x, int y)
{
    std::cerr << __func__ << "(" << (int)charType << ", " << x << ", " << y << ")\n";
    if ( charType == CharacterType::PLAYER )
    {
        this->charSpeedMax = 10.0;
        this->charSpeed = {0.0f, 0.0f};
        this->charAcc = 50.0;

        Rectangle playerWorldBounds{x,y,2,2};
        Rectangle playerScreenBounds = LevelScreen::WorldToScreen(this, playerWorldBounds);
        playerScreenBounds.width = 32;
        playerScreenBounds.height = 32;
        WorldObjectStatus initialPlayerStats = {10,10};
        this->player = new Character("player", CharacterState::CHAR_IDLE,
            initialPlayerStats,
            playerWorldBounds,
            playerScreenBounds,
            new AnimatedSprite(
                {0.0,0.0,32.0,32.0},
                playerScreenBounds,
                Datastore::getInstance().getTexture("images/dragon_0_20240112_01.png"),
                {{CharacterState::CHAR_IDLE, idleDragon}}
        ));
        this->drawableObjects.push_back(this->player);
        std::cerr << " add player at " << player->worldBounds.x << " " << player->worldBounds.y << "\n";
    }
    else
    {
        /// NPCs
        /// class-specific parameters
        TraceLog(LOG_INFO, "load NPCs");
        Rectangle npcWorldBounds;
        npcWorldBounds.width = 1;
        npcWorldBounds.height = 1;
        WorldObjectStatus initialNPCStats = {10,10};
        Rectangle npcScreenBounds{0,0,16,16};
        Rectangle npcTextureBounds{0,0,16,16};
        std::map<CharacterState, Animation> npcAnimations = {
            {CharacterState::CHAR_IDLE, charAnimationIdle},
            {CharacterState::CHAR_DIE, charAnimationDie},
            {CharacterState::CHAR_WALK_N, charAnimationWalkN},
            {CharacterState::CHAR_WALK_E, charAnimationWalkE},
            {CharacterState::CHAR_WALK_S, charAnimationWalkS},
            {CharacterState::CHAR_WALK_W, charAnimationWalkW}
            };
        CharacterState stateMap[] = {CharacterState::CHAR_IDLE,
            CharacterState::CHAR_DIE,
            CharacterState::CHAR_WALK_N,
            CharacterState::CHAR_WALK_E,
            CharacterState::CHAR_WALK_S,
            CharacterState::CHAR_WALK_W};
        npcWorldBounds.x = x;
        npcWorldBounds.y = y;
        npcScreenBounds = LevelScreen::WorldToScreen(this, npcWorldBounds);
        npcTextureBounds.y = GetRandomValue(1,3) * 16;
        CharacterState npcState = CharacterState::CHAR_IDLE;
        Character *npc = 
            new Character("NPC", npcState,
                initialNPCStats,
                npcWorldBounds,
                npcScreenBounds,
                new AnimatedSprite(
                    npcTextureBounds,
                    npcScreenBounds,
                    npcTexture,
                    npcAnimations,{(AnimationState){npcState,0,0,0}}
                    ));
        TraceLog(LOG_DEBUG,"generate character at world: {pos {%.0f,%.0f}, size {%.0f,%.0f}} screen {pos {%.0f,%.0f}, size {%.0f,%.0f}} statue: %d",
            npcWorldBounds.x, npcWorldBounds.y, npcWorldBounds.width, npcWorldBounds.height,
            npcScreenBounds.x, npcScreenBounds.y, npcScreenBounds.width, npcScreenBounds.height,
            static_cast<int>(npcState));
    
        /// common parameters
        switch ( charType )
        {
            case VILLAGER:
            {
                npc->name = "meh";
                npc->sprite->textureBounds.y = 0 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleVillager;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackVillager;
                break;
            }
            case GUARD:
            {
                npc->sprite->textureBounds.y = 2 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleGuard;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackGuard;
                break;
            }
            case MAGE:
            {
                npc->sprite->textureBounds.y = 3 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleMage;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackMage;
                break;
            }
            case HERO:
            {
                /// hero for now cosplays as female villager
                npc->sprite->textureBounds.y = 1 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleHero;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackHero;
                break;
            }
            case PLAYER:
                // fall through
            default:
                break;
        }

        this->drawableObjects.push_back(npc);
        this->characters.push_back(npc);
    }
}

void LevelScreen::addObject(ObjectType objType, int x, int y)
{
    std::cerr << __func__ << "(" << (int)objType << ", " << x << ", " << y << ")\n";
    Rectangle objectWorldBounds;
    objectWorldBounds.width = 1;
    objectWorldBounds.height = 2;
    WorldObjectStatus initialObjectStats = {10,10};
    Rectangle objectScreenBounds{0,0,1,2};
    Rectangle objectTextureBounds{0,0,16,32};
    const int MAX_OBJECTS = 20;
    this->winThreshold = MAX_OBJECTS/10;
    std::map<CharacterState, Animation> objectAnimations = 
        {{CharacterState::CHAR_IDLE,
            (Animation){-1, {},
            {   {0.2, {0.0,0.0,16.0,32.0}},
                {0.2, {16.0,0.0,16.0,32.0}},
                {0.2, {32.0,0.0,16.0,32.0}},
                {0.2, {48.0,0.0,16.0,32.0}}
            }}},
            {CharacterState::CHAR_CATCH_FIRE,
            (Animation){1, {}, /// TODO add action to transit to CHAR_BURNING
            {   {0.2, {64.0,0.0,16.0,32.0}},
                {0.2, {80.0,0.0,16.0,32.0}},
                {0.2, {96.0,0.0,16.0,32.0}}
            }}},
            {CharacterState::CHAR_BURNING,
            (Animation){-1, {},
            {   {0.2, {112.0,0.0,16.0,32.0}},
                {0.2, {128.0,0.0,16.0,32.0}},
                {0.2, {144.0,0.0,16.0,32.0}}
            }}}};
    objectWorldBounds.x = x;
    objectWorldBounds.y = y;
    objectScreenBounds = LevelScreen::WorldToScreen(this, objectWorldBounds);
    switch ( objType )
    {
        case ObjectType::HOUSE:
            objectTextureBounds.y = 0 * 32;
            break;
        case ObjectType::TREE:
            objectTextureBounds.y = GetRandomValue(3,4) * 32;
            objectTextureBounds.y = 3 * 32;
            break;
        case ObjectType::TOWER:
            objectTextureBounds.y = 1 * 32;
            break;
        case ObjectType::WALL:
            /// TODO tower is a wall now ...
            objectTextureBounds.y = 1 * 32;
            break;
        default:
            break;
    }
    Character *obj = 
            new Character("OBJECT", CharacterState::CHAR_IDLE,
                initialObjectStats,
                objectWorldBounds,
                objectScreenBounds,
                new AnimatedSprite(
                    objectTextureBounds,
                    objectScreenBounds,
                    objectTexture,
                    objectAnimations));
    obj->strategy[CharacterState::CHAR_IDLE] = strategy::idleObject;
    obj->strategy[CharacterState::CHAR_BURNING] = strategy::burningObject;
    obj->strategy[CharacterState::CHAR_CATCH_FIRE] = strategy::startBurningObject;
    /// TODO random start time for animation
    obj->sprite->animationState.activeFrame = GetRandomValue(0,obj->sprite->animations[obj->sprite->animationState.activeAnimation].frames.size());
    obj->sprite->animationState.frameDelta = 
        obj->sprite->animations[obj->sprite->animationState.activeAnimation].frames[obj->sprite->animationState.activeFrame].first
         * static_cast<float>(GetRandomValue(0,100))/100.0f;
    this->objects[(GridPos){x,y}] = obj;
    this->drawableObjects.push_back(obj);
}

void LevelScreen::updateDistanceMap(DistanceMapType selectedDistanceMap, Vector2 worldTargetPos)
{
    if ( nullptr == this->player )
    {
        return;
    }
    /// TODO: do proper flood-fill or anything better than this !!!
    for ( int y = 0; y < this->levelSize.y; ++y )
    {
        for ( int x = 0; x < this->levelSize.x; ++x )
        {
            // do manhattan distance
            Vector2 delta = {x - worldTargetPos.x, y - worldTargetPos.y};
            distanceMaps[static_cast<DistanceMapType>(selectedDistanceMap)][x][y] = fabs(delta.x) + fabs(delta.y);
        }
    }
}

void LevelScreen::loadUI()
{
    this->ui = new UI({0,0}, 2.0, &this->player->stats);
}

void LevelScreen::initialize()
{
    TRACE;
    SetWindowSize(640, 480);
    this->camera.offset = {0,0};
    this->camera.rotation = 0;
    this->camera.target = {640, 480};
    this->camera.zoom = 1.0;
    //SetTargetFPS(-1);

    this->infoScreen = new InfoScreen({400,10});

    loadLevel();

    loadUI();

}
void LevelScreen::finalize()
{
    TRACE;
}
void LevelScreen::enter()
{
    TRACE;
    SetWindowTitle("LEVEL 1");

}
void LevelScreen::exit()
{
    TRACE;
    DebugStats::getInstance().printStats();
}

void LevelScreen::movePlayer(float delta)
{
    Vector2 spdDelta = {0.0f,0.0f};
    if ( IsKeyDown(KEY_W) ) // move up
    {
        spdDelta.y -= delta * this->charAcc;
    }
    else
    {
        if ( this->charSpeed.y < 0.0 )
        {
            spdDelta.y += delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_A) ) // move left
    {
        spdDelta.x -= delta * this->charAcc;
    }
    else
    {
        if ( this->charSpeed.x < 0.0 )
        {
            spdDelta.x += delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_S) ) // move down
    {
        spdDelta.y += delta * this->charAcc;
    }
    else
    {
        if ( this->charSpeed.y > 0.0 )
        {
            spdDelta.y -= delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_D) ) // move right
    {
        spdDelta.x += delta * this->charAcc;
    }
    else
    {
        if ( this->charSpeed.x > 0.0 )
        {
            spdDelta.x -= delta * this->charAcc;
        }
    }
    /// NOTE: WARNING, unchecked addition - no collision-check against anything, even level boundaries
    this->charSpeed.x += spdDelta.x;
    this->charSpeed.y += spdDelta.y;
    if ( this->charSpeed.x > this->charSpeedMax )
    {
        this->charSpeed.x = this->charSpeedMax;
    }
    if ( this->charSpeed.y > this->charSpeedMax )
    {
        this->charSpeed.y = this->charSpeedMax;
    }
    if ( this->charSpeed.x < -this->charSpeedMax )
    {
        this->charSpeed.x = -this->charSpeedMax;
    }
    if ( this->charSpeed.y < -this->charSpeedMax )
    {
        this->charSpeed.y = -this->charSpeedMax;
    }

    /// TODO intermediate "solution"
    if ( fabs(this->charSpeed.x) > 1.0f )
    {
        this->player->worldBounds.x += this->charSpeed.x * delta;
    }
    if ( fabs(this->charSpeed.y) > 1.0f )
    {
        this->player->worldBounds.y += this->charSpeed.y * delta;
    }
    GridPos newPlayerGridPos = {
        this->player->worldBounds.x,
        this->player->worldBounds.y };
    Vector2 screenPos = LevelScreen::WorldToScreenPos(this,(Vector2){this->player->worldBounds.x, this->player->worldBounds.y});
    this->player->screenBounds.x = screenPos.x;
    this->player->screenBounds.y = screenPos.y;
    this->player->sprite->screenBounds = this->player->screenBounds;

    /// simple NPC kill
    Character *hit = this->getCollision(this->player, this->player->worldBounds);
    if ( hit )
    {
        hit->stats.HP = 0;
    }
    if ( newPlayerGridPos != this->lastPlayerGridPos )
    {
        lastPlayerGridPos = newPlayerGridPos;
        updateDistanceMap(DistanceMapType::PLAYER_DISTANCE,
            {this->player->worldBounds.x+1, this->player->worldBounds.y+1},
            true, true,
            20);
    }
}

void LevelScreen::updateDistanceMap(DistanceMapType type, GridPos pos, bool clean, bool ignoreObjects, int distMax)
{
    /// reset
    if ( clean )
    {
        std::for_each(std::execution::par_unseq,
                    this->distanceMaps[type].begin(),
                    this->distanceMaps[type].end(), 
                    [](std::vector<int> &dist){
//                        std::memset(&dist[0], -1, sizeof(int)*dist.size());
                        std::for_each(std::execution::par_unseq,
                                        dist.begin(),
                                        dist.end(),
                                        [](int &n) { n = -1; });
        });
    }
    const int MAX_STEP = 1000;
    bool done = false;
    using Dist_t = struct {int dist; GridPos pos;};
    const GridPos directions[] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    std::queue<Dist_t> queue;
    queue.push((Dist_t){0, pos});
    for ( int step = 0; (!queue.empty()) && step < MAX_STEP; ++step )
    {
        Dist_t current = queue.front();
        if ( current.dist > distMax )
        {
            break;
        }
        queue.pop();

        for ( int d = 0; d < 4; ++d )
        {
            GridPos pos = current.pos + directions[d];
            if ( (pos.x == 0 && pos.y == 0)
                 || pos.x <= 0 || pos.x >= this->levelSize.x
                 || pos.y <= 0 || pos.y >= this->levelSize.y )
            {
                continue;
            }
            bool collision = false;
            if ( ignoreObjects && (this->objects.count(pos) > 0 || this->objects.count({pos.x,pos.y-1}) > 0))
            {
                continue;
            }
            if ( (this->distanceMaps[type][pos.x][pos.y] == -1 ||
                    current.dist < this->distanceMaps[type][pos.x][pos.y])
                )
            {
                this->distanceMaps[type][pos.x][pos.y] = current.dist;
                queue.push((Dist_t){current.dist+1, pos.x, pos.y});
            }
            else
            {
                continue;
            }
        }
    }
}

bool LevelScreen::checkCollision(Character *source, Rectangle worldBounds)
{
    /// TODO HELL NO THIS IS SLOW - USE A QUADTREE OR SOMETHING; BUT NOT THIS WAAAAAAAAH
    /// this is complexity O(n^2) ... there is a solution to do this in worst case O(nlogn)
    for ( Character *target : this->drawableObjects )
    {
        if ( target != source && CheckCollisionPointRec({worldBounds.x + 0.5f, worldBounds.y + 0.5f}, target->worldBounds))
        {
            return true;
        }
    }
    return false;
}

Character* LevelScreen::getCollision(Character *source, Rectangle worldBounds)
{
    /// TODO HELL NO THIS IS SLOW - USE A QUADTREE OR SOMETHING; BUT NOT THIS WAAAAAAAAH
    /// this is complexity O(n^2) ... there is a solution to do this in worst case O(nlogn)
    for ( Character *target : this->drawableObjects )
    {
        if ( target != source && CheckCollisionRecs(worldBounds, target->worldBounds))
        {
            return target;
        }
    }
    return nullptr;
}

void LevelScreen::moveNPCs(float delta)
{
    float charSpeed = 2.0;
    for ( Character *npc : this->characters )
    {
        Vector2 deltaPos = { 0, 0};
        bool isMoving = true;
        switch( npc->state )
        {
            case CharacterState::CHAR_WALK_E:
                deltaPos.x += delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_W:
                deltaPos.x -= delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_N:
                deltaPos.y -= delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_S:
                deltaPos.y += delta * charSpeed;
                break;
            default:
                isMoving = false;
                break;
        }
        if ( isMoving )
        {
            Rectangle tempBounds = npc->worldBounds;
            tempBounds.x += deltaPos.x;
            if ( not this->checkCollision(npc, tempBounds) )
            {
                npc->worldBounds = tempBounds;
                npc->screenBounds = LevelScreen::WorldToScreen(this, tempBounds);
                npc->sprite->screenBounds = npc->screenBounds;
                TraceLog(LOG_DEBUG,"%s(%s) by {%.0f,%.0f} to screenPos {%.0f,%.0f}",
                    __func__, npc->name, deltaPos.x, deltaPos.y, npc->screenBounds.x, npc->screenBounds.y);
            }
            tempBounds.y += deltaPos.y;
            if ( not this->checkCollision(npc, tempBounds) )
            {
                npc->worldBounds = tempBounds;
                npc->screenBounds = LevelScreen::WorldToScreen(this, tempBounds);
                npc->sprite->screenBounds = npc->screenBounds;
                TraceLog(LOG_DEBUG,"%s(%s) by {%.0f,%.0f} to screenPos {%.0f,%.0f}",
                    __func__, npc->name, deltaPos.x, deltaPos.y, npc->screenBounds.x, npc->screenBounds.y);
            }
        }
    }

}

void LevelScreen::updateNPCs(float delta)
{
    for ( Character *npc : this->characters )
    {
        if ( npc->strategy.count(npc->state) > 0 )
        {
            npc->strategy[npc->state](npc, delta, this->distanceMaps);
        }
    }
}

void LevelScreen::updateObjects(float delta)
{
    std::vector<GridPos> deleteList;
    for ( auto tmp : this->objects )
    {
        Character *obj = tmp.second;
        if ( obj->strategy.count(obj->state) > 0 )
        {
            bool stateChange = obj->strategy[obj->state](obj, delta, this->distanceMaps);
            if ( stateChange )
            {
                if ( obj->stats.HP <= 0 )
                {
                    deleteList.push_back(tmp.first);
                }
                this->updateDistanceMap(DistanceMapType::FIRE_DISTANCE,
                    tmp.first, false, false, 99);
                std::cerr << "UPDATING FIRE distance for OBJECT at " << tmp.first.x << " " << tmp.first.y << "\n";
            }
        }
    }
    /// TODO find a better way instead of this very clumsy attempt at removing stuff ...
    if ( deleteList.size() > 0 )
    {
        for ( std::vector<Character*>::iterator it = this->drawableObjects.begin(); it != this->drawableObjects.end(); )
        {
            if ( (*it)->stats.HP <= 0 )
            {
                it = this->drawableObjects.erase(it);
            }
            else
            {
                ++it;
            }
        }
        for ( GridPos key : deleteList)
        {
            this->objects.erase(key);
        }
    }
}
void LevelScreen::checkWinCondition()
{
    if ( player->stats.HP > 0 )
    {
        int sumRemaining = 0;
        for ( auto tmp : this->objects )
        {
            Character *obj = tmp.second;
            if ( obj->stats.HP > 0 )
            {
                ++sumRemaining;
            }
        }
        //std::cerr << "sumRemaining: " << sumRemaining << " threshold: " << this->winThreshold << "\n";
        if ( sumRemaining < this->winThreshold )
        {
            this->isDone = true;
        }
    }
    else
    {
        std::cerr << "gameover, player HP: " << player->stats.HP << "\n";
        this->isGameOver = true;
    }
}

void LevelScreen::update(float delta)
{
    if ( !IsTextureReady(this->npcTexture) || !IsTextureReady(this->objectTexture) )
    {
        return;
    }
    TRACE;
    if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT) )
    {
        if( CheckCollisionPointRec(GetMousePosition(), this->player->sprite->screenBounds) )
        {
            this->isDone = true;
            PlaySound(this->fireBreath);
        }
        this->updateDistanceMap(this->selectedDebugDistanceMap,LevelScreen::ScreenToWorld(this,GetMousePosition()));
        DebugStats::getInstance().printStats();
    }
    if ( IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) )
    {
        Vector2 clickPos = LevelScreen::ScreenToWorld(this, GetMousePosition());
        GridPos clickGrid = {clickPos.x, clickPos.y};
        if ( (this->objects.count(clickGrid) > 0 )
            && (this->objects[clickGrid]->state == CharacterState::CHAR_IDLE ) )
        {
            this->objects[clickGrid]->state = CharacterState::CHAR_CATCH_FIRE;
            this->objects[clickGrid]->sprite->animationState.activeAnimation = CharacterState::CHAR_CATCH_FIRE;
            this->objects[clickGrid]->sprite->animationState.activeFrame = 0;
            this->objects[clickGrid]->sprite->animationState.currentLoop = 0;
            this->objects[clickGrid]->sprite->animationState.frameDelta = 0;
            this->updateDistanceMap(DistanceMapType::FIRE_DISTANCE,
                clickGrid, false, false, 99);
            std::cerr << "set object on fire at " << clickGrid.x << " " << clickGrid.y <<"\n";
        }
        else
        {
            std::cerr << "nothing to burn at " << clickGrid.x << " " << clickGrid.y << "\n";
        }
    }
    // cycle through distance maps - NOTE: write the active distance map somewhere
    static bool isKeyQPressed = true;
    if ( IsKeyDown(KEY_Q) )
    {
        if ( !isKeyQPressed )
        {
            selectedDebugDistanceMap = static_cast<DistanceMapType>((static_cast<int>(selectedDebugDistanceMap) + 1 ) % this->distanceMaps.size());
            infoScreen->setActiveDistanceMap(selectedDebugDistanceMap);   
        }
        isKeyQPressed = true;
    }
    else
    {
        isKeyQPressed = false;
    }
    float zoomDelta = GetMouseWheelMove();
    bool needsUpdate = false;
    if ( zoomDelta != 0.0 )
    {
        /// TODO the zoom is inverted for characters and tiles ... fixing later
        //this->scale = (this->scale + zoomDelta) > 0.3 ? (this->scale + zoomDelta) < 5.0 ? this->scale + zoomDelta : 5.0 : 0.3;
        Vector2 mousePos = GetMousePosition();
        /// TODO find a way to zoom into the mouse position ... too tired for this now
        this->camera.zoom = this->scale;
        std::stringstream scaleStr;
        scaleStr << "scale: " << this->scale;
        this->infoScreen->scaleText = scaleStr.str();
        needsUpdate = true;
    }
    // since zoom does not work:
    needsUpdate = false;

    if ( needsUpdate )
    {
        this->tiles.updateCamera(this->camera);
        for ( Character *character : this->characters )
        {
            character->screenBounds = LevelScreen::WorldToScreen(this, character->worldBounds);
            character->sprite->screenBounds = character->screenBounds;
        }
        for ( auto tmp : this->objects )
        {
            Character *obj = tmp.second;
            obj->screenBounds = LevelScreen::WorldToScreen(this, obj->worldBounds);
            obj->sprite->screenBounds = obj->screenBounds;
        }
    }

    checkWinCondition();

    updateObjects(delta);
    updateNPCs(delta);

    movePlayer(delta);
    moveNPCs(delta);

    /// TODO z-sort characters and objects !!!

    this->draw(delta);
}
void LevelScreen::sortDrawableObjects()
{
    // sort by y-value
    std::sort(this->drawableObjects.begin(), this->drawableObjects.end(),
        [](Character *a, Character *b) {
            return a->screenBounds.y < b->screenBounds.y;
        }
    );
}

void LevelScreen::draw(float delta)
{
    TRACE;
    BeginDrawing();
        ClearBackground(GREEN);

        //int numTiles = this->tiles.draw();
        //infoScreen->setNumTiles(numTiles);
        /// start debug
        std::string distanceText;
        static Color distanceMapColor[] = {RED, BLUE, PURPLE, YELLOW};
        for ( int y = 0; y < this->levelSize.y; ++y )
        {
            for ( int x = 0; x < this->levelSize.x; ++x )
            {
                if ( distanceMaps.count(selectedDebugDistanceMap)
                    && (distanceMaps[selectedDebugDistanceMap][x][y] >= 0))
                {
                    Rectangle screenTextBounds = LevelScreen::WorldToScreen(this, (Rectangle){static_cast<float>(x), static_cast<float>(y), 1,1});
                    distanceText = std::to_string(distanceMaps[selectedDebugDistanceMap][x][y]);
                    distanceText = std::to_string(distanceMaps[selectedDebugDistanceMap][x][y]);
                    DrawText(distanceText.c_str(), screenTextBounds.x, screenTextBounds.y, 10, distanceMapColor[this->selectedDebugDistanceMap]);
                }
            }
        }
        
        sortDrawableObjects();

        for ( Character *object : this->drawableObjects )
        {
            if ( object->sprite && object->stats.HP > 0 )
            {
                object->sprite->draw(delta);
            }
        }

        this->ui->draw(delta);

        this->infoScreen->draw(delta);
    EndDrawing();

}
