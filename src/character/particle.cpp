#include "particle.h"
#include "levelScreen.h"

#include <sstream>



std::string Particle::str()
{
    std::stringstream sstr;
    sstr << "Particle("<< this->uid <<"): alive: " << std::boolalpha << this->alive
        << " lifetime: " << this->lifetime << " pos: " << this->pos.x << " " << this->pos.y
        << " speed: " << this->speed.x << " " << this->speed.y;

    return sstr.str();
}

bool Particle::update(float delta, Level *level)
{
    if ( this && this->alive )
    {
        this->lifetime -= delta;
        this->pos.x += this->speed.x * delta;
        this->pos.y += this->speed.y * delta;
        this->pos.width = 1.0;
        this->pos.height = 1.0;



        if ( this->alive && this->lifetime < delta )
        {
            if ( trigger )
            {
                this->trigger(this, level);
            }
            this->alive = false;
        }

        return isAlive();
    }
    else return false;
}

void Particle::draw(float delta, LevelScreen *screen)
{
    this->sprite->screenBounds = LevelScreen::WorldToScreen(screen, this->pos);
    this->sprite->draw(delta);
}

bool Particle::isAlive()
{
    return this->alive;
}
