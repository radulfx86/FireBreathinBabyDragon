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
    virtual void update(float delta) = 0;
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

    virtual void draw(float delta) = 0;

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


#include <iostream>
#define TRACE ;
#if 0
do { \
    std::cerr << __func__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
} while(0)

#endif


#endif // GAME_H
