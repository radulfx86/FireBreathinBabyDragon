#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>
#include "raylib.h"

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
    Vector2 textureOrigin;
    Vector2 spriteSize;
    Texture2D texture;
} Sprite_t;

void updateAnimation(float delta, Animation_t *animation);

void drawChar(float delta, Sprite_t *character);
#endif // ANIMATION_H