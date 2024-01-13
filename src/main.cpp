#include "raylib.h"
#include <vector>
#include <iostream>
#include "demos.h"

Rectangle AnimationDemoButton = {10,10,250,50};
Rectangle TileDemoButton = {10,70,250,50};
Rectangle MotionDemoButton = {10,130,250,50};

const int MARGIN_X = 5;
const int MARGIN_Y = 10;

Demo_t selectDemo()
{
    if ( WindowShouldClose() )
    {
        return QUIT;
    }

    Demo_t demoSelected = NONE;
    if ( !IsMouseButtonReleased(MOUSE_BUTTON_LEFT) )
    {
        return demoSelected;
    }

    Vector2 mousePos = GetMousePosition();
    if ( CheckCollisionPointRec(mousePos, AnimationDemoButton) )
    {
        demoSelected = ANIMATION_DEMO;
    }
    else if ( CheckCollisionPointRec(mousePos, TileDemoButton) )
    {
        demoSelected = TILE_DEMO;
    }
    else if ( CheckCollisionPointRec(mousePos, MotionDemoButton) )
    {
        demoSelected = MOTION_DEMO;
    }
    else
    {
        // nothing to do
    }
    return demoSelected;
}

void drawSelectionScreen()
{
    BeginDrawing();
    float delta = GetFrameTime();
    std::cout << "delta: " << delta << " s\n";
    std::cout << "FPS: " << 1.0 / delta << "\n";
    ClearBackground(BLACK);
    Vector2 mousePos = GetMousePosition();
    Color AnimationDemoColor = CheckCollisionPointRec(mousePos, AnimationDemoButton) ? RED : DARKGRAY;
    Color TileDemoColor = CheckCollisionPointRec(mousePos, TileDemoButton) ? RED : DARKGRAY;
    Color MotionDemoColor = CheckCollisionPointRec(mousePos, MotionDemoButton) ? RED : DARKGRAY;
    DrawRectangleRec(AnimationDemoButton, AnimationDemoColor);
    DrawText("Animation Demo", AnimationDemoButton.x+MARGIN_X, AnimationDemoButton.y+MARGIN_Y, 30, LIGHTGRAY);
    DrawRectangleRec(TileDemoButton, TileDemoColor);
    DrawText("Tile Demo", TileDemoButton.x+MARGIN_X, TileDemoButton.y+MARGIN_Y, 30, LIGHTGRAY);
    DrawRectangleRec(MotionDemoButton, MotionDemoColor);
    DrawText("Motion Demo", MotionDemoButton.x+MARGIN_X, MotionDemoButton.y+MARGIN_Y, 30, LIGHTGRAY);
    EndDrawing();
}

int main(void)
{
    InitWindow(270, 450, "select demo");
    Demo_t demoSelected = NONE;
    //SetTargetFPS(10);
    while (demoSelected == NONE)
    {
        demoSelected = selectDemo();
        drawSelectionScreen();
    }
    switch( demoSelected )
    {
        case ANIMATION_DEMO:
            showAnimationDemo();
            break;
        case MOTION_DEMO:
            showMotionDemo();
            break;
        case TILE_DEMO:
            showTileDemo();
            break;
        case NONE:
            std::cerr << "this is an error, NONE was selected" << std::endl;
            // fall through
        case QUIT:
            // fall through
        default:
            break;
    }

    CloseWindow();

    return 0;
}