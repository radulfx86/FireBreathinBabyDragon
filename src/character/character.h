#ifndef CHARACTER_H
#define CHARACTER_H

#include "raylib.h"
#include <map>
#include <vector>
#include <string>


using Direction = enum { DIR_E = 0, DIR_N, DIR_W, DIR_S };
using ActionType = enum { ACTION_HIT = 0 };
class Character;
using ActionFunction = bool (*)(Character *);
using StateFunction = bool (*)(Character *, std::vector<std::vector<int>>); 
class Animation;
using AnimationTrigger = void (*)(Animation *);
using TimedFrame = std::pair<float, Rectangle>;
using AnimationFrames = std::vector<TimedFrame>;
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
    CHAR_SPECIAL_2 = 11};

typedef struct Animation
{
    int loop;
    std::map<unsigned int, AnimationTrigger> triggers;
    AnimationFrames frames;
} Animation;


typedef struct AnimationState
{
    CharacterState activeAnimation;
    unsigned int activeFrame;
    unsigned int currentLoop;
    float frameDelta;
} AnimationState;

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
    virtual void draw(float delta);
    Rectangle textureBounds;
    Rectangle screenBounds;
protected:
    Vector2 textureOffset;
private:
    Texture texture;
};

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

    std::map<CharacterState, Animation> animations;
    AnimationState animationState;
};

/// NOTE: rename to something more generic - as it can be used for both characters and objects
class Character
{
public:
    Character(std::string name, CharacterState state, Rectangle worldBounds, Rectangle screenBounds, AnimatedSprite *sprite)
        : name(name),
        state(state),
        worldBounds(worldBounds),
        screenBounds(screenBounds),
            sprite(sprite) {}
    std::string name;
    Rectangle worldBounds;
    Rectangle screenBounds;
    CharacterState state;
    std::map<ActionType, ActionFunction> actions;
    std::map<CharacterState, StateFunction> strategy;
    AnimatedSprite *sprite;
};


#endif // CHARACTER_H