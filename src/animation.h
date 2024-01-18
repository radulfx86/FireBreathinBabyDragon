#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>
#include "raylib.h"
#include "character.h"
using Animation_t = Animation;
using Sprite_t = Sprite;
#if 0
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
#endif
extern Animation walkLeftNPC;
extern Animation walkRightNPC;
extern Animation idleDragon;

#endif // ANIMATION_H