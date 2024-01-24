#ifndef UI_H
#define UI_H
#include "character.h"
#include "datastore.h"

class UI
{
public:
    UI(Vector2 pos, float scale, WorldObjectStatus *playerState):
        pos(pos),
        scale(scale),
        playerState(playerState)
    {
        this->texture = Datastore::getInstance().getTexture("images/ui.png");
        this->lifeSprite = new AnimatedSprite(
            // texture bounds
            {0.0,0.0,16.0,16.0},
            // screen bounds
            {0.0,0.0,16.0f * scale, 16.0f * scale},
            this->texture,
            {{CharacterState::CHAR_IDLE,
            (Animation){-1, {},
            {   {0.2, {0.0,0.0,16.0,16.0}},
                {0.2, {16.0,0.0,16.0,16.0}},
                {0.2, {32.0,0.0,16.0,16.0}},
                {0.2, {48.0,0.0,16.0,16.0}},
                {0.2, {64.0,0.0,16.0,16.0}},
                {0.2, {80.0,0.0,16.0,16.0}}
            }}}});
        this->fireSprite = new AnimatedSprite(
            // texture bounds
            {0.0,16.0,16.0,16.0},
            // screen bounds
            {200.0,0.0,16.0f * scale, 16.0f * scale},
            this->texture,
            {{CharacterState::CHAR_IDLE,
            (Animation){-1, {},
            {   {0.2, {0.0,0.0,16.0,16.0}},
                {0.2, {16.0,0.0,16.0,16.0}},
                {0.2, {32.0,0.0,16.0,16.0}},
                {0.2, {48.0,0.0,16.0,16.0}},
                {0.2, {64.0,0.0,16.0,16.0}},
                {0.2, {80.0,0.0,16.0,16.0}}
            }}}});
    }

    void draw(float delta);
private:
    Vector2 pos;
    float scale;
    Texture texture;
    WorldObjectStatus *playerState;
    AnimatedSprite *lifeSprite;
    AnimatedSprite *fireSprite;
};

#endif // UI_H