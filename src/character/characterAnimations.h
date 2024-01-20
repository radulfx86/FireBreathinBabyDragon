#ifndef CHARACTER_ANIMATION_H
#define CHARACTER_ANIMATION_H
#include "character.h"

/// villagers.png animations
Animation charAnimationIdle= { -1, {},
        { { 1.0, {0.0,0.0,16.0,16.0} }
        }};
Animation charAnimationDie = { 1, {}, 
        { { 0.3, {160.0,0.0,16.0,16.0} },
        { 0.3, {172.0,0.0,16.0,16.0} },
        { 0.3, {184.0,0.0,16.0,16.0} }
        }};
Animation charAnimationSpecial = { -1, {}, 
        { { 0.3, {112.0,0.0,16.0,16.0} },
        { 0.3, {128.0,0.0,16.0,16.0} },
        { 0.3, {144.0,0.0,16.0,16.0} },
        { 0.3, {128.0,0.0,16.0,16.0} }
        }};
Animation charAnimationWalkN = { -1, {},
        { { 0.3, {48.0,0.0,16.0,16.0} },
        { 0.3, {64.0,0.0,16.0,16.0} }
        }};
Animation charAnimationWalkS = { -1, {},
        { { 0.3, {80.0,0.0,16.0,16.0} },
        { 0.3, {96.0,0.0,16.0,16.0} }
        }};
Animation charAnimationWalkW = { -1, {},
        { { 0.3, {16.0,0.0,-16.0,16.0} },
        { 0.3, {32.0,0.0,-16.0,16.0} }
        }};
Animation charAnimationWalkE = { -1, {}, 
        { { 0.3, {16.0,0.0,16.0,16.0} },
        { 0.3, {32.0,0.0,16.0,16.0} }
        }};
#endif // CHARACTER_ANIMATION_H