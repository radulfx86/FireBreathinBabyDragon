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
    this->screens[GameState::SPLASH] = new SplashScreen(this);
    this->screens[GameState::LEVEL] = new LevelScreen(this);
    this->screens[GameState::EXIT] = new ExitScreen(3.0, this);
    for ( auto screen : this->screens )
    {
        screen.second->initialize();
    }

}

void Game::finalize()
{
    TRACE;
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
            this->game->state = GameState::LEVEL;
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
void LevelScreen::draw()
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
            this->game->state = GameState::EXIT;
        }
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
            this->game->state = GameState::DONE;
        }

    EndDrawing();
}


void Game::propagateState()
{
    TRACE;
    if ( WindowShouldClose() )
    {
        this->state = DONE;
        return;
    }
    GameState oldState = this->oldState;
    GameState newState = this->state;
    if ( oldState != newState )
    {
        if ( this->screens.count(oldState) )
        {
            this->screens[oldState]->exit();
        }
        if ( this->screens.count(newState) )
        {
            this->screens[newState]->enter();
        }
    }
    if ( this->state == GameState::NONE )
    {
        initialize();
        this->state = GameState::INITIALIZED;
    }
    if ( this->state == GameState::INITIALIZED )
    {
        this->state = GameState::SPLASH;
    }
    if ( this->state == GameState::DONE )
    {
        finalize();
    }
    this->oldState = this->state;
}

void Game::drawScreen()
{
    TRACE;
    if ( this->screens.count(this->oldState) )
    {
        this->screens[oldState]->draw();
    }
}

void Game::run()
{
    TRACE;
    while ( this->state < GameState::DONE )
    {
        propagateState();
        drawScreen();
    }
}

void showGame()
{
    TRACE;

    Game game;

    game.run();
}