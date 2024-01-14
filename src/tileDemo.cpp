#include "demos.h"
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "raylib.h"
#include "types.h"

void ySort(TileMap *tiles)
{
    // sort by y-value
    std::sort(tiles->coords.begin(), tiles->coords.end(),
        [](std::pair<Rectangle, Rectangle> a, std::pair<Rectangle, Rectangle> b) {
            return a.second.y < b.second.y;
        }
    );
}

void initBackground(const int screenWidth, const int screenHeight, const float scaleFactor, TileMap *background)
{
    background->texture = LoadTexture("images/tileMap.png");
    background->tileSize = {16.0, 16.0};
    background->numTiles = {4, 4};

    const int numScreenTilesX = screenWidth / (background->tileSize.x * scaleFactor);
    const int numScreenTilesY = screenHeight / (background->tileSize.y * scaleFactor);

    /// initialize a tile map for the screen
    for ( int y = 0; y < numScreenTilesY; ++y )
    {
        for ( int x = 0; x < numScreenTilesX; ++x )
        {
            /// source rectangle in source-px-coordinates
            background->coords.push_back((std::pair<Rectangle, Rectangle>){{
                3 * background->tileSize.x,
                1 * background->tileSize.y,
                background->tileSize.x,
                background->tileSize.y},
            /// target rectangle in target-px-coordinates (scaled)
            {
                x * background->tileSize.x * scaleFactor,
                y * background->tileSize.y * scaleFactor,
                background->tileSize.x * scaleFactor,
                background->tileSize.y * scaleFactor}});
        }
    }
}

void initForeground(const int screenWidth, const int screenHeight, const float scaleFactor, TileMap *foreground)
{
    foreground->texture = LoadTexture("images/assets.png");
    foreground->tileSize = {16.0, 32.0};
    foreground->numTiles = {10, 5};
    static const int MAX_ELEMENTS = 100;
    for ( int i = 0; i < MAX_ELEMENTS; ++i )
    {
        float x = GetRandomValue(0, foreground->numTiles.x) * scaleFactor * foreground->tileSize.x;
        float y = GetRandomValue(0, foreground->numTiles.y) * scaleFactor * foreground->tileSize.y/2;
        foreground->coords.push_back({{0,(i%(int)foreground->numTiles.y) * foreground->tileSize.y,foreground->tileSize.x, foreground->tileSize.y},
        {x,y,foreground->tileSize.x * scaleFactor, foreground->tileSize.y * scaleFactor}});
    }
    // sort by z (in this case y)-coordinate
    ySort(foreground);
}

void showTileDemo()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    const float scaleFactor = 4;
    TileMap background;
    initBackground(screenWidth, screenHeight, scaleFactor, &background);

    TileMap foreground;
    initForeground(screenWidth, screenHeight, scaleFactor, &foreground);

    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Tilemap Demo");
    //SetTargetFPS(10);
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
            }
        BeginDrawing();
            ClearBackground(BLACK);
            background.draw();
            foreground.draw();
        EndDrawing();
    }
}