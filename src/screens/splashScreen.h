#ifndef SPLASH_SCREEN_H
#define SPLASH_SCREEN_H
#include "game.h"

class SplashScreen : public GameScreen, public GameState
{
public:
    SplashScreen(Game *game) : GameScreen(game), isDone(false) {}
    virtual ~SplashScreen() = default;
    virtual void initialize() override;

    virtual void draw() override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update() override;

    static bool checkDone(GameState *caller)
    {
        if ( SplashScreen *me = dynamic_cast<SplashScreen*>(caller) )
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
#endif // SPLASH_SCREEN_H