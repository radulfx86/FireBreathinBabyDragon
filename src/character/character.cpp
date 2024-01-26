#include "character.h"
#include "debugStats.h"

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
    TraceLog(LOG_DEBUG,"AnimatedSprite::draw(%f) - animation: %d/%d, frame %d/d, update %s, frameDelta: %f",
        delta,
        static_cast<int>(this->animationState.activeAnimation), this->animations.size(),
        this->animationState.activeFrame, this->animations[this->animationState.activeAnimation].frames.size(),
        updateAnimation ? "true" : "false",
        this->animationState.frameDelta);
    if ( updateAnimation )
    {
        float tmpDelta = delta + this->animationState.frameDelta;
        if ( tmpDelta > 1.0 )
        {
            tmpDelta = this->animationState.frameDelta;
        }

        /// with too many NPC the program gets stuck in this loop with tmpDelta rediculously high
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
                // check triggers
                if ( this->animations[this->animationState.activeAnimation].triggers.count(nextFrame) > 0 )
                {
                    this->animations[this->animationState.activeAnimation].triggers[nextFrame]();
                }
            }
            if ( tmpDelta > FRAMESKIP_MAX )
            {
                DebugStats::getInstance().addFrameSkip();
                tmpDelta = this->animationState.frameDelta;
                break;
            }
        }
        this->animationState.frameDelta = tmpDelta;
        Sprite::textureBounds = this->animations[this->animationState.activeAnimation].frames[this->animationState.activeFrame].second;
        Sprite::textureBounds.x += Sprite::textureOffset.x;
        Sprite::textureBounds.y += Sprite::textureOffset.y;
    }

    Sprite::draw(delta);
}
