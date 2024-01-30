#include "particle.h"
#include "levelScreen.h"


bool Particle::update(float delta, LevelScreen *screen)
{
    this->lifetime -= delta;
    this->pos.x += this->speed.x * delta;
    this->pos.y += this->speed.y * delta;
    this->pos.width = 1.0;
    this->pos.height = 1.0;

    this->sprite->screenBounds = LevelScreen::WorldToScreen(screen, this->pos);


    if ( this->alive && this->lifetime < delta )
    {
        this->alive = false;
        this->executeTrigger();
    }

    return isAlive();
}

void Particle::draw(float delta)
{
    this->sprite->draw(delta);
}

bool Particle::isAlive()
{
    return this->alive;
}

void Particle::executeTrigger()
{
    if( trigger )
    {
        trigger(this);
    }
}