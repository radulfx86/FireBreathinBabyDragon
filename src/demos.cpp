#include "raylib.h"
#include "demos.h"
#include <iostream>


void DemoList::addEntry(std::string text, DemoList::DemoFunPtr_t function)
{
    float buttonWidth = this->dimensions.width - 2 * DemoList::MARGIN_X;
    float offsetY = this->menuEntries.size() * (DemoList::BUTTON_HEIGHT + DemoList::MARGIN_Y);
    this->menuEntries.push_back( 
        (DemoList::DemoEntry_t) 
        {
            (Rectangle){   DemoList::MARGIN_X,
                DemoList::MARGIN_Y + offsetY,
                buttonWidth,
                DemoList::BUTTON_HEIGHT},
            text,
            function
        }
    );
    std::cout << "offsetY: " << offsetY << "\n";
    for ( DemoList::DemoEntry_t &entry : this->menuEntries )
    {
        std::cout << "entry (" << text << ") at ("
        << entry.dimensions.x << ", "
        << entry.dimensions.y << ", "
        << entry.dimensions.width << ", "
        << entry.dimensions.height << ")\n";
    }
}

DemoList::DemoFunPtr_t DemoList::checkSelection()
{
    DemoList::DemoFunPtr_t funPtr = nullptr;
    Vector2 mousePos = GetMousePosition();
    for ( DemoList::DemoEntry_t entry : this->menuEntries )
    {
        if ( CheckCollisionPointRec(mousePos, entry.dimensions) )
        {
            std::cout << "click on " << entry.text<<"\n";
            funPtr = entry.function;
            break;
        }
    }
    return funPtr;
}

void DemoList::draw()
{
    BeginDrawing();
        ClearBackground(BLACK);
        Vector2 mousePos = GetMousePosition();
        for ( DemoList::DemoEntry_t entry : this->menuEntries )
        {
            Color buttonColor = DARKGRAY;
            if ( CheckCollisionPointRec(mousePos, entry.dimensions) )
            {
                buttonColor = RED;
            }
            DrawRectangleRec(entry.dimensions, buttonColor);
            DrawText(entry.text.c_str(), entry.dimensions.x+MARGIN_X, entry.dimensions.y+DemoList::TEXT_MARGIN_Y, 30, LIGHTGRAY);
        }
    EndDrawing();
}