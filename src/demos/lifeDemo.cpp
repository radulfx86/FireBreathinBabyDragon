#include "demos.h"
#include <iostream>
#include <sstream>
#include "raylib.h"
#include "types.h"
#include "animation.h"

class PlayerState
{
public:
    PlayerState(int HP, int EP) : HP(HP), EP(EP) {}
    int HP;
    int EP;
};

class UI
{
public:
    UI(Vector2 pos, float scale, PlayerState *playerState):
        pos(pos),
        scale(scale),
        playerState(playerState)
    {
        this->texture = LoadTexture("images/ui.png");
        this->lifeSprite = 
        {
            // animation
        {
            // frameTimes
            { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 },
            // framePos
            {   {0.0,0.0,16.0,16.0},
                {16.0,0.0,16.0,16.0},
                {32.0,0.0,16.0,16.0},
                {48.0,0.0,16.0,16.0},
                {64.0,0.0,16.0,16.0},
                {80.0,0.0,16.0,16.0}
            },
            0,
            0.0
        },
        // position
        {0.0,0.0},
        // texOrigin
        {0.0,0.0},
        // sprite size
        {16.0 * scale,16.0 * scale},
        this->texture};
        this->fireSprite = 
        {
            // animation
        {
            // frameTimes
            { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 },
            // framePos
            {   {0.0,16.0,16.0,16.0},
                {16.0,16.0,16.0,16.0},
                {32.0,16.0,16.0,16.0},
                {48.0,16.0,16.0,16.0},
                {64.0,16.0,16.0,16.0},
                {80.0,16.0,16.0,16.0}
            },
            0,
            0.0
        },
        // position
        {100.0,0.0},
        // texOrigin
        {0.0,0.0},
        // sprite size
        {16.0 * scale,16.0 * scale},
        this->texture};
    }

    void draw(float delta)
    {
        drawChar(delta, &this->lifeSprite);
        DrawRectangleRec((Rectangle){40,16,this->playerState->HP, 20}, {172, 50, 50, 255});
        DrawRectangleLinesEx((Rectangle){40,16,this->playerState->HP, 20}, 2, {75, 22, 22, 255});
        std::stringstream hpText;
        hpText << this->playerState->HP;
        DrawText(hpText.str().c_str(), 42, 2, 30, LIGHTGRAY);

        drawChar(delta, &this->fireSprite);
        DrawRectangleRec((Rectangle){240,16,this->playerState->EP, 20}, {255, 134, 0, 255});
        DrawRectangleLinesEx((Rectangle){240,16,this->playerState->EP, 20}, 2, {186, 47, 0, 255});
        std::stringstream epText;
        epText << this->playerState->EP;
        DrawText(epText.str().c_str(), 242, 2, 30, LIGHTGRAY);
    }
private:
    Vector2 pos;
    float scale;
    Texture texture;
    PlayerState *playerState;
    Sprite_t lifeSprite;
    Sprite_t fireSprite;
};

class LifeGrid
{
public:
    LifeGrid(Vector2 gridSize, float scaleFactor, TileMap *tileMap, Vector2 enabledCoords, Vector2 disabledCoords)
    : gridSize(gridSize), tileMap(tileMap), enableCoords(enabledCoords), disabledCoords(disabledCoords)
    {
        enabled.resize(gridSize.y, std::vector<bool>(gridSize.x));
        coordToIndexMap.resize(gridSize.y, std::vector<int>(gridSize.x));
        for ( int y = 0; y < gridSize.y; ++y )
        {
            for ( int x = 0; x < gridSize.x; ++x )
            {
                enabled[y][x] = false;
            }
        }
        int c = 0;
        for ( int y = 0; y < gridSize.y; ++y )
        {
            for ( int x = 0; x < gridSize.x; ++x )
            {
                coordToIndexMap[y][x] = c++;
                /// source rectangle in source-px-coordinates
                tileMap->coords.push_back((std::pair<Rectangle, Rectangle>){{
                    disabledCoords.x * tileMap->tileSize.x,
                    disabledCoords.y * tileMap->tileSize.y,
                    tileMap->tileSize.x,
                    tileMap->tileSize.y},
                /// target rectangle in target-px-coordinates (scaled)
                {
                    x * tileMap->tileSize.x * scaleFactor,
                    y * tileMap->tileSize.y * scaleFactor,
                    tileMap->tileSize.x * scaleFactor,
                    tileMap->tileSize.y * scaleFactor}});
            }
        }
    }

