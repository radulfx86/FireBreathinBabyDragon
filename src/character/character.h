#ifndef CHARACTER_H
#define CHARACTER_H

#include <raylib.h>
#include <map>
#include <vector>
#include <string>
#include <functional>

const int FRAMESKIP_MAX = 10.0;

/// object types
typedef enum {
    HOUSE = 0,
    TREE = 1,
    TOWER = 2,
    WALL = 3
} ObjectType;

/// character types
typedef enum {
    PLAYER = 0,
    VILLAGER = 1,
    GUARD = 2,
    MAGE = 3,
    HERO = 4
} CharacterType;

const std::string CharacterTypeStr[] {
    "PLAYER",
    "VILLAGER",
    "GUARD",
    "MAGE",
    "HERO"
};

typedef enum
{
    FIRE = 0,
    WATER = 1,
    WIND = 2,
    LIGHTNING = 3,
    STEEL = 4,
    NUM_DMG_TYPES
} DamageType;

typedef struct
{
    float value;
    DamageType type;
    float time;
} Damage;

typedef enum { FIRE_DISTANCE = 0,
            PLAYER_DISTANCE = 1,
            VILLAGE_DISTANCE = 2,
            MAGE_DISTANCE = 3 } DistanceMapType;
typedef std::vector<std::vector<int>> DistanceMap;
typedef std::map<int,DistanceMap> MappedDistanceMaps;
/**
 * @brief type for direction
 * @note unused, delete if not used later
 */
typedef enum { DIR_E = 0, DIR_N, DIR_W, DIR_S, DIR_NONE } Direction;
/**
 * @brief type for actions that can be performed on characters
 */
typedef enum { ACTION_HIT = 0 } ActionType;
/// forward declaration for ActionFunction
class Character;
/**
 * @brief type for function to be called to apply an action on the character
 * @param character pointer to character
 */
typedef bool (*ActionFunction)(Character *);
/// forward declaration for StateFunction
class Level;
/**
 * @brief type for function to be called while a character is in a specific state
 * @note TODO might replace the second parameter with a pointer to Level
 * @param character pointer to character
 * @param delta	time since last update [s]
 * @param level pointer to level
 */
typedef bool (*StateFunction)(Character *, float, Level *); 
/**
 * @brief type for function to be called along with a frame in an animation
 * @note to be used for e.g. splash-animations, ...
 */
typedef std::function<void(void)> AnimationTrigger;
/**
 * @brief type for an animation frame
 * @param delta time in [s] for which the frame is shown
 * @param bounds bounds of animation frame in pixels (texture-coordinates)
 */
typedef std::pair<float, Rectangle> TimedFrame;
/// list of timed frames
typedef std::vector<TimedFrame> AnimationFrames;
/// all possible character states
enum class CharacterState
{
    CHAR_IDLE = 0,
    CHAR_WALK_N = 1,
    CHAR_WALK_E = 2,
    CHAR_WALK_S = 3,
    CHAR_WALK_W = 4,
    CHAR_DIE = 5,
    CHAR_ATTACK_N = 6,
    CHAR_ATTACK_E = 7,
    CHAR_ATTACK_S = 8,
    CHAR_ATTACK_W = 9,
    CHAR_SPECIAL_1 = 10,
    CHAR_SPECIAL_2 = 11,
    CHAR_CATCH_FIRE = 12,
    CHAR_BURNING = 13};

/// character status values
typedef struct WorldObjectStatus
{
    /// hitpoints
    int HP;
    /// energy points
    int EP;
    /// attack points
    int AP;
    /// dmg applicator
    float dmgApplicator[DamageType::NUM_DMG_TYPES];
} WorldObjectStatus;

/// Animation structure
typedef struct Animation
{
    /// number of loops for this animation. -1 will loop forever
    int loop;
    /// triggers, key refers to the frame in the animation frames
    std::map<unsigned int, AnimationTrigger> triggers;
    /// animation frames
    AnimationFrames frames;
} Animation;

/// Animation status structure
typedef struct AnimationState
{
    /// character state this animation refers to
    CharacterState activeAnimation;
    /// currently active frame in animation
    unsigned int activeFrame;
    /// current loop of animation
    unsigned int currentLoop;
    /// delta/offset [s] in current frame
    float frameDelta;
} AnimationState;

/// sprite class
class Sprite
{
public:
    Sprite():
          textureBounds({0,0,0,0}),
          screenBounds({0,0,0,0}),
          textureOffset({0,0}),
          texture({0}) {}
    Sprite(Rectangle textureBounds,
            Rectangle screenBounds,
            Texture2D texture)
    : textureBounds(textureBounds),
        screenBounds(screenBounds),
        textureOffset({textureBounds.x, textureBounds.y}),
        texture(texture)
    {}
    /** @brief draw sprite
     * @param delta delta [s] */
    virtual void draw(float delta);
    /// texture bounds of sprite
    Rectangle textureBounds;
    /// screen bounds of sprite
    Rectangle screenBounds;
    /// offset of sprite in sprite sheet in texture coordinates
    Vector2 textureOffset;
private:
    /// sprite sheet texture
    Texture texture;
};

/// Animated sprite
class AnimatedSprite : public Sprite
{
public:
    AnimatedSprite(Rectangle textureBounds,
        Rectangle screenBounds,
        Texture2D texture,
        std::map<CharacterState, Animation> animations,
        AnimationState animationState)
        : Sprite(textureBounds, screenBounds, texture),
            animations(animations),
            animationState(animationState) {}
    AnimatedSprite(Rectangle textureBounds,
        Rectangle screenBounds,
        Texture2D texture,
        std::map<CharacterState, Animation> animations)
        : Sprite(textureBounds, screenBounds, texture),
            animations(animations),
            animationState{(AnimationState){CharacterState::CHAR_IDLE,0,0,0}} {}

    virtual void draw(float delta) override;
    /// animations for character states
    std::map<CharacterState, Animation> animations;
    /// animation state TODO: make private
    AnimationState animationState;
};

/// NOTE: rename to something more generic - as it can be used for both characters and objects
class Character
{
public:
    Character(std::string name, CharacterState state, WorldObjectStatus stats, Rectangle worldBounds, Rectangle screenBounds, AnimatedSprite *sprite)
        : name(name),
        state(state),
        stats(stats),
        worldBounds(worldBounds),
        screenBounds(screenBounds),
            sprite(sprite) {}
    /// name of the character - mainly for debugging
    std::string name;
    /// bounds in world coordinates
    Rectangle worldBounds;
    /// bounds in screen coordinates
    Rectangle screenBounds;
    /// current state of character
    CharacterState state;
    /// TODO check if needed
    //std::map<ActionType, ActionFunction> actions;
    /// strategy functions to be called for each character state
    std::map<CharacterState, StateFunction> strategy;
    /// pointer to animated sprite
    AnimatedSprite *sprite;
    /// character stats
    WorldObjectStatus stats;
};

#endif // CHARACTER_H
