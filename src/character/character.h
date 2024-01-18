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
using CharacterState = enum { CHAR_IDLE = 0, CHAR_WALK, CHAR_DIE, CHAR_FIGHT, CHAR_SPECIAL_1, CHAR_SPECIAL_2};

class Animation
{
public:
    Animation(Animation *animation)
        : loop(animation->loop),
        triggers(animation->triggers),
        frames(animation->frames),
        currentFrame(0),
        loopCnt(0),
        frameDelta(0) {}
    Animation(AnimationFrames frames) : loop(-1), frames(frames), loopCnt(0), frameDelta(0), currentFrame(0) {}
    int loop;
    std::map<unsigned int, AnimationTrigger> triggers;
    AnimationFrames frames;
    void update(float delta);
    unsigned int currentFrame;
    void setTrigger(unsigned int frame, AnimationTrigger trigger);
private:
    int loopCnt;
    float frameDelta;
};

class Sprite
{
public:
    Sprite()
        : animation(nullptr),
          position({0,0}),
          origin({0,0}),
          size({0,0}),
          texture({0}) {}
    Sprite(Animation *animation,
            Vector2 position,
            Vector2 origin,
            Vector2 size,
            Texture2D texture)
    : animation(animation),
        position(position),
        origin(origin),
        size(size),
        texture(texture)
    {}
    Animation *animation;
    void draw(float delta);
    Vector2 position;
    Vector2 size;
private:
    Vector2 origin; ///< optional
    Texture texture;
};

class Character
{
public:
    Character(Vector2 position, Vector2 size, Sprite *sprite)
        : position(position),
            size(size),
            direction(DIR_E),
            sprite(sprite) {}
    Vector2 position;
    Vector2 size;
    Direction direction;
    CharacterState state;
    std::map<ActionType, ActionFunction> actions;
    Sprite *sprite;
};

#endif // CHARACTER_H