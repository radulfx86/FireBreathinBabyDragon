#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib.h"
#include "character.h"
#include <iostream>

class UIDGenerator
{
public:
    static UIDGenerator &getInstance()
    {
        static UIDGenerator singleton;
        return singleton;
    }
    int requestUID()
    {
        return ++next;
    }
private:
    UIDGenerator() : next(0) {}
    int next;
};

class Particle;
class LevelScreen;
using ParticleTriggerFunction = std::function<bool(Particle *, LevelScreen *)>;
class Particle
{
public:
    Particle(AnimatedSprite *sprite, Rectangle pos, Vector2 speed, float lifetime, float dmg, ParticleTriggerFunction trigger)
        :  sprite(sprite),
            pos(pos),
            speed(speed),
            lifetime(lifetime),
            dmg(dmg),
            trigger(trigger),
            alive(true),
            uid(UIDGenerator::getInstance().requestUID())
        {
            
        std::cerr << __func__ << " of " << this->str() << "\n";
}

    bool update(float delta, LevelScreen *screen);
    void draw(float delta);
    bool isAlive();
    Rectangle pos;
    std::string str();
    float dmg;
private:
    int uid;
    bool alive;
    ParticleTriggerFunction trigger;
    AnimatedSprite *sprite;
    Vector2 speed;
    float lifetime;
};

#endif // PARTICLE_H