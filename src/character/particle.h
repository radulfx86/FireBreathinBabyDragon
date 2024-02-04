#ifndef PARTICLE_H
#define PARTICLE_H
#include "raylib.h"
#include "character.h"
#include <iostream>

/// singleton UUID generator @todo track when uids are returned?
class UIDGenerator
{
public:
    /// singleton
    static UIDGenerator &getInstance()
    {
        static UIDGenerator singleton;
        return singleton;
    }
    /// @brief get the next available UID
    /// @return next available UID 
    int requestUID()
    {
        return ++next;
    }
private:
    /// internal constructor
    UIDGenerator() : next(0) {}
    /// next UID
    int next;
};

/// forward declaration for trigger
class Particle;
/// forward declaration for trigger
class Level;
/// forward declaration for update function
class LevelScreen;
/// trigger function for particle after 
using ParticleTriggerFunction = std::function<bool(Particle *, Level *)>;

/// Particle @todo make more lightweight?
class Particle
{
public:
    /**
     * @brief Construct a new Particle object
     * 
     * @param sprite    sprite for graphical represenatation
     * @param pos       initial position @todo change to Vector2
     * @param speed     directed speed (2D) 
     * @param lifetime  lifetime of particle [s] 
     * @param dmg       dmg contained (for projectiles) 
     * @param trigger   function to be triggered after lifetime 
     */
    Particle(AnimatedSprite *sprite,
            Rectangle pos,
            Vector2 speed,
            float lifetime,
            float dmg,
            ParticleTriggerFunction trigger)
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
    /**
     * @brief update particle position and alive-state
     * @note call trigger-function if not alive after update
     * 
     * @param delta     delta time [s] for update
     * @param screen    pointer to level for trigger-function 
     * @return true     particle is alive 
     * @return false    particle is dead 
     */
    bool update(float delta, Level *screen);
    /**
     * @brief draw particle
     * 
     * @param delta     delta time [s]
     * @param screen    screen for coordinate transformation
     */
    void draw(float delta, LevelScreen *screen);
    /**
     * @brief check if particle is alive
     * 
     * @return true     particle is alive 
     * @return false    particle is dead 
     */
    bool isAlive();
    /**
     * @brief   compose string representation of particle
     * 
     * @return std::string  string representation of particle
     */
    std::string str();
    /// current particle position
    Rectangle pos;
    /// particle damage (if projectile)
    float dmg;
private:
    /// unique ID for tracking
    const int uid;
    /// internal alive-status tracker
    bool alive;
    /// trigger function. to be called at first update that exceeds lifetime
    ParticleTriggerFunction trigger;
    /// graphical representation of particle
    AnimatedSprite * const sprite;
    /// speed vector representing 2D distance in [s]
    const Vector2 speed;
    /// remaining lifetime of particle [s]
    float lifetime;
};

#endif // PARTICLE_H