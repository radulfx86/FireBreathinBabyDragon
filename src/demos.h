#ifndef DEMOS_H
#define DEMOS_H

typedef enum
{
    NONE = 0,
    ANIMATION_DEMO = 1,
    MOTION_DEMO = 2,
    TILE_DEMO = 3,
    GAME = 4,
    QUIT
} Demo_t;

void showAnimationDemo();

void showMotionDemo();

void showTileDemo();

void showGame();

#endif // DEMOS_H