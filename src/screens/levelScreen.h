#ifndef LEVEL_SCREEN_H
#define LEVEL_SCREEN_H
#include "game.h"
#include "ui.h"
#include <unordered_map>
#define MAX_LEVEL_SIZE_X 1000u
#define MAX_LEVEL_SIZE_Y 1000u

using GridPos = struct {int x; int y;};
bool operator<(const GridPos &a, const GridPos &b)
{
    return (a.x + MAX_LEVEL_SIZE_X * a.y) < (b.x + MAX_LEVEL_SIZE_X * b.y);
}
bool operator==(const GridPos &a, const GridPos &b)
{
    return (a.x + MAX_LEVEL_SIZE_X * a.y) == (b.x + MAX_LEVEL_SIZE_X * b.y);
}
bool operator!=(const GridPos &a, const GridPos &b)
{
    return !(a==b);
}
GridPos operator+(const GridPos &a, const GridPos &b)
{
    return (GridPos){a.x+b.x, a.y+b.y};
}

using ObjectType = enum {
    HOUSE = 0,
    TREE = 1,
    TOWER = 2,
    WALL = 3
};

using CharacterType = enum {
    PLAYER = 0,
    VILLAGER = 1,
    GUARD = 2,
    MAGE = 3,
    HERO = 4
};

/// info screen
class InfoScreen
{
public:
    InfoScreen(Vector2 origin) : origin(origin), sumDelta(0), frameCnt(0), numTiles(0) {}
    void draw(float delta);
    std::string scaleText;
    void setNumTiles(int numTiles);
    void setActiveDistanceMap(DistanceMapType selectedDebugDistanceMap);
private:
    Vector2 origin;
    float sumDelta;
    unsigned int frameCnt;
    std::string fpsText;
    std::string tileText;
    int numTiles;
    std::string distanceMapText;
    DistanceMapType selectedDebugDistanceMap;
};

/// level screen
class LevelScreen : public GameScreen, public GameState
{
public:
    LevelScreen(Game *game) : GameScreen(game), isDone(false), isGameOver(false), scale(1), offset({0,0}), selectedDebugDistanceMap(DistanceMapType::PLAYER_DISTANCE), lastPlayerGridPos({0,0})
    //, tileSize({16,16})
     {}
    virtual void initialize() override;

    virtual void draw(float delta) override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update(float delta) override;

    static bool checkWin(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->isDone;
        }
        return false;
    }
    static bool checkGameover(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->isGameOver;
        }
        return false;
    }
    /**
     * @brief convert coords on screen (e.g. mouse) to world coordinates
     * @note check computation, especially usage and unit-space of offset
     * 
     * @param level         pointer to level/screen
     * @param screenCoords  rectangle containing position and size of screen object
     * @return Rectangle 
     */
    static Rectangle ScreenToWorld(LevelScreen *level, Rectangle screenCoords)
    {
        if ( level && (level->scale != 0.0 ))
        {
            return {(screenCoords.x - level->offset.x) / (level->scale * level->tiles.tileSize.x),
                (screenCoords.y - level->offset.y) / (level->scale * level->tiles.tileSize.y),
                screenCoords.width / (level->scale * level->tiles.tileSize.x),
                screenCoords.height / (level->scale * level->tiles.tileSize.y)};
        }
        else
        {
            return {0,0,0,0};
        }
    }
    static Vector2 ScreenToWorld(LevelScreen *level, Vector2 screenCoords)
    {
        if ( level && (level->scale != 0.0 ))
        {
            return {(screenCoords.x - level->offset.x) / (level->scale * level->tiles.tileSize.x),
                (screenCoords.y - level->offset.y) / (level->scale * level->tiles.tileSize.y)};
        }
        else
        {
            return {0,0};
        }
    }
    /**
     * @brief convert coords from world to screen
     * @note check computation, especially usage and unit-space of offset
     * 
     * @param level         pointer to level/screen
     * @param worldCoords   rectangle containing position and size of object in world 
     * @return Rectangle 
     */
    static Rectangle WorldToScreen(LevelScreen *level, Rectangle worldCoords)
    {
        if ( level )
        {
#if 0
            std::cerr << __func__ << "(" << worldCoords.x << " " << worldCoords.y << " " << worldCoords.width << " " << worldCoords.height << ")"
                << " tileSize: " << level->tiles.tileSize.x << " " << level->tiles.tileSize.y
                << " scale: " << level->scale << "\n";
#endif
            return {(level->offset.x + worldCoords.x) * level->scale * level->tiles.tileSize.x,
                (level->offset.y + worldCoords.y) * level->scale * level->tiles.tileSize.y,
                (worldCoords.width * level->scale) * level->tiles.tileSize.x,
                (worldCoords.height * level->scale) * level->tiles.tileSize.y};
        }
        else
        {
            return {0,0,0,0};
        }
    }
    static Vector2 WorldToScreenPos(LevelScreen *level, Vector2 worldCoords)
    {
        if ( level )
        {
            return {(level->offset.x + worldCoords.x) * level->scale * level->tiles.tileSize.x,
                (level->offset.y + worldCoords.y) * level->scale * level->tiles.tileSize.y};
        }
        else
        {
            return {0,0};
        }
    }
private:
    /// TODO total mess below, CLEAN UP @Radulf
    Vector2 charSpeed;
    float charSpeedMax;
    float charAcc;
    int winThreshold;
    Sound fireBreath;
    bool isDone;
    bool isGameOver;
    float scale;
    /// offset - in world-coordinates for now
    Vector2 offset;
    //Vector2 tileSize;
    TileMap tiles;
    InfoScreen *infoScreen;
    void loadLevel();
    void addCharacter(CharacterType charType, int x, int y);
    void addObject(ObjectType objType, int x, int y);
    void loadSounds();
    void loadUI();
    void movePlayer(float delta);
    void moveNPCs(float delta);
    void updateNPCs(float delta);
    /// TODO merge with updateNPCs
    void updateObjects(float delta);
    bool checkCollision(Character *source, Rectangle worldBounds);
    Character* getCollision(Character *source, Rectangle worldBounds);
    void updateDistanceMap(DistanceMapType selectedDistanceMap, Vector2 worldTargetPos);
    void updateDistanceMap(DistanceMapType type, GridPos pos, bool clean, bool ignoreObjects, int distMax);
    void sortDrawableObjects();
    void checkWinCondition();
    Camera2D camera;
    Vector2 levelSize;

    std::vector<Character*> characters;
    /// TBD: is there no way to actually use the tileMap for this?
    std::map<GridPos,Character*> objects;
    std::vector<Character*> drawableObjects;
    /// this is ugly
    GridPos lastPlayerGridPos;
    Character *player;

    UI *ui;

    Texture2D objectTexture, npcTexture;

    /// distance maps - mapped per distance-type
    MappedDistanceMaps distanceMaps;
    
    DistanceMapType selectedDebugDistanceMap;

};

#endif // LEVEL_SCREEN_H