#include "raylib.h"
#include "demos.h"
#include "animation.h"

Sound fireBreath;

void loadAudioCharacters(Sprite *sprite)
{
    sprite = new Sprite(
        new Animation({
            {0.2, {0.0,0.0,32.0,32.0}},
            {0.2, {32.0,0.0,32.0,32.0}},
            {0.2, {64.0,0.0,32.0,32.0}},
            {0.2, {96.0,0.0,32.0,32.0}},
            {0.2, {128.0,0.0,32.0,32.0}}}),
        {100.0,100.0},
        {13.0,0.0},
        {32.0,32.0},
        LoadTexture("images/dragon_0_20240112_01.png")
    );
    InitAudioDevice();
    fireBreath = LoadSound("audio/Firebreath_Level_1.mp3");
}

void showAudioDemo()
{

    const int screenWidth = 800;
    const int screenHeight = 600;
    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Audio Demo");
    Sprite *sprite;
    loadAudioCharacters(sprite);
    while (!WindowShouldClose())
    {
        /// TODO the "* 2 " for the position is not necessary, fix this in drawChar !!!
        if ( IsMouseButtonDown(MOUSE_BUTTON_LEFT)
            && CheckCollisionPointRec(GetMousePosition(),
                    {sprite->position.x * 2, sprite->position.y * 2,
                    sprite->size.x * 2, sprite->size.y * 2})
            
                    )
        {
            PlaySound(fireBreath);
        }
        BeginDrawing();
            float delta = GetFrameTime();
            DrawRectangleLinesEx({sprite->position.x * 2, sprite->position.y * 2,
                    sprite->size.x * 2, sprite->size.y * 2}, 1.0, RED);
            DrawText("click on dragon to play sound", 100,100,30,ORANGE);
            ClearBackground(BLACK);
            sprite->draw(delta);
        EndDrawing();
    }
    UnloadSound(fireBreath);
    CloseAudioDevice();
    if ( sprite )
    {
        delete sprite;
    }
}