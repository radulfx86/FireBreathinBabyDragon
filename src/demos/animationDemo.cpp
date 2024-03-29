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

Animation walkLeftNPC = { -1, {}, 
        { { 0.3, {16.0,0.0,16.0,16.0} },
        { 0.3, {32.0,0.0,16.0,16.0} }
        }};

Animation walkRightNPC = { -1, {},
        { { 0.3, {32.0,0.0,-16.0,16.0} },
        { 0.3, {48.0,0.0,-16.0,16.0} }
        }};

Animation idleDragon = { -1, {},
            {{ 0.2, {0.0,0.0,32.0,32.0}},
            { 0.2, {32.0,0.0,32.0,32.0}},
            { 0.2, {64.0,0.0,32.0,32.0}},
            { 0.2, {96.0,0.0,32.0,32.0}},
            { 0.2, {128.0,0.0,32.0,32.0}}
            }};


void showAnimationDemo()
{
    AnimatedSprite DragonSprite(
        {0.0,0.0,32.0,32.0},
        {100.0,100.0,32.0,32.0},
        LoadTexture("images/dragon_0_20240112_01.png"),
        {{CharacterState::CHAR_IDLE, idleDragon}});

    AnimatedSprite VillagerSprite(
        {0.0,16.0,16.0,16.0},
        {200.0,100.0,32.0,32.0},
        LoadTexture("images/villagers_20240112_01.png"),
        {{CharacterState::CHAR_WALK_E, walkLeftNPC}});
    VillagerSprite.animationState.activeAnimation = CharacterState::CHAR_WALK_E;

    AnimatedSprite MageSprite(
        {0.0,48.0,16.0,16.0},
        {250.0,100.0,24.0,24.0},
        LoadTexture("images/villagers_20240112_01.png"),
        {{CharacterState::CHAR_WALK_W, walkLeftNPC}});
    MageSprite.animationState.activeAnimation = CharacterState::CHAR_WALK_W;

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