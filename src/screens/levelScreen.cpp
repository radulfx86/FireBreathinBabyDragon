#include "levelScreen.h"
#include "datastore.h"
#include "characterAnimations.h"
#include "villagerStrategy.h"
#include "debugStats.h"
#include <sstream>
#include <cmath>

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

void LevelScreen::loadCharacters()
{

    this->charSpeedMax = 10.0;
    this->charSpeed = {10.0f, 0.0f};
    this->charAcc = 50.0;

    Rectangle playerWorldBounds{10,10,2,2};
    Rectangle playerScreenBounds = LevelScreen::WorldToScreen(this, playerWorldBounds);
    playerScreenBounds.width = 32;
    playerScreenBounds.width = 32;
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
    this->drawableObjects.push_back(player);

    this->fireBreath = Datastore::getInstance().getSound("audio/Firebreath_Level_1.mp3");

    /// NPCs
    TraceLog(LOG_INFO, "load NPCs");
    Rectangle npcWorldBounds;
    npcWorldBounds.width = 1;
    npcWorldBounds.height = 1;
    WorldObjectStatus initialNPCStats = {10,10};
    Rectangle npcScreenBounds{0,0,16,16};
    Rectangle npcTextureBounds{0,0,16,16};
    this->npcTexture = Datastore::getInstance().getTexture("images/villagers.png");
    const int MAX_NPC = 100;
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
    for ( int i = 0; i < MAX_NPC; ++i )
    {
        npcWorldBounds.x = GetRandomValue(0, this->levelSize.x);
        npcWorldBounds.y = GetRandomValue(0, this->levelSize.y);
        npcScreenBounds = LevelScreen::WorldToScreen(this, npcWorldBounds);
        npcTextureBounds.y = GetRandomValue(1,3) * 16;
        CharacterState npcState = stateMap[static_cast<int>(GetRandomValue(0,5))];
        npcState = CharacterState::CHAR_IDLE;
        this->characters.push_back(
            new Character("NPC", npcState,
                initialNPCStats,
                npcWorldBounds,
                npcScreenBounds,
                new AnimatedSprite(
                    npcTextureBounds,
                    npcScreenBounds,
                    npcTexture,
                    npcAnimations,{(AnimationState){npcState,0,0,0}}
                    )));
            TraceLog(LOG_DEBUG,"generate character at world: {pos {%.0f,%.0f}, size {%.0f,%.0f}} screen {pos {%.0f,%.0f}, size {%.0f,%.0f}} statue: %d",
                npcWorldBounds.x, npcWorldBounds.y, npcWorldBounds.width, npcWorldBounds.height,
                npcScreenBounds.x, npcScreenBounds.y, npcScreenBounds.width, npcScreenBounds.height,
                static_cast<int>(npcState));
    }
    for (Character *npc : this->characters )
    {
        npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleCharacter;
        npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::moveCharacter;
        this->drawableObjects.push_back(npc);
    }
}

