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
    LevelScreen(Game *game) : GameScreen(game), isDone(false), scale(1) {}
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
private:
    Sprite_t *dragonSprite;
    Vector2 charSpeed;
    float charSpeedMax;
    float charAcc;
    Sound fireBreath;
    bool isDone;
    float scale;
    TileMap tiles;
    InfoScreen *infoScreen;
    void loadCharacters();
    void loadTiles();
    void movePlayer(float delta);
    Camera2D camera;
};

#endif // LEVEL_SCREEN_H