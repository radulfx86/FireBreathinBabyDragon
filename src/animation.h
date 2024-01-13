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
    Texture2D texture;
} Sprite_t;

#endif // ANIMATION_H