    void draw()
    {
        tileMap->draw();
    }

    void update()
    {
        /// this simulates a "game-of-life".
        /// in the "real" game, the sumEnabled(player.pos) can be used to determine the increase/decrease of power
        std::vector<std::vector<bool>> newEnabled;
        newEnabled.resize(gridSize.y, std::vector<bool>(gridSize.x));
        
        for ( int y = 0; y < enabled.size(); ++y )
        {
            for ( int x = 0; x < enabled[y].size(); ++x )
            {
                int sum = sumEnabled(x,y);
                newEnabled[y][x] = sum > 1 && sum < 4;
                int coordIndex = coordToIndexMap[y][x];
                if ( newEnabled[y][x] )
                {
                    tileMap->coords[coordIndex].first.x = enableCoords.x * tileMap->tileSize.x;
                    tileMap->coords[coordIndex].first.y = enableCoords.y * tileMap->tileSize.y;
                }
                else
                {
                    tileMap->coords[coordIndex].first.x = disabledCoords.x * tileMap->tileSize.x;
                    tileMap->coords[coordIndex].first.y = disabledCoords.y * tileMap->tileSize.y;
                }
            }
        }

        enabled = newEnabled;
    }

    void init()
    {
        for ( int i = 0; i < 42; ++i )
        {
            int x = GetRandomValue(0, gridSize.x-1);
            int y = GetRandomValue(0, gridSize.y-1);
            enabled[y][x] = true;
        }
    }
private:
    Vector2 gridSize;
    TileMap *tileMap;
    Vector2 enableCoords, disabledCoords;
    std::vector<std::vector<bool>> enabled;
    std::vector<std::vector<int>> coordToIndexMap;

    int sumEnabled(int x, int y)
    {
        int sum = 0;
        for ( int iY = -1; iY < 2; ++iY )
        {
            int ty = iY + y;
            if ( ty < 0 || ty >= enabled.size() )
            {
                continue;
            }
            for ( int iX = -1; iX < 2; ++iX )
            {
                int tx = iX + x;
                if ( tx < 0 || tx >= enabled[y].size() )
                {
                    continue;
                }
                sum += enabled[ty][tx] ? 1 : 0;
            }
        }
        return sum;
    }
};

void showLifeDemo()
{
    const int screenWidth = 32 * 24;
    const int screenHeight = 32 * 20;
    const float scaleFactor = 2;

    PlayerState playerState(20,30);

    LifeGrid lifeGrid({24,20}, scaleFactor, new TileMap(LoadTexture("images/tileMap.png"), {16.0,16.0}, {4,4}), {3,0}, {3,2});
    lifeGrid.init();

    UI ui({0,0},1.0,&playerState);

    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Life Demo");
    SetTargetFPS(10);
    float sumDelta = 0.0;
    int frameCnt = 0;
    while (!WindowShouldClose())
    {
            float delta = GetFrameTime();
            sumDelta += delta;
            ++frameCnt;
            if ( sumDelta > 1.0 )
            {
                std::cout << "FPS: " << frameCnt << "\n";
                sumDelta -= 1.0;
                frameCnt = 0;
                lifeGrid.update();
                playerState.EP = (playerState.EP + 1 ) % 200;
                playerState.HP = (playerState.HP + 2 ) % 100;
            }
        BeginDrawing();
            ClearBackground(BLACK);
            lifeGrid.draw();
            ui.draw(delta);
        EndDrawing();
    }
}