#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib.h"
#include "character.h"

class Particle;
class LevelScreen;
using ParticleTriggerFunction = bool (*)(Particle *);
class Particle
{
public:
    Particle(AnimatedSprite *sprite, Rectangle pos, Vector2 speed, float lifetime, ParticleTriggerFunction trigger)
        : sprite(sprite),
            pos(pos),
            speed(speed),
            lifetime(lifetime),
            trigger(trigger),
            alive(true)
        {}

    bool update(float delta, LevelScreen *screen);
    void draw(float delta);
    bool isAlive();
private:
    void executeTrigger();
    bool alive;
    ParticleTriggerFunction trigger;
    AnimatedSprite *sprite;
    Rectangle pos;
    Vector2 speed;
    float lifetime;
};

#endif // PARTICLE_H