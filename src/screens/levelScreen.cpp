#include "levelScreen.h"
#include "datastore.h"
#include "characterAnimations.h"
#include <sstream>
#include <cmath>

bool getNextPos(int &x, int &y, CharacterState &dir, std::vector<std::vector<int>> distanceMap)
{
    const int maxX = distanceMap.size()-1;
    const int maxY = distanceMap[0].size()-1;
    x = (x < 0 ) ? 0 : x > maxX ? maxX : x;
    y = (y < 0 ) ? 0 : y > maxY ? maxY : y;
    #if 0
    std::cerr << "distances: L: " << distanceMap[x-1][y]
        << " R: " << distanceMap[x+1][y]
        << " U: " << distanceMap[x][y-1]
        << " D: " << distanceMap[x][y+2] << "\n";
        #endif
    float dist = distanceMap[x][y];
    // left
    if ( x > 0 && distanceMap[x-1][y] < dist )
    {
        dist = distanceMap[x-1][y];
        dir = CharacterState::CHAR_WALK_W;
        std::cerr << __func__ << " left dist: " << dist
        << " select dir " << static_cast<int>(dir) <<  std::endl;
    } // right
    if ( x < distanceMap.size()-1 && distanceMap[x+1][y] < dist )
    {
        dist = distanceMap[x+1][y];
        dir = CharacterState::CHAR_WALK_E;
        std::cerr << __func__ << " right dist: " << dist
        << " select dir " << static_cast<int>(dir) <<  std::endl;
    } // up
    if ( y > 0 && distanceMap[x][y-1] < dist )
    {
        dist = distanceMap[x][y-1];
        dir = CharacterState::CHAR_WALK_N;
        std::cerr << __func__ << " up dist: " << dist
        << " select dir " << static_cast<int>(dir) <<  std::endl;
    } // down
    if ( y < distanceMap[0].size() && distanceMap[x][y-1] < dist )
    {
        dist = distanceMap[x][y+1];
        dir = CharacterState::CHAR_WALK_S;
        std::cerr << __func__ << " down dist: " << dist
        << " select dir " << static_cast<int>(dir) <<  std::endl;
    }
    return true;
}

bool idleCharacter(Character *character, std::vector<std::vector<int>> distanceMap)
{
    if ( nullptr == character )
    {
        return false;
    }
    CharacterState dir = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x);
    int y = static_cast<int>(character->worldBounds.y);
    getNextPos(x,y,dir,distanceMap);
    std::cerr << __func__ << "(" << character->name << ") state " << static_cast<int>(character->state) << " -> " << static_cast<int>(dir) << "\n";
    if ( dir != CharacterState::CHAR_IDLE )
    {
        character->state = dir;
        character->sprite->animationState.activeAnimation = dir;
    }
    return true;
}

