#ifndef TYPES_H
#define TYPES_H
#include "raylib.h"
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>

/// @brief collection of a number of tiles
/// TODO extend to use "Sprites" or AnimatedSprites instead
class TileMap
{
public:
    TileMap() {}
    TileMap(Texture texture,
            Vector2 tileSize,
            Vector2 numTiles)
            : texture(texture),
                tileSize(tileSize),
                numTiles(numTiles),
                viewport({0,0,tileSize.x * numTiles.x, tileSize.y * numTiles.y}),
                viewScale(1.0)
                {}
    ~TileMap()
    {
    //    UnloadTexture(this->texture);
    }
    Texture texture;
    Vector2 tileSize;
    Vector2 numTiles;
    std::vector<std::pair<Rectangle, Rectangle>> coords;
    Rectangle viewport;
    float viewScale;

    int draw()
    {
        int countVisTiles = 0;
        for ( const std::pair<Rectangle,Rectangle>& coords : this->coords )
        {
            if ( CheckCollisionRecs(this->viewport, coords.second))
            {
                ++countVisTiles;
                Rectangle scaledTarget = {
                    coords.second.x / viewScale,
                    coords.second.y / viewScale,
                    coords.second.width / viewScale,
                    coords.second.height / viewScale};
                DrawTexturePro(
                    this->texture, 
                    coords.first,
                    scaledTarget,
                    {0.0,0.0},
                    0.0,
                    WHITE);
            }
        }
        DrawRectangleLinesEx(this->viewport, 1.0, RED);
        return countVisTiles;
    }
    /// @brief apply camera transformation to the image section
    /// @note TODO there is something wrong, zoom works inverse??
    /// @param camera 
    void updateCamera(Camera2D camera)
    {
        std::cerr << __func__ << " cam: " << camera.target.x << " " << camera.target.y << "\n";
        this->viewport = {camera.offset.x * camera.zoom,
                        camera.offset.y * camera.zoom,
                        camera.target.x * camera.zoom,
                        camera.target.y * camera.zoom};
        this->viewScale = camera.zoom;
    }
    void ySort()
    {
        // sort by y-value
        std::sort(this->coords.begin(), this->coords.end(),
            [](std::pair<Rectangle, Rectangle> a, std::pair<Rectangle, Rectangle> b) {
                return a.second.y < b.second.y;
            }
        );
    }
    static void initBackground(TileMap *background, Vector2 screenSize, float scaleFactor, Vector2 defaultTile)
    {
        const int numScreenTilesX = screenSize.x / (background->tileSize.x * scaleFactor);
        const int numScreenTilesY = screenSize.y / (background->tileSize.y * scaleFactor);

        std::cerr << "background of size: " << numScreenTilesX << " x " << numScreenTilesY << "\n";
        std::cerr << "background of size: " << screenSize.x << " x " << screenSize.y << "\n";
        std::cerr << "tileSize: " << background->tileSize.x << " x " << background->tileSize.y << "\n";
        exit(3);
        /// initialize a tile map for the screen
        for ( int y = 0; y < numScreenTilesY; ++y )
        {
            for ( int x = 0; x < numScreenTilesX; ++x )
            {
                /// source rectangle in source-px-coordinates
                background->coords.push_back((std::pair<Rectangle, Rectangle>){{
                    defaultTile.x * background->tileSize.x,
                    defaultTile.y * background->tileSize.y,
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
};

#endif // TYPES_H