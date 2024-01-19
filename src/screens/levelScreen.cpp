#include "levelScreen.h"
#include "datastore.h"
#include "characterAnimations.h"
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
        {CharacterState::CHAR_WALK_W, charAnimationWalkW}
        };
    for ( int i = 0; i < MAX_NPC; ++i )
    {
        npcWorldBounds.x = GetRandomValue(0, this->levelSize.x);
        npcWorldBounds.y = GetRandomValue(0, this->levelSize.y);
        npcScreenBounds = LevelScreen::WorldToScreen(this, npcWorldBounds);
        npcTextureBounds.y = GetRandomValue(0,4) * 16;
        this->characters.push_back(
            Character(npcWorldBounds,
                npcScreenBounds,
                new AnimatedSprite(
                    npcTextureBounds,
                    npcScreenBounds,
                    npcTexture,
                    npcAnimations,{(AnimationState){CharacterState::CHAR_IDLE,0,0,0}}
                    )));
                std::cerr << "generate character at " << npcScreenBounds.x
                    << ", " << npcScreenBounds.y
                    << " size " << npcScreenBounds.width
                    << " x " << npcScreenBounds.height 
                    << " in world at " << npcWorldBounds.x
                    << ", " << npcWorldBounds.y
                    << " size " << npcWorldBounds.width
                    << " x " << npcWorldBounds.height << "\n";
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
    const int MAX_OBJECTS = 42;
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
            Character(objectWorldBounds,
                objectScreenBounds,
                new AnimatedSprite(
                    objectTextureBounds,
                    objectScreenBounds,
                    objectTexture,
                    objectAnimations)));
    }
    std::cerr << this->objects.size() <<" Objects loaded" << std::endl;
}

void LevelScreen::loadTiles()
{
    this->tiles = TileMap(Datastore::getInstance().getTexture("images/tileMap.png"),{16.0f,16.0f},{4.0f,4.0f});
    const unsigned int tilesX = 50;
    const unsigned int tilesY = 50;
    this->levelSize = {tilesX, tilesY};
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

void LevelScreen::moveNPCs(float delta)
{
    /// TODO - leaving delta unused so I get the compiler warning
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
        for ( Character &character : this->characters )
        {
            character.screenBounds = LevelScreen::WorldToScreen(this, character.worldBounds);
            character.sprite->screenBounds = character.screenBounds;
        }
        for ( Character &obj : this->objects )
        {
            obj.screenBounds = LevelScreen::WorldToScreen(this, obj.worldBounds);
            obj.sprite->screenBounds = obj.screenBounds;
        }
    }

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

        int numTiles = this->tiles.draw();
        infoScreen->setNumTiles(numTiles);
        
        for ( Character &object : this->objects )
        {
            if ( object.sprite )
            {
                object.sprite->draw(delta);
            }
        }
        for ( Character &character : this->characters )
        {
            if ( character.sprite )
            {
                character.sprite->draw(delta);
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
