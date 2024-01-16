#ifndef LEVEL_SCREEN_H
#define LEVEL_SCREEN_H
#include "game.h"

class LevelScreen : public GameScreen, public GameState
{
public:
    LevelScreen(Game *game) : GameScreen(game), isDone(false) {}
    virtual void initialize() override;

    virtual void draw() override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update() override;

    static bool checkDone(GameState *caller)
    {
        if ( LevelScreen *me = dynamic_cast<LevelScreen*>(caller) )
        {
            return me->isDone;
        }
        return false;
    }
private:
    Sprite_t dragonSprite;
    Sound fireBreath;
    bool isDone;
};

#endif // LEVEL_SCREEN_H