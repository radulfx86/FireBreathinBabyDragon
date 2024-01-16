#include "exitScreen.h"
#include <sstream>

void ExitScreen::initialize()
{
    TRACE;

}
void ExitScreen::finalize()
{
    TRACE;

}
void ExitScreen::enter()
{
    TRACE;
    SetWindowTitle("See you Space Cowboy");

}
void ExitScreen::exit()
{
    TRACE;

}

void ExitScreen::update()
{
    this->draw();
}

void ExitScreen::draw()
{
    TRACE;
    BeginDrawing();
        float delta = GetFrameTime();
        
        ClearBackground(DARKGRAY);
        std::stringstream exitText;
        exitText << "GOODBYE : " << static_cast<int>(exitTime);
        DrawText(exitText.str().c_str(), 100,100+(3.0-exitTime)*100,30,ORANGE);

        exitTime -= delta;
        if ( exitTime < 0.0f )
        {
            this->game->stop();
        }

    EndDrawing();
}
