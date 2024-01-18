#ifndef CHARACTER_H
#define CHARACTER_H

#include "raylib.h"
#include <map>
#include <vector>


using Direction = enum { DIR_E = 0, DIR_N, DIR_W, DIR_S };
using ActionType = enum { ACTION_HIT = 0 };
class Character;
using ActionFunction = bool (*)(Character *);
class Animation;
using AnimationTrigger = void (*)(Animation *);
using TimedFrame = std::pair<float, Rectangle>;
using AnimationFrames = std::vector<TimedFrame>;
enum class CharacterState { CHAR_IDLE = 0, CHAR_WALK, CHAR_DIE, CHAR_FIGHT, CHAR_SPECIAL_1, CHAR_SPECIAL_2};

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
        std::map<CharacterState, Animation> animations)
        : Sprite(textureBounds, screenBounds, texture),
            animations(animations),
            animationState{(AnimationState){CharacterState::CHAR_IDLE,0,0,0}} {}

    virtual void draw(float delta) override;

    std::map<CharacterState, Animation> animations;
    AnimationState animationState;
};

class Character
{
public:
    Character(Rectangle worldBounds, Rectangle screenBounds, AnimatedSprite *sprite)
        : worldBounds(worldBounds),
        screenBounds(screenBounds),
            direction(DIR_E),
            sprite(sprite) {}
    Rectangle worldBounds;
    Rectangle screenBounds;
    Direction direction;
    CharacterState state;
    std::map<ActionType, ActionFunction> actions;
    AnimatedSprite *sprite;
};

#endif // CHARACTER_H