#include "levelScreen.h"
#include "datastore.h"
#include "villagerStrategy.h"
#include "objectStrategy.h"
#include "debugStats.h"
#include <sstream>
#include <cmath>
#include <execution>
#include <queue>
#include <cstring>
#include "levelLoader.h"

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

void InfoScreen::setScale(float scale)
{
    std::stringstream scaleStr;
    scaleStr << "scale: " << scale;
    this->scaleText = scaleStr.str();
}
void LevelScreen::loadUI()
{
    this->ui = new UI({0,0}, 2.0, &this->level->player->stats);
}

void LevelScreen::initialize()
{
    TRACE;
    SetWindowSize(640, 480);
    this->camera.offset = {0,0};
    this->camera.rotation = 0;
    this->camera.target = {640, 480};
    this->camera.zoom = 1.0;
    SetTargetFPS(-1);

    this->infoScreen = new InfoScreen({400,10});

    this->level = LevelLoader::loadLevelFromImage("images/testLevel.png", this);

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

void LevelScreen::handleInput(float delta)
{
#if NOT_NECESSARY_REMOVE_LATER 
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
#endif
    static bool isKeyEPressed = true;
    if ( IsKeyDown(KEY_E) )
    {
        if ( !isKeyEPressed )
        {
            float dmg = 50.0;
            if ( dmg > this->level->player->stats.EP )
            {
                dmg = this->level->player->stats.EP;
            }
            this->level->player->stats.EP -= dmg;
            if ( dmg > 0 )
            {
                Vector2 target = LevelScreen::ScreenToWorld(this,GetMousePosition());
                Vector2 dir = {target.x - this->level->player->worldBounds.x, target.y - this->level->player->worldBounds.y};
                float len = sqrt(dir.x * dir.x + dir.y * dir.y)/2.0;
                dir.x /= len;
                dir.y /= len;
                this->level->launchProjectile(dmg, this->level->player->worldBounds, dir, len, new AnimatedSprite(
                    // texture bounds
                    {0.0,16.0,16.0,16.0},
                    // screen bounds
                    LevelScreen::WorldToScreen(this, this->level->player->worldBounds),
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
            }
        }
        isKeyEPressed = true;
    }
    else
    {
        isKeyEPressed = false;
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
        this->infoScreen->setScale(this->scale);
        needsUpdate = true;
    }
    // since zoom does not work:
    needsUpdate = false;

    if ( needsUpdate )
    {
        this->tiles.updateCamera(this->camera);
        for ( Character *character : this->level->characters )
        {
            character->screenBounds = LevelScreen::WorldToScreen(this, character->worldBounds);
            character->sprite->screenBounds = character->screenBounds;
        }
        for ( auto tmp : this->level->objects )
        {
            Character *obj = tmp.second;
            obj->screenBounds = LevelScreen::WorldToScreen(this, obj->worldBounds);
            obj->sprite->screenBounds = obj->screenBounds;
        }
    }

}

void LevelScreen::update(float delta)
{
    if ( not this->level->isReady() )
    {
        return;
    }
    TRACE;
    // check end of game
    this->level->checkWinCondition();

    // update object states
    handleInput(delta);
    this->level->update(delta);

    // draw
    this->draw(delta);
}
void LevelScreen::draw(float delta)
{
    TRACE;
    BeginDrawing();
        ClearBackground(GREEN);

#if WAITING_FOR_REFACTORING
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
#endif        


        this->level->draw(delta, this);

        this->ui->draw(delta);

        this->infoScreen->draw(delta);
    EndDrawing();

}
