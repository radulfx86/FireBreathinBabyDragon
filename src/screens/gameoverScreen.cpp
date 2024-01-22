#include "gameoverScreen.h"
#include <sstream>

void GameoverScreen::initialize()
{
    TRACE;

}
void GameoverScreen::finalize()
{
    TRACE;

}
void GameoverScreen::enter()
{
    TRACE;
    SetWindowTitle("YOU DED");

}
void GameoverScreen::exit()
{
    TRACE;

}

void GameoverScreen::update(float delta)
{
    this->draw(delta);
}

void GameoverScreen::draw(float delta)
{
    TRACE;
    BeginDrawing();
        
        ClearBackground(BLACK);
        std::stringstream exitText;
        exitText << "GAMEOVER : " << static_cast<int>(exitTime);
        DrawText(exitText.str().c_str(), 100,100+(4.0-exitTime)*100,30,RED);

        exitTime -= delta;
        if ( exitTime < 0.0f )
        {
            this->game->stop();
        }

    EndDrawing();
}
