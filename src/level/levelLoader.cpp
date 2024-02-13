#include "levelLoader.h"
#include "levelScreen.h"
#include "datastore.h"
#include "level.h"
#include <string>
#include <execution>

Level *LevelLoader::loadLevelFromImage(std::string path, LevelScreen *screen)
{
    Level *level = new Level(screen);
    Image levelImageData = LoadImage(path.c_str());
    
    level->levelSize = {static_cast<float>(levelImageData.width),
                        static_cast<float>(levelImageData.height)};

    level->objectTexture = Datastore::getInstance().getTexture("images/assets.png");
    level->npcTexture = Datastore::getInstance().getTexture("images/villagers.png");
     
    const unsigned int tilesX = level->levelSize.x;
    const unsigned int tilesY = level->levelSize.y;
    screen->tiles = TileMap(Datastore::getInstance().getTexture("images/tileMap.png"),{16.0f,16.0f},{4.0f,4.0f});
    level->distanceMaps[DistanceMapType::PLAYER_DISTANCE].resize(tilesX,std::vector<int>(tilesY));
    float tileScaleFactor = 1.0;
    
    for ( unsigned int y = 0; y < levelImageData.height; ++y )
    {
        for ( unsigned int x = 0; x < levelImageData.width; ++x )
        {
            Color levelColor = GetImageColor(levelImageData, x, y);
            if ( levelColor.r == 255 && levelColor.g == 0 && levelColor.b == 0 )
            {
                /// add Player
                level->addCharacter(CharacterType::PLAYER, x, y);
            }
            if ( levelColor.r == 0 && levelColor.g == 0 && levelColor.b == 255 )
            {
                /// add Mage
                level->addCharacter(CharacterType::MAGE, x, y);
            }
            if ( levelColor.r == 255 && levelColor.g == 255 && levelColor.b == 0 )
            {
                /// add Villager
                level->addCharacter(CharacterType::VILLAGER, x, y);
            }
            if ( levelColor.r == 0 && levelColor.g == 149 && levelColor.b == 255 )
            {
                /// add Guard
                level->addCharacter(CharacterType::GUARD, x, y);
            }
            if ( levelColor.r == 255 && levelColor.g == 0 && levelColor.b == 215 )
            {
                /// add Hero
                level->addCharacter(CharacterType::HERO, x, y);
            }
            if ( levelColor.r == 75 && levelColor.g == 105 && levelColor.b == 47 )
            {
                /// add Tree
                level->addObject(ObjectType::TREE, x, y);
            }
            if ( levelColor.r == 203 && levelColor.g == 219 && levelColor.b == 252 )
            {
                /// add House
                level->addObject(ObjectType::HOUSE, x, y);
            }
            /// TILES:
            Vector2 defaultTile = {(float)(x%1),0};//(float)(y%2)};
            /// source rectangle in source-px-coordinates
            screen->tiles.coords.push_back((std::pair<Rectangle, Rectangle>){{
                defaultTile.x * screen->tiles.tileSize.x,
                defaultTile.y * screen->tiles.tileSize.y,
                screen->tiles.tileSize.x,
                screen->tiles.tileSize.y},
            /// target rectangle in target-px-coordinates (scaled)
            {
                x * screen->tiles.tileSize.x * tileScaleFactor,
                y * screen->tiles.tileSize.y * tileScaleFactor,
                screen->tiles.tileSize.x * tileScaleFactor,
                screen->tiles.tileSize.y * tileScaleFactor}});
        }
    }
    UnloadImage(levelImageData);

    for (int type = 0; type < 4; ++type )
    {
        level->distanceMaps[static_cast<DistanceMapType>(type)].resize(level->levelSize.x,std::vector<int>(level->levelSize.y));
        //updateDistanceMap(static_cast<DistanceMapType>(i), {level->levelSize.x/2, level->levelSize.y/2});
                std::for_each(std::execution::par_unseq,
                    level->distanceMaps[type].begin(),
                    level->distanceMaps[type].end(), 
                    [](std::vector<int> &dist){
//                        std::memset(&dist[0], -1, sizeof(int)*dist.size());
                        std::for_each(std::execution::par_unseq,
                                        dist.begin(),
                                        dist.end(),
                                        [](int &n) { n = -1; });
        });
    }
    // find a better solution to find villages !!!
    for ( auto tmp : level->objects )
    {
        Character *obj = tmp.second;
        if ( obj->sprite->textureBounds.y == 0  // HOUSE
            || obj->sprite->textureBounds.y == 1 ) // TOWER
        {
            level->updateDistanceMap(DistanceMapType::VILLAGE_DISTANCE, {obj->worldBounds.x+1, obj->worldBounds.y+1}, false, true, 10);
        }
    }
    return level;
}