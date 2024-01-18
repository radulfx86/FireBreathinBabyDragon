#include "demos.h"
#include "raylib.h"
#include <vector>
#include <iostream>
#include "animation.h"

#define NULLCHECK(PTR) \
do { \
    if ( PTR == 0 ) { \
        return; \
    }\
} while(0)

Animation walkLeftNPC(
        { { 0.3, {16.0,0.0,16.0,16.0} },
        { 0.3, {32.0,0.0,16.0,16.0} }
        });

Animation walkRightNPC(
        { { 0.3, {32.0,0.0,-16.0,16.0} },
        { 0.3, {48.0,0.0,-16.0,16.0} }
        });

Animation idleDragon({
            { 0.2, {0.0,0.0,32.0,32.0}},
            { 0.2, {32.0,0.0,32.0,32.0}},
            { 0.2, {64.0,0.0,32.0,32.0}},
            { 0.2, {96.0,0.0,32.0,32.0}},
            { 0.2, {128.0,0.0,32.0,32.0}}
            });


void showAnimationDemo()
{
    Sprite DragonSprite(&idleDragon,
        {100.0,100.0},
        {13.0,0.0},
        {32.0,32.0},
        LoadTexture("images/dragon_0_20240112_01.png"));

    Sprite VillagerSprite(
        &walkLeftNPC,
        {200.0,100.0},
        {3.0,16.0},
        {32.0,32.0},
        LoadTexture("images/villagers_20240112_01.png"));

    Sprite MageSprite(
        &walkRightNPC,
        {250.0,100.0},
        {0.0,48.0},
        {24.0,24.0},
        LoadTexture("images/villagers_20240112_01.png"));
    const int screenWidth = 800;
    const int screenHeight = 600;
    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Animation Demo");
    //SetTargetFPS(10);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            float delta = GetFrameTime();
            std::cout << "delta: " << delta << " s\n";
            std::cout << "FPS: " << 1.0 / delta << "\n";
            ClearBackground(BLACK);

            VillagerSprite.draw(delta);
            MageSprite.draw(delta);
            DragonSprite.draw(delta);
        EndDrawing();
    }
}