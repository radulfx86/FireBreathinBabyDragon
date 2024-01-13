#include "raylib.h"
#include <vector>
#include <iostream>

#define NULLCHECK(PTR) \
do { \
    if ( PTR == 0 ) { \
        return; \
    }\
} while(0)

typedef struct
{
    std::vector<float> frameTimes;
    std::vector<Rectangle> frames;
    unsigned int currentFrame;
    float delta;
} Animation_t;

typedef struct
{
    Animation_t animation;
    Vector2 position;
    Texture2D texture;
} Sprite_t;


Sprite_t DragonSprite;
Sprite_t VillagerSprite;
Sprite_t MageSprite;

Animation_t walk_left = {
        // frameTimes
        { 0.3, 0.3 },
        // framePos
        {
            {16.0,0.0,16.0,16.0},
            {32.0,0.0,16.0,16.0}
        },
        0,
        0.0
    };

Animation_t walk_right = {
        // frameTimes
        { 0.3, 0.3 },
        // framePos
        {
            {32.0,0.0,-16.0,16.0},
            {48.0,0.0,-16.0,16.0}
        },
        0,
        0.0
    };

void loadDefaultCharacters()
{
    DragonSprite = {
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
    LoadTexture("images/dragon_0_20240112_01.png")
    };
    VillagerSprite = {
    walk_right,
    {200.0,100.0},
    LoadTexture("images/villagers_20240112_01.png")
    };
    MageSprite = {
    walk_left,
    {200.0,100.0},
    LoadTexture("images/villagers_20240112_01.png")
    };
}

void updateAnimation(float delta, Animation_t *animation)
{
    NULLCHECK(animation);
    float tmpDelta = delta + animation->delta;
    while ( tmpDelta > animation->frameTimes[animation->currentFrame] )
    {
        tmpDelta -= animation->frameTimes[animation->currentFrame];
        animation->currentFrame = (animation->currentFrame + 1) % animation->frameTimes.size();
    }
    animation->delta = tmpDelta;
    //std::cout << "delta = " << delta << " updated animation of " << animation->frames.size() << " to frame " << animation->currentFrame << " with internal delta at " << animation->delta << "\n";
}

void drawChar(float delta, Sprite_t *character)
{
    NULLCHECK(character);
    updateAnimation(delta, &character->animation);
    DrawTextureRec(character->texture, character->animation.frames[character->animation.currentFrame], character->position, WHITE);
}

int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");
    loadDefaultCharacters();
    //SetTargetFPS(10);
    while (!WindowShouldClose())
    {
        BeginDrawing();
            float delta = GetFrameTime();
            std::cout << "delta: " << delta << " s\n";
            std::cout << "FPS: " << 1.0 / delta << "\n";
            ClearBackground(BLACK);
            DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

            for(int i = 0; i < 10000; ++i )
            {
                VillagerSprite.position.x = (i/(800/16))*16;
                VillagerSprite.position.y = (i%(450/16))*16;
                drawChar(delta, &VillagerSprite);
                MageSprite.position.x = 8+(i/(800/16))*16;
                MageSprite.position.y = 8+(i%(450/16))*16;
                drawChar(delta, &MageSprite);
            }
            drawChar(delta, &DragonSprite);
//        DrawTextureRec(DragonSprite.texture, DragonSprite.animation.frames[DragonSprite.animation.currentFrame], DragonSprite.position, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}