bool moveCharacter(Character *character, std::vector<std::vector<int>> distanceMap)
{
    if ( nullptr == character )
    {
        return false;
    }
    std::cerr << __func__ << "(" << character->name << ")\n";
    CharacterState dir = CharacterState::CHAR_IDLE;
    int x = static_cast<int>(character->worldBounds.x);
    int y = static_cast<int>(character->worldBounds.y);
    getNextPos(x, y, dir, distanceMap);
    std::cerr << __func__ << "(" << character->name << ")"
        << "state " << static_cast<int>(character->state)
        << "(" << static_cast<int>(character->sprite->animationState.activeAnimation)
        << ") -> " << static_cast<int>(dir)
        << " x: " << x << " y: " << y << "\n";
    character->state = dir;
    character->sprite->animationState.activeAnimation = dir;
    return true;
}

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
    BeginDrawing();
        /// TODO make this stuff dynamic, liek the demo-list
        DrawText(fpsText.c_str(), this->origin.x + 0, this->origin.y + 0,30,ORANGE);
        DrawText(scaleText.c_str(), this->origin.x + 150, this->origin.y + 0,30,ORANGE);
        DrawText(this->tileText.c_str(), this->origin.x + 0, this->origin.y + 30, 30, BLUE);
    EndDrawing();
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
    this->dragonSprite = new AnimatedSprite(
        {0.0,0.0,32.0,32.0},
        {100.0,100.0,64.0,64.0},
        Datastore::getInstance().getTexture("images/dragon_0_20240112_01.png"),
        {{CharacterState::CHAR_IDLE, idleDragon}}
    );

    this->charSpeedMax = 200.0;
    this->charSpeed = {100.0f, 0.0f};
    this->charAcc = 100.0;

    Rectangle playerWorldBounds{100,100,64,64};
    Rectangle playerScreenBounds = LevelScreen::WorldToScreen(this, playerWorldBounds);
    this->player = new Character("player", CharacterState::CHAR_IDLE, playerWorldBounds, playerScreenBounds, this->dragonSprite);

    InitAudioDevice();
    this->fireBreath = Datastore::getInstance().getSound("audio/Firebreath_Level_1.mp3");

    /// NPCs
    std::cerr << "load NPCs" << std::endl;
    Rectangle npcWorldBounds;
    npcWorldBounds.width = 1;
    npcWorldBounds.height = 1;
    Rectangle npcScreenBounds{0,0,16,16};
    Rectangle npcTextureBounds{0,0,16,16};
    this->npcTexture = Datastore::getInstance().getTexture("images/villagers_20240112_01.png");
    const int MAX_NPC = 42;
    std::map<CharacterState, Animation> npcAnimations = {
        {CharacterState::CHAR_IDLE, charAnimationIdle},
        {CharacterState::CHAR_DIE, charAnimationDie},
        {CharacterState::CHAR_WALK_N, charAnimationWalkN},
        {CharacterState::CHAR_WALK_E, charAnimationWalkE},
        {CharacterState::CHAR_WALK_S, charAnimationWalkS},
        {CharacterState::CHAR_WALK_W, charAnimationWalkW}/*,
        {CharacterState::CHAR_WALK_W, (Animation){ -1, {}, 
        { { 0.3, {16.0,0.0,16.0,16.0} },
        { 0.3, {32.0,0.0,16.0,16.0} }
        }}}*/
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
        npcTextureBounds.y = GetRandomValue(0,4) * 16;
        CharacterState npcState = stateMap[static_cast<int>(GetRandomValue(0,5))];
        npcState = CharacterState::CHAR_IDLE;
        this->characters.push_back(
            new Character("NPC", npcState, npcWorldBounds,
                npcScreenBounds,
                new AnimatedSprite(
                    npcTextureBounds,
                    npcScreenBounds,
                    npcTexture,
                    npcAnimations,{(AnimationState){npcState,0,0,0}}
                    )));
                std::cerr << "generate character at " << npcScreenBounds.x
                    << ", " << npcScreenBounds.y
                    << " size " << npcScreenBounds.width
                    << " x " << npcScreenBounds.height 
                    << " in world at " << npcWorldBounds.x
                    << ", " << npcWorldBounds.y
                    << " size " << npcWorldBounds.width
                    << " x " << npcWorldBounds.height
                    << " with start state " << static_cast<int>(npcState) << "\n";
    }
    for (Character *npc : this->characters )
    {
        std::cerr << "modifying " << npc->name << " with state " << static_cast<int>(npc->state) << "(" << static_cast<int>(npc->sprite->animationState.activeAnimation) << ")" << "\n";
        npc->strategy[CharacterState::CHAR_IDLE] = idleCharacter;
        npc->strategy[CharacterState::CHAR_WALK_E] = moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_W] = moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_S] = moveCharacter;
        npc->strategy[CharacterState::CHAR_WALK_N] = moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_N] = moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_E] = moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_S] = moveCharacter;
        npc->strategy[CharacterState::CHAR_ATTACK_W] = moveCharacter;

    }
}

void LevelScreen::loadObjects()
{
    TRACE;
    ///
    std::cerr << "load Objects" << std::endl;
    Rectangle objectWorldBounds;
    objectWorldBounds.width = 1;
    objectWorldBounds.height = 2;
    Rectangle objectScreenBounds{0,0,1,1};
    Rectangle objectTextureBounds{0,0,16,32};
    this->objectTexture = Datastore::getInstance().getTexture("images/assets.png");
    const int MAX_OBJECTS = 2;
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
            Character("OBJECT", CharacterState::CHAR_IDLE, objectWorldBounds,
                objectScreenBounds,
                new AnimatedSprite(
                    objectTextureBounds,
                    objectScreenBounds,
                    objectTexture,
                    objectAnimations)));
    }
    std::cerr << this->objects.size() <<" Objects loaded" << std::endl;
}

void LevelScreen::updateDistanceMaps()
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
            Vector2 delta = {x - this->levelSize.x/2, y - this->levelSize.y/2};
            distanceMap[x][y] = sqrt(delta.x * delta.x + delta.y * delta.y);
        }
    }
}

