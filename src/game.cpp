#include "demos.h"
#include <iostream>
#include <sstream>
#include <map>
#include "animation.h"
#include "game.h"
#include "splashScreen.h"
#include "levelScreen.h"
#include "exitScreen.h"
#include "gameoverScreen.h"

void Game::initialize()
{
    TRACE;
    InitAudioDevice();
    SplashScreen *splashScreen = new SplashScreen(this);
    LevelScreen *levelScreen = new LevelScreen(this);
    ExitScreen *exitScreen = new ExitScreen(3.0, this);
    GameoverScreen *gameoverScreen = new GameoverScreen(4.0, this);
    this->states.push_back(splashScreen);
    this->states.push_back(levelScreen);
    this->states.push_back(exitScreen);
    this->states.push_back(gameoverScreen);
    this->transitions.push_back(Transition(splashScreen, levelScreen, SplashScreen::checkWin));
    this->transitions.push_back(Transition(levelScreen, exitScreen, levelScreen->checkWin));
    this->transitions.push_back(Transition(levelScreen, gameoverScreen, levelScreen->checkGameover));
    for ( auto state : this->states )
    {
        if ( GameScreen *screen = dynamic_cast<GameScreen*>(state) )
        {
            screen->initialize();
        }
    }
    this->state = splashScreen;
    this->isRunning = true;
}

void Game::stop()
{
    TRACE;
    this->isRunning = false;
}

void Game::finalize()
{
    TRACE;
    for ( auto state : this->states )
    {
        if ( GameScreen *screen = dynamic_cast<GameScreen*>(state) )
        {
            screen->finalize();
        }
    }
    CloseAudioDevice();
}

void Game::propagateState()
{
    TRACE;
    GameState *newState = this->state;
    for ( Transition &transition : this->transitions )
    {
        if ( transition.source != this->state )
        {
            continue;
        }
        if ( transition.isApplicable(this->state) )
        {
            newState = transition.target;
            break;
        }
    }
    if ( state != newState )
    {
        state->exit();
        newState->enter();
        state = newState;
    }
    /// TODO not sure this is necessary
    if ( WindowShouldClose() )
    {
        this->isRunning = false;
    }
}

void Game::run()
{
    initialize();
    TRACE;
    while ( this->isRunning )
    {
        this->propagateState();
        this->state->update(GetFrameTime());
    }
    finalize();
}

void showGame()
{
    TRACE;

    Game game;

    game.run();
}