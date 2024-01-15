#include "demos.h"
#include <iostream>
#include <sstream>
#include "animation.h"

#define TRACE ;
#if 0
\
do { \
    std::cerr << __func__ << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
} while(0)
#endif

class Game
{
public:
    using GameState = enum {NONE = 0,
                            INITIALIZED = 1,
                            SPLASH = 2,
                            LEVEL = 3,
                            MENU = 4,
                            GAMEOVER = 5,
                            EXIT = 6,
                            DONE = 7};
    Game() : state(GameState::NONE), exitTime(3.0f) {TRACE;}
    ~Game() = default;

    void run();

    void propagateState();

    void drawScreen();

private:
    GameState state;

    void initialize();

    void drawSplashScreen();

    void drawLevelScreen();

    void drawMenuScreen();

    void drawGameoverScreen();

    void drawExitScreen();

    Sprite_t dragonSprite;

    float exitTime;
};

void Game::initialize()
{
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
    SetTargetFPS(10);
}

void Game::drawSplashScreen()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
                && CheckCollisionPointRec(GetMousePosition(),
                    {this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}) )
        {
            this->state = GameState::LEVEL;
        }
        ClearBackground(BLACK);
            DrawRectangleLinesEx({this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}, 1.0, RED);
            DrawText("click on dragon to start level", 100,100,30,ORANGE);

        drawChar(delta, &dragonSprite);
    EndDrawing();

}

void Game::drawLevelScreen()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
                && CheckCollisionPointRec(GetMousePosition(),
                    {this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}) )
        {
            this->state = GameState::EXIT;
        }
        ClearBackground(GREEN);
            DrawRectangleLinesEx({this->dragonSprite.position.x * 2, this->dragonSprite.position.y * 2,
                    this->dragonSprite.spriteSize.x * 2, this->dragonSprite.spriteSize.y * 2}, 1.0, RED);
            DrawText("this is the LEVEL", 100,100,30,ORANGE);
            DrawText("click on the dragon to exit", 100,300,30,ORANGE);

        drawChar(delta, &dragonSprite);
    EndDrawing();

}

void Game::drawMenuScreen()
{
    TRACE;

}

void Game::drawGameoverScreen()
{
    TRACE;

}

void Game::drawExitScreen()
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
            this->state = GameState::DONE;
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
    switch( this->state )
    {
        case GameState::NONE:
            initialize();
            this->state = GameState::INITIALIZED;
            SetWindowTitle("START THE GAME");
            break;
        case GameState::INITIALIZED:
        // fall through
        case GameState::SPLASH:
            break;
        case GameState::LEVEL:
            SetWindowTitle("LEVEL 1");
            break;
        case GameState::MENU:
            break;
        case GameState::GAMEOVER:
            break;
        case GameState::EXIT:
            SetWindowTitle("See you Space Cowboy");
            break;
        case GameState::DONE:
            break;
        default:
            break;
    }
}

void Game::drawScreen()
{
    TRACE;
    switch( this->state )
    {
        case GameState::NONE:
        // fall through
        case GameState::INITIALIZED:
        // fall through
        case GameState::SPLASH:
            drawSplashScreen();
            break;
        case GameState::LEVEL:
            drawLevelScreen();
            break;
        case GameState::MENU:
            drawMenuScreen();
            break;
        case GameState::GAMEOVER:
            drawGameoverScreen();
            break;
        case GameState::EXIT:
            drawExitScreen();
            break;
        default:
            break;
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