#include "character.h"

void Animation::setTrigger(unsigned int frame, AnimationTrigger trigger)
{
    this->triggers[frame] = trigger;
}

void Animation::update(float delta)
{
    if ( this->loop > 0);
    float tmpDelta = delta + this->frameDelta;

    while ( tmpDelta > this->frames[this->currentFrame].first )
    {
        tmpDelta -= this->frames[this->currentFrame].first;
        this->currentFrame = (this->currentFrame + 1) % this->frames.size();
    }
    this->frameDelta = tmpDelta;
}

void Sprite::draw(float delta)
{
    if ( this->animation )
    {
        this->animation->update(delta);
        Rectangle dest = {this->position.x*2, this->position.y*2,
                        this->size.x*2, this->size.y*2};
        DrawTexturePro(this->texture,
            this->animation->frames[this->animation->currentFrame].second,
            dest,
            {0.0,0.0},
            0.0,
            WHITE);
    }
}