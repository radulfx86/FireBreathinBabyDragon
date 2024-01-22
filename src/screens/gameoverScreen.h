#ifndef GAMEOVER_SCREEN_H
#define GAMEOVER_SCREEN_H
#include "game.h"

class GameoverScreen : public GameScreen, public GameState
{
public:
    GameoverScreen(float exitTime, Game *game) : GameScreen(game), exitTime(exitTime) {}
    virtual void initialize() override;

    virtual void draw(float delta) override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update(float delta) override;

private:
    float exitTime;
};
#endif // GAMEOVER_SCREEN_H