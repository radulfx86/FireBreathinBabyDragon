#include "demos.h"
#include <iostream>
#include "raylib.h"

void showTileDemo()
{
    Texture tileMap = LoadTexture("images/tileMap.png");
    const int screenWidth = 800;
    const int screenHeight = 600;
    const Vector2 tileSize = {16.0, 16.0};
    const float scaleFactor = 4;
    const int numScreenTilesX = screenWidth / (tileSize.x * scaleFactor);
    const int numScreenTilesY = screenHeight / (tileSize.y * scaleFactor);
    const int numMapTilesX = tileMap.width / tileSize.x;
    const int numMapTilesY = tileMap.height / tileSize.y;
    Rectangle tileSource[numScreenTilesX][numScreenTilesY];
    for ( int y = 0; y < numScreenTilesY; ++y )
    {
        for ( int x = 0; x < numScreenTilesX; ++x )
        {
            tileSource[x][y].width = tileSize.x;
            tileSource[x][y].height = tileSize.y;
            tileSource[x][y].x = tileSize.x * (x % numMapTilesX);
            tileSource[x][y].y = tileSize.y * (y % numMapTilesY);
        }
    }
    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Tilemap Demo");
    //SetTargetFPS(10);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            float delta = GetFrameTime();
            std::cout << "delta: " << delta << " s\n";
            std::cout << "FPS: " << 1.0 / delta << "\n";
            ClearBackground(BLACK);
            Rectangle tileDest = {0.0, 0.0, tileSize.x * scaleFactor, tileSize.y * scaleFactor};
            Vector2 origin = {0.0,0.0};
            for ( int y = 0; y < numScreenTilesY; ++y )
            {
                for ( int x = 0; x < numScreenTilesX; ++x )
                {
//                    DrawTextureRec(tileMap, tileSource[x][y], Vector2({(float)(x * tileSize.x * scaleFactor), (float)(y * tileSize.y * scaleFactor)}), WHITE);
                    tileDest.x = x * tileSize.x * scaleFactor;
                    tileDest.y = y * tileSize.y * scaleFactor;
                    DrawTexturePro(
                        tileMap, 
                        tileSource[x][y],
                        tileDest,
                        origin,
                        0.0,
                        WHITE);
                }
            }
        EndDrawing();
    }
}