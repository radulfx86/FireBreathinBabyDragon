#include "splashScreen.h"
#include "datastore.h"

void SplashScreen::initialize()
{
    TRACE;
    this->dragonSprite = new AnimatedSprite(
        {0.0,0.0,32.0,32.0},
        {100.0,100.0,128.0,128.0},
        Datastore::getInstance().getTexture("images/dragon_0_20240112_01.png"),
        {{CharacterState::CHAR_IDLE, idleDragon}}
    );

    SetWindowSize(640, 480);
    SetTargetFPS(60);
    this->mainTheme = Datastore::getInstance().getSound("audio/Main_Theme_Lute.mp3");

}
void SplashScreen::finalize()
{
    TRACE;
}
void SplashScreen::enter()
{
    TRACE;
    SetWindowTitle("START THE GAME");
}
void SplashScreen::exit()
{
    /// quick hack - asynchronous looping would be better
    if ( IsSoundPlaying(this->mainTheme))
    {
        StopSound(this->mainTheme);
    } 
    TRACE;

}
void SplashScreen::update(float delta)
{
    /// quick hack - asynchronous looping would be better
    if ( IsSoundReady(this->mainTheme) && (!IsSoundPlaying(this->mainTheme)))
    {
        PlaySound(this->mainTheme);
    }
    this->draw(delta);
}
void SplashScreen::draw(float delta)
{
    TRACE;
    BeginDrawing();
        if ( IsMouseButtonReleased(MOUSE_BUTTON_LEFT)
                && CheckCollisionPointRec(GetMousePosition(),
                    dragonSprite->screenBounds) )
        {
            /// TODO this is NOT the way to do such stuff !!!
            this->isDone = true;
        }
        ClearBackground(BLACK);
            DrawRectangleLinesEx(dragonSprite->screenBounds, 1.0, RED);
            DrawText("click on dragon to start level", 100,100,30,ORANGE);

        dragonSprite->draw(delta);
    EndDrawing();

}

