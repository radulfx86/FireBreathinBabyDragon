#include "raylib.h"
#include <vector>
#include <iostream>
#include "demos.h"


int main(void)
{
    InitWindow(280, 450, "select demo");
    DemoList demoList((Rectangle){0,0,(float)GetScreenWidth(), (float)GetScreenHeight()});
    demoList.addEntry("GAME", showGame);
    demoList.addEntry("Tiles", showTileDemo);
    demoList.addEntry("Sprite animation", showAnimationDemo);
    demoList.addEntry("Motion", showMotionDemo);
    demoList.addEntry("Life-Demo", showLifeDemo);
    DemoList::DemoFunPtr_t demoFunction = nullptr;
    while ( (!WindowShouldClose()) && (nullptr == demoFunction ))
    {
        if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT ) )
        {
            demoFunction = demoList.checkSelection();
        }
        demoList.draw();
    }
    if ( demoFunction )
    {
        demoFunction();
    }

    CloseWindow();

    return 0;
}