#ifndef LEVEL_SCREEN_H
#define LEVEL_SCREEN_H
#include "game.h"

class InfoScreen
{
public:
    InfoScreen(Vector2 origin) : origin(origin), sumDelta(0), frameCnt(0), numTiles(0) {}
    void draw(float delta);
    std::string scaleText;
    void setNumTiles(int numTiles);
private:
    Vector2 origin;
    float sumDelta;
    unsigned int frameCnt;
    std::string fpsText;
    std::string tileText;
    int numTiles;
};

class LevelScreen : public GameScreen, public GameState
{
public:
    LevelScreen(Game *game) : GameScreen(game), isDone(false), scale(1), offset({0,0})
    //, tileSize({16,16})
     {}
    virtual void initialize() override;

    virtual void draw(float delta) override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update(float delta) override;

    static bool checkDone(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->isDone;
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
private:
    AnimatedSprite *dragonSprite;
    Vector2 charSpeed;
    float charSpeedMax;
    float charAcc;
    Sound fireBreath;
    bool isDone;
    float scale;
    /// offset - in world-coordinates for now
    Vector2 offset;
    //Vector2 tileSize;
    TileMap tiles;
    InfoScreen *infoScreen;
    void loadCharacters();
    void loadObjects();
    void loadTiles();
    void movePlayer(float delta);
    void moveNPCs(float delta);
    Camera2D camera;
    Vector2 levelSize;

    std::vector<Character> characters;
    std::vector<Character> objects;

    Texture2D objectTexture, npcTexture;
};

#endif // LEVEL_SCREEN_H