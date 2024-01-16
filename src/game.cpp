#include "demos.h"
#include <iostream>
#include <sstream>
#include <map>
#include "animation.h"
#include "game.h"

#define TRACE \
do { \
    std::cerr << __func__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
} while(0)


void Game::initialize()
{
    TRACE;
    SplashScreen *splashScreen = new SplashScreen(this);
    LevelScreen *levelScreen = new LevelScreen(this);
    ExitScreen *exitScreen = new ExitScreen(3.0, this);
    this->states.push_back(splashScreen);
    this->states.push_back(levelScreen);
    this->states.push_back(exitScreen);
    this->transitions.push_back(Transition(splashScreen, levelScreen, SplashScreen::checkDone));
    this->transitions.push_back(Transition(levelScreen, exitScreen, levelScreen->checkDone));
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


void SplashScreen::initialize()
{
    TRACE;
    this->dragonSprite = {
    // animation
    {
        // frameTimes
        { 0.2, 0.2, 0.2, 0.2, 0.2 },
        // framePos
        {   {0.0,0.0,32.0,32.0},
            {32.0,0.0,32.0,32.0},
            {64.0,0.0,32.0,32.0},
            {96.0,0.0,32.0,32.0},
            {128.0,0.0,32.0,32.0}
        },
        0,
        0.0
    },
    {100.0,100.0},
    {13.0,0.0},
    {32.0,32.0},
    LoadTexture("images/dragon_0_20240112_01.png")
    };

    SetWindowSize(640, 480);
    SetTargetFPS(60);
    InitAudioDevice();
    this->fireBreath = LoadSound("audio/Firebreath_Level_1.mp3");

}
void SplashScreen::finalize()
{
    TRACE;
    UnloadTexture(this->dragonSprite.texture);
    UnloadSound(this->fireBreath);

}
void SplashScreen::enter()
{
    TRACE;
    SetWindowTitle("START THE GAME");
}
void SplashScreen::exit()
{
    TRACE;

}
void SplashScreen::update()
{
    this->draw();
}
void SplashScreen::draw()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
                && CheckCollisionPointRec(GetMousePosition(),
                    {this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}) )
        {
            /// TODO this is NOT the way to do such stuff !!!
            this->isDone = true;
            PlaySound(this->fireBreath);
        }
        ClearBackground(BLACK);
            DrawRectangleLinesEx({this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}, 1.0, RED);
            DrawText("click on dragon to start level", 100,100,30,ORANGE);

        drawChar(delta, &dragonSprite);
    EndDrawing();

}

void LevelScreen::initialize()
{
    TRACE;
    this->dragonSprite = {
    // animation
    {
        // frameTimes
        { 0.2, 0.2, 0.2, 0.2, 0.2 },
        // framePos
        {   {0.0,0.0,32.0,32.0},
            {32.0,0.0,32.0,32.0},
            {64.0,0.0,32.0,32.0},
            {96.0,0.0,32.0,32.0},
            {128.0,0.0,32.0,32.0}
        },
        0,
        0.0
    },
    {100.0,100.0},
    {13.0,0.0},
    {32.0,32.0},
    LoadTexture("images/dragon_0_20240112_01.png")
    };

    SetWindowSize(640, 480);
    SetTargetFPS(60);
    InitAudioDevice();
    this->fireBreath = LoadSound("audio/Firebreath_Level_1.mp3");

}
void LevelScreen::finalize()
{
    TRACE;
    UnloadTexture(this->dragonSprite.texture);
    UnloadSound(this->fireBreath);

}
void LevelScreen::enter()
{
    TRACE;
    SetWindowTitle("LEVEL 1");

}
void LevelScreen::exit()
{
    TRACE;
}

void LevelScreen::update()
{
    TRACE;
    this->draw();
    if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
            && CheckCollisionPointRec(GetMousePosition(),
                {this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}) )
    {
        this->isDone = true;
    }
}

void LevelScreen::draw()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        ClearBackground(GREEN);
            DrawRectangleLinesEx({this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}, 1.0, RED);
            DrawText("this is the LEVEL", 100,100,30,ORANGE);
            DrawText("click on the dragon to exit", 100,300,30,ORANGE);

        drawChar(delta, &dragonSprite);
    EndDrawing();

}

void ExitScreen::initialize()
{
    TRACE;

}
void ExitScreen::finalize()
{
    TRACE;

}
void ExitScreen::enter()
{
    TRACE;
    SetWindowTitle("See you Space Cowboy");

}
void ExitScreen::exit()
{
    TRACE;

}

void ExitScreen::update()
{
    this->draw();
}

void ExitScreen::draw()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        
        ClearBackground(DARKGRAY);
        std::stringstream exitText;
        exitText << "GOODBYE : " << static_cast<int>(exitTime);
        DrawText(exitText.str().c_str(), 100,100+(3.0-exitTime)*100,30,ORANGE);

        exitTime -= delta;
        if ( exitTime < 0.0f )
        {
            this->game->stop();
        }

    EndDrawing();
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
        this->state->update();
    }
    finalize();
}

void showGame()
{
    TRACE;

    Game game;

    game.run();
}