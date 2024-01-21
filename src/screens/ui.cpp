#include "ui.h"
#include <sstream>
void UI::draw(float delta)
{
    /// TODO make that a texture
    DrawRectangleRec({pos.x, pos.y, 400,40}, {55,55,30,125});
    this->lifeSprite->draw(delta);
    DrawRectangleRec((Rectangle){40,16,static_cast<float>(this->playerState->HP), 20}, {172, 50, 50, 255});
    DrawRectangleLinesEx((Rectangle){40,16,static_cast<float>(this->playerState->HP), 20}, 2, {75, 22, 22, 255});
    std::stringstream hpText;
    hpText << this->playerState->HP;
    DrawText(hpText.str().c_str(), 42, 2, 30, LIGHTGRAY);

    this->fireSprite->draw(delta);
    DrawRectangleRec((Rectangle){240,16,static_cast<float>(this->playerState->EP), 20}, {255, 134, 0, 255});
    DrawRectangleLinesEx((Rectangle){240,16,static_cast<float>(this->playerState->EP), 20}, 2, {186, 47, 0, 255});
    std::stringstream epText;
    epText << this->playerState->EP;
    DrawText(epText.str().c_str(), 242, 2, 30, LIGHTGRAY);
}