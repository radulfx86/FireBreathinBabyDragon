#ifndef EXIT_SCREEN_H
#define EXIT_SCREEN_H
#include "game.h"

class ExitScreen : public GameScreen, public GameState
{
public:
    ExitScreen(float exitTime, Game *game) : GameScreen(game), exitTime(exitTime) {}
    virtual void initialize() override;

    virtual void draw(float delta) override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update(float delta) override;

private:
    float exitTime;
};
#endif // EXIT_SCREEN_H