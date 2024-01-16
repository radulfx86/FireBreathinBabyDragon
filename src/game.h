#ifndef GAME_H
#define GAME_H
#include "types.h"
#include "animation.h"
#include <map>

class Game;

class GameState
{
public:
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update() = 0;
};

class Transition
{
public:
    using CallbackFunction_t = bool (*)(GameState *);
    Transition(GameState *source, GameState *target, CallbackFunction_t checkFunction)
        : source(source),
        target(target),
        callback(checkFunction) {}
    bool isApplicable(GameState *state) { return this->callback(state); }
    GameState *source;
    GameState *target;
private:
    CallbackFunction_t callback;
};

class GameScreen
{
public:
    GameScreen(Game *game) : game(game) {}
    virtual ~GameScreen() = default;

    virtual void initialize() = 0;

    virtual void draw() = 0;

    virtual void finalize() = 0;
protected:
    Game *game;
};

class Game
{
public:
    Game() : isRunning(false) {}
    ~Game() = default;

    void run();

    void stop();

    void propagateState();

private:
    GameState *state;

    void initialize();

    void finalize();

    std::vector<GameState*> states;

    std::vector<Transition> transitions;

    bool isRunning;
};

class SplashScreen : public GameScreen, public GameState
{
public:
    SplashScreen(Game *game) : GameScreen(game), isDone(false) {}
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

class ExitScreen : public GameScreen, public GameState
{
public:
    ExitScreen(float exitTime, Game *game) : GameScreen(game), exitTime(exitTime) {}
    virtual void initialize() override;

    virtual void draw() override;

    virtual void finalize() override;

    virtual void enter() override;

    virtual void exit() override;

    virtual void update() override;

private:
    float exitTime;
};

#endif // GAME_H
