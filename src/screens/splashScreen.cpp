#include "splashScreen.h"
#include "datastore.h"

void SplashScreen::initialize()
{
    TRACE;
    this->dragonSprite = new Sprite(&idleDragon,
        {100.0,100.0},
        {13.0,0.0},
        {32.0,32.0},
        Datastore::getInstance().getTexture("images/dragon_0_20240112_01.png")
    );

    SetWindowSize(640, 480);
    SetTargetFPS(60);
    InitAudioDevice();
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
                    {this->dragonSprite->position.x * 2, this->dragonSprite->position.y * 2,
                    this->dragonSprite->size.x * 2, this->dragonSprite->size.y * 2}) )
        {
            /// TODO this is NOT the way to do such stuff !!!
            this->isDone = true;
        }
        ClearBackground(BLACK);
            DrawRectangleLinesEx({this->dragonSprite->position.x * 2, this->dragonSprite->position.y * 2,
                    this->dragonSprite->size.x * 2, this->dragonSprite->size.y * 2}, 1.0, RED);
            DrawText("click on dragon to start level", 100,100,30,ORANGE);

        dragonSprite->draw(delta);
    EndDrawing();

}

