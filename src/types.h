#ifndef TYPES_H
#define TYPES_H
#include "raylib.h"
#include <vector>
#include <utility>

class TileMap
{
public:
    TileMap() {}
    TileMap(Texture texture,
            Vector2 tileSize,
            Vector2 numTiles)
            : texture(texture),
                tileSize(tileSize),
                numTiles(numTiles)
                {}
    ~TileMap()
    {
        UnloadTexture(this->texture);
    }
    Texture texture;
    Vector2 tileSize;
    Vector2 numTiles;
    std::vector<std::pair<Rectangle, Rectangle>> coords;

    void draw()
    {
        for ( const std::pair<Rectangle,Rectangle>& coords : this->coords )
        {
            DrawTexturePro(
                this->texture, 
                coords.first,
                coords.second,
                {0.0,0.0},
                0.0,
                WHITE);
        }
    }
};

#endif // TYPES_H