#include "character.h"

void Sprite::draw(float delta)
{
    DrawTexturePro(this->texture,
        this->textureBounds,
        this->screenBounds,
        {0.0,0.0},
        0.0,
        WHITE);
}

void AnimatedSprite::draw(float delta)
{
    bool updateAnimation = (this->animations.size() > 0)
            && (this->animations.count(this->animationState.activeAnimation) > 0)
            && (this->animations[this->animationState.activeAnimation].frames.size() > 0)
            && (this->animations[this->animationState.activeAnimation].loop == -1
                 || this->animations[this->animationState.activeAnimation].loop > this->animationState.currentLoop);
    // update animation
    if ( updateAnimation )
    {
        float tmpDelta = delta + this->animationState.frameDelta;

        while ( tmpDelta > this->animations[this->animationState.activeAnimation].frames[this->animationState.activeFrame].first )
        {
            tmpDelta -= this->animations[this->animationState.activeAnimation].frames[this->animationState.activeFrame].first;
            unsigned int nextFrame = (this->animationState.activeFrame + 1) % this->animations[this->animationState.activeAnimation].frames.size();
            if ( nextFrame < this->animationState.activeFrame )
            {
                /// TODO only increments loop by 1, does not account for slow animations
                this->animationState.currentLoop += 1;
            }
            if ( nextFrame != this->animationState.activeFrame )
            {
                this->animationState.activeFrame = nextFrame;
            }
        }
        this->animationState.frameDelta = tmpDelta;
        Sprite::textureBounds = this->animations[this->animationState.activeAnimation].frames[this->animationState.activeFrame].second;
        Sprite::textureBounds.x += Sprite::textureOffset.x;
        Sprite::textureBounds.y += Sprite::textureOffset.y;
    }

    Sprite::draw(delta);
}