void LevelScreen::loadTiles()
{
    this->tiles = TileMap(Datastore::getInstance().getTexture("images/tileMap.png"),{16.0f,16.0f},{4.0f,4.0f});
    const unsigned int tilesX = 50;
    const unsigned int tilesY = 50;
    this->levelSize = {tilesX, tilesY};
    this->distanceMap.resize(tilesX,std::vector<int>(tilesY));
    float tileScaleFactor = 1.0;
    for ( int y = 0; y < tilesY; ++y )
    {
        for ( int x = 0; x < tilesX; ++x )
        {
            Vector2 defaultTile = {(float)(x%4),(float)(y%4)};
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

void LevelScreen::initialize()
{
    TRACE;
    SetWindowSize(640, 480);
    this->camera.offset = {0,0};
    this->camera.rotation = 0;
    this->camera.target = {640, 480};
    this->camera.zoom = 1.0;
    //SetTargetFPS(-1);

    this->infoScreen = new InfoScreen({200,10});
    
    loadTiles();

    loadObjects();

    loadCharacters();

    updateDistanceMaps();
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

    if ( fabs(this->charSpeed.x) > 1.0f )
    {
        this->dragonSprite->screenBounds.x += this->charSpeed.x * delta;
    }
    if ( fabs(this->charSpeed.y) > 1.0f )
    {
        this->dragonSprite->screenBounds.y += this->charSpeed.y * delta;
    }
}

bool LevelScreen::checkCollision(Rectangle worldBounds)
{
    // TODO
    return false;
}

void LevelScreen::moveNPCs(float delta)
{
    std::cerr << __func__ << std::endl;
    /// TODO - leaving delta unused so I get the compiler warning
    float charSpeed = 10.0;
    for ( Character *npc : this->characters )
    {
        Vector2 deltaPos = { 0, 0};
        bool isMoving = true;
        //std::cerr << __func__ << "(" << npc->name << " with state " << static_cast<int>(npc->state) << "(" << static_cast<int>(npc->sprite->animationState.activeAnimation) << ")"  << "\n";
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
            if ( not this->checkCollision(tempBounds) )
            {
                npc->worldBounds = tempBounds;
                npc->screenBounds = LevelScreen::WorldToScreen(this, tempBounds);
                npc->sprite->screenBounds = npc->screenBounds;
                std::cerr << __func__ << "(" << npc->name << ") by " << deltaPos.x << " " << deltaPos.y << " to screen pos " << npc->screenBounds.x << " " << npc->screenBounds.y <<"\n";
            }
        }
    }

}

void LevelScreen::updateNPCs(float delta)
{
    for ( Character *npc : this->characters )
    {
            //std::cerr << __func__ << "(" << npc->name << " with state " << static_cast<int>(npc->state) << "(" << static_cast<int>(npc->sprite->animationState.activeAnimation) << ")"  << "\n";
        if ( npc->strategy.count(npc->state) > 0 )
        {
            std::cerr << __func__ << "(" << npc->name << " with state " << static_cast<int>(npc->state) << "(" << static_cast<int>(npc->sprite->animationState.activeAnimation) << ")"  << "\n";
            npc->strategy[npc->state](npc, this->distanceMap);
        }
    }
}

void LevelScreen::update(float delta)
{
    if ( !IsTextureReady(this->npcTexture) || !IsTextureReady(this->objectTexture) )
    {
        return;
    }
    TRACE;
    if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
            && CheckCollisionPointRec(GetMousePosition(), this->dragonSprite->screenBounds
                ) )
    {
        this->isDone = true;
        PlaySound(this->fireBreath);
    }
    float zoomDelta = GetMouseWheelMove();
    bool needsUpdate = false;
    if ( zoomDelta != 0.0 )
    {
        /// TODO the zoom is inverted for characters and tiles ... fixing later
        this->scale = (this->scale + zoomDelta) > 0.3 ? (this->scale + zoomDelta) < 5.0 ? this->scale + zoomDelta : 5.0 : 0.3;
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
        for ( Character &obj : this->objects )
        {
            obj.screenBounds = LevelScreen::WorldToScreen(this, obj.worldBounds);
            obj.sprite->screenBounds = obj.screenBounds;
        }
    }

    updateNPCs(delta);

    movePlayer(delta);
    moveNPCs(delta);

    /// TODO z-sort characters and objects !!!

    this->draw(delta);
    this->infoScreen->draw(delta);
}

void LevelScreen::draw(float delta)
{
    TRACE;
    BeginDrawing();
        ClearBackground(GREEN);
            DrawRectangleLinesEx(this->dragonSprite->screenBounds, 1.0, RED);
            DrawText("this is the LEVEL", 100,100,30,ORANGE);
            DrawText("click on the dragon to exit", 100,300,30,ORANGE);

        //int numTiles = this->tiles.draw();
        //infoScreen->setNumTiles(numTiles);
        /// start debug
        std::string distanceText;
        for ( int y = 0; y < this->levelSize.y; ++y )
        {
            for ( int x = 0; x < this->levelSize.x; ++x )
            {
                Rectangle screenTextBounds = LevelScreen::WorldToScreen(this, (Rectangle){x, y, 1,1});
                distanceText = std::to_string(distanceMap[x][y]);
                DrawText(distanceText.c_str(), screenTextBounds.x, screenTextBounds.y, 10, RED);
            }
        }
        
        for ( Character &object : this->objects )
        {
            if ( object.sprite )
            {
                object.sprite->draw(delta);
            }
        }
        for ( Character *character : this->characters )
        {
            if ( character->sprite )
            {
                character->sprite->draw(delta);
                #if 0
                std::cerr << "draw character at " << character.sprite->screenBounds.x
                    << ", " << character.sprite->screenBounds.y
                    << " size " << character.sprite->screenBounds.width
                    << " x " << character.sprite->screenBounds.height << "\n";
                    #endif
            }
        }

        dragonSprite->draw(delta);
    EndDrawing();

}