void LevelScreen::loadObjects()
{
    TRACE;
    TraceLog(LOG_INFO, "load objects");
    Rectangle objectWorldBounds;
    objectWorldBounds.width = 1;
    objectWorldBounds.height = 2;
    WorldObjectStatus initialObjectStats = {10,10};
    Rectangle objectScreenBounds{0,0,1,1};
    Rectangle objectTextureBounds{0,0,32,32};
    this->objectTexture = Datastore::getInstance().getTexture("images/assets.png");
    const int MAX_OBJECTS = 20;
    this->winThreshold = MAX_OBJECTS/10;
    std::map<CharacterState, Animation> objectAnimations = 
        {{CharacterState::CHAR_IDLE,
            (Animation){-1, {},
            {   {0.2, {0.0,0.0,16.0,32.0}},
                {0.2, {16.0,0.0,16.0,32.0}},
                {0.2, {32.0,0.0,16.0,32.0}},
                {0.2, {48.0,0.0,16.0,32.0}}
            }}}};
    for ( int i = 0; i < MAX_OBJECTS; ++i )
    {
        objectWorldBounds.x = GetRandomValue(0, this->levelSize.x);
        objectWorldBounds.y = GetRandomValue(0, this->levelSize.y);
        objectScreenBounds = LevelScreen::WorldToScreen(this, objectWorldBounds);
        objectTextureBounds.y = GetRandomValue(0,5) * 32;
        this->objects.push_back(
            new Character("OBJECT", CharacterState::CHAR_IDLE, initialObjectStats, objectWorldBounds,
                objectScreenBounds,
                new AnimatedSprite(
                    objectTextureBounds,
                    objectScreenBounds,
                    objectTexture,
                    objectAnimations)));
    }
    TraceLog(LOG_INFO, "%d objects loaded", this->objects.size());
    for (Character *obj : this->objects )
    {
        this->drawableObjects.push_back(obj);
    }
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

void LevelScreen::loadTiles()
{
    this->tiles = TileMap(Datastore::getInstance().getTexture("images/tileMap.png"),{16.0f,16.0f},{4.0f,4.0f});
    const unsigned int tilesX = 40;
    const unsigned int tilesY = 30;
    this->levelSize = {tilesX, tilesY};
    this->distanceMaps[DistanceMapType::PLAYER_DISTANCE].resize(tilesX,std::vector<int>(tilesY));
    float tileScaleFactor = 1.0;
    for ( int y = 0; y < tilesY; ++y )
    {
        for ( int x = 0; x < tilesX; ++x )
        {
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
    this->tiles.updateCamera(this->camera);
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
    
    loadTiles();

    loadObjects();

    loadCharacters();

    loadUI();

    for (int i = 0; i < 4; ++i )
    {
        distanceMaps[static_cast<DistanceMapType>(i)].resize(levelSize.x,std::vector<int>(levelSize.y));
        updateDistanceMap(static_cast<DistanceMapType>(i), {this->levelSize.x/2, this->levelSize.y/2});
    }
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
    Vector2 screenPos = LevelScreen::WorldToScreen(this,(Vector2){this->player->worldBounds.x, this->player->worldBounds.y});
    this->player->screenBounds.x = screenPos.x;
    this->player->screenBounds.y = screenPos.y;
    this->player->sprite->screenBounds = this->player->screenBounds;

    /// simple NPC kill
    Character *hit = this->getCollision(this->player, this->player->worldBounds);
    if ( hit )
    {
        hit->stats.HP = 0;
    }
    updateDistanceMap(DistanceMapType::PLAYER_DISTANCE, {this->player->worldBounds.x, this->player->worldBounds.y});
}

bool LevelScreen::checkCollision(Character *source, Rectangle worldBounds)
{
    /// TODO HELL NO THIS IS SLOW - USE A QUADTREE OR SOMETHING; BUT NOT THIS WAAAAAAAAH
    /// this is complexity O(n^2) ... there is a solution to do this in worst case O(nlogn)
    for ( Character *target : this->drawableObjects )
    {
        if ( target != source && CheckCollisionRecs(worldBounds, target->worldBounds))
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
            npc->strategy[npc->state](npc, this->distanceMaps);
        }
    }
}
void LevelScreen::checkWinCondition()
{
    if ( player->stats.HP > 0 )
    {
        int sumRemaining = 0;
        for ( Character *obj : this->objects )
        {
            if ( obj->stats.HP > 0 )
            {
                ++sumRemaining;
            }
        }
        std::cerr << "sumRemaining: " << sumRemaining << " threshold: " << this->winThreshold << "\n";
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
        for ( Character *obj : this->objects )
        {
            obj->screenBounds = LevelScreen::WorldToScreen(this, obj->worldBounds);
            obj->sprite->screenBounds = obj->screenBounds;
        }
    }

    checkWinCondition();

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

        int numTiles = this->tiles.draw();
        //infoScreen->setNumTiles(numTiles);
        /// start debug
        std::string distanceText;
        static Color distanceMapColor[] = {RED, BLUE, PURPLE, YELLOW};
        for ( int y = 0; y < this->levelSize.y; ++y )
        {
            for ( int x = 0; x < this->levelSize.x; ++x )
            {
                if ( distanceMaps.count(selectedDebugDistanceMap))
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
