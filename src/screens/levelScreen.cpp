#include "levelScreen.h"
#include <sstream>

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
