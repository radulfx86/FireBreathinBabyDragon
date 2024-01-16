#include "raylib.h"
#include "demos.h"
#include "animation.h"

Sprite_t FireBreather;

Sound fireBreath;

void loadAudioCharacters()
{
    FireBreather = {
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
    InitAudioDevice();
    fireBreath = LoadSound("audio/Firebreath_Level_1.mp3");
}

void showAudioDemo()
{

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Audio Demo");
    loadAudioCharacters();
    while (!WindowShouldClose())
    {
        /// TODO the "* 2 " for the position is not necessary, fix this in drawChar !!!
        if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && CheckCollisionPointRec(GetMousePosition(),
                    {FireBreather.position.x * 2, FireBreather.position.y * 2,
                    FireBreather.spriteSize.x * 2, FireBreather.spriteSize.y * 2})
            
                    )
        {
            PlaySound(fireBreath);
        }
        BeginDrawing();
            float delta = GetFrameTime();
            DrawRectangleLinesEx({FireBreather.position.x * 2, FireBreather.position.y * 2,
                    FireBreather.spriteSize.x * 2, FireBreather.spriteSize.y * 2}, 1.0, RED);
            DrawText("click on dragon to play sound", 100,100,30,ORANGE);
            ClearBackground(BLACK);
            drawChar(delta, &FireBreather);
        EndDrawing();
    }
    UnloadSound(fireBreath);
    CloseAudioDevice();
}