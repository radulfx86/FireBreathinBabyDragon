#ifndef GAME_H
#define GAME_H
#include "types.h"
#include "animation.h"
#include <map>

class Game;

using GameState = enum {NONE = 0,
                        INITIALIZED = 1,
                        SPLASH = 2,
                        LEVEL = 3,
                        MENU = 4,
                        GAMEOVER = 5,
                        EXIT = 6,
                        DONE = 7};

class GameScreen
{
public:
    GameScreen(Game *game) : game(game) {}
    virtual ~GameScreen() = default;

    virtual void initialize() = 0;

    virtual void enter() = 0;

    virtual void draw() = 0;

    virtual void exit() = 0;

    virtual void finalize() = 0;
protected:
    Game *game;
};

class Game
{
public:
    Game() : state(GameState::NONE), oldState(GameState::NONE) {}
    ~Game() = default;

    void run();

    void propagateState();

    void drawScreen();

    GameState state;
private:

    void initialize();

    void finalize();

    std::map<GameState, GameScreen*> screens;

    GameState oldState;
};

class SplashScreen : public GameScreen
{
public:
    SplashScreen(Game *game) : GameScreen(game) {}
    virtual void initialize() override;

    virtual void enter() override;

    virtual void draw() override;

    virtual void exit() override;

    virtual void finalize() override;
private:
    Sprite_t dragonSprite;
    Sound fireBreath;
};

class LevelScreen : public GameScreen
{
public:
    LevelScreen(Game *game) : GameScreen(game) {}
    virtual void initialize() override;

    virtual void enter() override;

    virtual void draw() override;

    virtual void exit() override;

    virtual void finalize() override;
private:
    Sprite_t dragonSprite;
    Sound fireBreath;
};

class ExitScreen : public GameScreen
{
public:
    ExitScreen(float exitTime, Game *game) : GameScreen(game), exitTime(exitTime) {}
    virtual void initialize() override;

    virtual void enter() override;

    virtual void draw() override;

    virtual void exit() override;

    virtual void finalize() override;
private:
    float exitTime;
};

#endif // GAME_H
