#ifndef LEVEL_SCREEN_H
#define LEVEL_SCREEN_H
#include "game.h"
#include "level.h"
#include "particle.h"
#include "ui.h"
#include <unordered_map>

/// info screen, collect and display various information on current screen
class InfoScreen
{
public:
    /// @brief info screen constructor
    /// @param origin   origin of infoscreen overlay in screen coordinates 
    InfoScreen(Vector2 origin) : origin(origin), sumDelta(0), frameCnt(0), numTiles(0) {}
    /// draw screen
    void draw(float delta);
    void setNumTiles(int numTiles);
    void setActiveDistanceMap(DistanceMapType selectedDebugDistanceMap);
    void setScale(float scale);
private:
    Vector2 origin;
    float sumDelta;
    unsigned int frameCnt;
    std::string fpsText;
    std::string tileText;
    std::string scaleText;
    int numTiles;
    std::string distanceMapText;
    DistanceMapType selectedDebugDistanceMap;
};

/// level screen - absolutely bloated monster of game and screen combination @todo split/refactor!!!!
class LevelScreen : public GameScreen, public GameState
{
public:
    LevelScreen(Game *game) :
        GameScreen(game),
        scale(1),
        offset({0,0}),
        selectedDebugDistanceMap(DistanceMapType::PLAYER_DISTANCE),
        level(this)
     {std::cerr << __func__ << " this=" << static_cast<const void*>(this) << "\n";}
    virtual void initialize() override;

    virtual void draw(float delta) override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update(float delta) override;

    /// static check win condition
    static bool checkWin(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->level.isDone;
        }
        return false;
    }
    /// static check gameover condition
    static bool checkGameover(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->level.isGameOver;
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
    TileMap tiles; ///< move to Level?
private:
    Level level;
    /// TODO total mess below, CLEAN UP @Radulf
    DistanceMapType selectedDebugDistanceMap;
    /// stuff to go in level
    /// stuff that can remain in screen
    void loadUI();
    UI *ui;
    float scale;
    InfoScreen *infoScreen;
    /// offset - in world-coordinates for now
    Vector2 offset;


    void handleInput(float delta); ///< insert all input stuff here
    /// TODO merge with updateNPCs
    Camera2D camera;

};

#endif // LEVEL_SCREEN_H