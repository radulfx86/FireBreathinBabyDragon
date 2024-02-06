#include "level.h"
#include "datastore.h"
#include "characterAnimations.h"
#include "villagerStrategy.h"
#include "objectStrategy.h"
#include "debugStats.h"
#include "levelScreen.h"
#include <sstream>
#include <cmath>
#include <execution>
#include <queue>
#include <cstring>

void playFireBreath()
{
    auto sound = Datastore::getInstance().getSound("audio/Firebreath_Level_1.mp3");
    if ( IsSoundReady(sound) && not IsSoundPlaying(sound) )
    {
        PlaySound(sound);
    }
}

/**
 * @brief attack pattern for guard. e.g. for east:
 *       X X
 *  X -> X X X
 *       X X
 */
AttackPatternMap playerAttackPattern = {
{ Direction::DIR_E, {{1, 0}, {2, 0}, {3, 0}, {1, 1}, {1, -1}, {2, 1}, {2, -1}}},
{ Direction::DIR_W, {{-1, 0}, {-2, 0}, {-3, 0}, {-1, 1}, {-1, -1}, {-2, 1}, {-2, -1}}},
{ Direction::DIR_N, {{0, -1}, {0, -2}, {0, -3}, {1, -1}, {-1, -1}, {-1, -2}, {-1, -2}}},
{ Direction::DIR_S, {{0, 1}, {0, 2}, {0, 3}, {1, 1}, {-1, 1}, {1, 2}, {-1, 2}}},
};

/**
 * @brief attack pattern for guard. e.g. for east:
 *       X
 *  X -> X X
 *       X
 */
AttackPatternMap guardAttackPattern = {
{ Direction::DIR_E, {{1, 0}, {2, 0}, {1, 1}, {1, -1}}},
{ Direction::DIR_W, {{-1, 0}, {-2, 0}, {-1, 1}, {-1, -1}}},
{ Direction::DIR_N, {{0, -1}, {0, -2}, {1, -1}, {-1, -1}}},
{ Direction::DIR_S, {{0, 1}, {0, 2}, {1, 1}, {-1, 1}}},
};

/**
 * @brief "attack" pattern of villagers:
 *       X
 *  X -> X
 *       X
 */
AttackPatternMap villagerAttackPattern = {
{ Direction::DIR_E, {{1, 0}, {1, 1}, {1, -1}}},
{ Direction::DIR_W, {{-1, 0}, {-1, 1}, {-1, -1}}},
{ Direction::DIR_N, {{0, -1}, {1, -1}, {-1, -1}}},
{ Direction::DIR_S, {{0, 1}, {1, 1}, {-1, 1}}},
};

/**
 * @brief "explosion" pattern
 *     x
 *   x x x
 * x x i x x
 *   x x x
 *     x
 */
std::vector<GridPos> explosionPattern = {
    {1,0}, {0,1}, {-1,0}, {0, -1},
    {1,1}, {-1,-1}, {-1,1}, {1, -1},
    {2,0}, {0,2}, {-2,0}, {0, -2}
};

void Level::loadSounds()
{
    this->fireBreath = Datastore::getInstance().getSound("audio/Firebreath_Level_1.mp3");

}


void Level::addCharacter(CharacterType charType, int x, int y)
{
    std::cerr << __func__ << "(" << (int)charType << ", " << x << ", " << y << ")\n";
    if ( charType == CharacterType::PLAYER )
    {
        this->charSpeedMax = 10.0;
        this->charSpeed = {0.0f, 0.0f};
        this->charAcc = 50.0;

        Rectangle playerWorldBounds{x,y,2,2};
        Rectangle playerScreenBounds = LevelScreen::WorldToScreen(screen, playerWorldBounds);
        playerScreenBounds.width = 32;
        playerScreenBounds.height = 32;
        WorldObjectStatus initialPlayerStats = {10,1000,2};
        std::map<CharacterState, Animation> playerAnimations = {{CharacterState::CHAR_IDLE, playerAnimationIdle},
                {CharacterState::CHAR_WALK_E, playerAnimationWalkE},
                {CharacterState::CHAR_WALK_W, playerAnimationWalkW},
                {CharacterState::CHAR_WALK_S, playerAnimationWalkS},
                {CharacterState::CHAR_WALK_N, playerAnimationWalkN},
                {CharacterState::CHAR_ATTACK_E, playerAnimationAttackE},
                {CharacterState::CHAR_ATTACK_W, playerAnimationAttackW},
                {CharacterState::CHAR_ATTACK_S, playerAnimationAttackS},
                {CharacterState::CHAR_ATTACK_N, playerAnimationAttackN},
                {CharacterState::CHAR_DIE, playerAnimationDie}};
        playerAnimations[CharacterState::CHAR_ATTACK_E].triggers[0] = playFireBreath;
        playerAnimations[CharacterState::CHAR_ATTACK_W].triggers[0] = playFireBreath;
        playerAnimations[CharacterState::CHAR_ATTACK_N].triggers[0] = playFireBreath;
        playerAnimations[CharacterState::CHAR_ATTACK_S].triggers[0] = playFireBreath;
        /// NOTE: how is this possible ... 
        //auto setGameover = [&](LevelScreen *s) { s->forceGameover(); };
        //playerAnimations[CharacterState::CHAR_DIE].triggers[playerAnimationDie.frames.size()-1] = [&](){ setGameover(this); };
        this->player = new Character("player", CharacterState::CHAR_IDLE,
            initialPlayerStats,
            playerWorldBounds,
            playerScreenBounds,
            new AnimatedSprite(
                {0.0,0.0,32.0,32.0},
                playerScreenBounds,
                Datastore::getInstance().getTexture("images/dragon_1.png"),
                playerAnimations
        ));
        this->drawableObjects.push_back(this->player);
        std::cerr << " add player at " << player->worldBounds.x << " " << player->worldBounds.y << "\n";
    }
    else
    {
        /// NPCs
        /// class-specific parameters
        TraceLog(LOG_INFO, "load NPCs");
        Rectangle npcWorldBounds;
        npcWorldBounds.width = 1;
        npcWorldBounds.height = 1;
        WorldObjectStatus initialNPCStats = {10,10,-5};
        Rectangle npcScreenBounds{0,0,16,16};
        Rectangle npcTextureBounds{0,0,16,16};
        std::map<CharacterState, Animation> npcAnimations = {
            {CharacterState::CHAR_IDLE, charAnimationIdle},
            {CharacterState::CHAR_DIE, charAnimationDie},
            {CharacterState::CHAR_WALK_N, charAnimationWalkN},
            {CharacterState::CHAR_WALK_E, charAnimationWalkE},
            {CharacterState::CHAR_WALK_S, charAnimationWalkS},
            {CharacterState::CHAR_WALK_W, charAnimationWalkW}
            };
        CharacterState stateMap[] = {CharacterState::CHAR_IDLE,
            CharacterState::CHAR_DIE,
            CharacterState::CHAR_WALK_N,
            CharacterState::CHAR_WALK_E,
            CharacterState::CHAR_WALK_S,
            CharacterState::CHAR_WALK_W};
        npcWorldBounds.x = x;
        npcWorldBounds.y = y;
        npcScreenBounds = LevelScreen::WorldToScreen(screen, npcWorldBounds);
        npcTextureBounds.y = GetRandomValue(1,3) * 16;
        CharacterState npcState = CharacterState::CHAR_IDLE;
        Character *npc = 
            new Character("NPC", npcState,
                initialNPCStats,
                npcWorldBounds,
                npcScreenBounds,
                new AnimatedSprite(
                    npcTextureBounds,
                    npcScreenBounds,
                    npcTexture,
                    npcAnimations,{(AnimationState){npcState,0,0,0}}
                    ));
        TraceLog(LOG_DEBUG,"generate character at world: {pos {%.0f,%.0f}, size {%.0f,%.0f}} screen {pos {%.0f,%.0f}, size {%.0f,%.0f}} statue: %d",
            npcWorldBounds.x, npcWorldBounds.y, npcWorldBounds.width, npcWorldBounds.height,
            npcScreenBounds.x, npcScreenBounds.y, npcScreenBounds.width, npcScreenBounds.height,
            static_cast<int>(npcState));
    
        /// common parameters
        switch ( charType )
        {
            case VILLAGER:
            {
                npc->name = "Villager";
                npc->sprite->textureBounds.y = 0 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleVillager;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackVillager;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackVillager;
                break;
            }
            case GUARD:
            {
                npc->name = "Guard";
                npc->stats.AP = 2;
                npc->stats.EP = 5;
                npc->sprite->textureBounds.y = 2 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleGuard;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackGuard;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackGuard;
                break;
            }
            case MAGE:
            {
                npc->name = "Mage";
                npc->stats.AP = 5;
                npc->stats.EP = 15;
                npc->sprite->textureBounds.y = 3 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleMage;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveMage;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackMage;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackMage;
                break;
            }
            case HERO:
            {
                npc->name = "Hero";
                /// hero for now cosplays as female villager
                npc->sprite->textureBounds.y = 1 * 16;
                npc->strategy[CharacterState::CHAR_IDLE] = strategy::idleHero;
                npc->strategy[CharacterState::CHAR_WALK_E] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_W] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_S] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_WALK_N] = strategy::moveHero;
                npc->strategy[CharacterState::CHAR_ATTACK_N] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_E] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_S] = strategy::attackHero;
                npc->strategy[CharacterState::CHAR_ATTACK_W] = strategy::attackHero;
                break;
            }
            case PLAYER:
                // fall through
            default:
                break;
        }

        this->drawableObjects.push_back(npc);
        this->characters.push_back(npc);
    }
}

void Level::addObject(ObjectType objType, int x, int y)
{
    std::cerr << __func__ << "(" << (int)objType << ", " << x << ", " << y << ")\n";
    Rectangle objectWorldBounds;
    objectWorldBounds.width = 1;
    objectWorldBounds.height = 2;
    WorldObjectStatus initialObjectStats = {10,10};
    Rectangle objectScreenBounds{0,0,1,2};
    Rectangle objectTextureBounds{0,0,16,32};
    const int MAX_OBJECTS = 20;
    this->winThreshold = MAX_OBJECTS/10;
    std::map<CharacterState, Animation> objectAnimations = 
        {{CharacterState::CHAR_IDLE,
            (Animation){-1, {},
            {   {0.2, {0.0,0.0,16.0,32.0}},
                {0.2, {16.0,0.0,16.0,32.0}},
                {0.2, {32.0,0.0,16.0,32.0}},
                {0.2, {48.0,0.0,16.0,32.0}}
            }}},
            {CharacterState::CHAR_CATCH_FIRE,
            (Animation){1, {}, /// TODO add action to transit to CHAR_BURNING
            {   {0.2, {64.0,0.0,16.0,32.0}},
                {0.2, {80.0,0.0,16.0,32.0}},
                {0.2, {96.0,0.0,16.0,32.0}}
            }}},
            {CharacterState::CHAR_BURNING,
            (Animation){-1, {},
            {   {0.2, {112.0,0.0,16.0,32.0}},
                {0.2, {128.0,0.0,16.0,32.0}},
                {0.2, {144.0,0.0,16.0,32.0}}
            }}}};
    objectWorldBounds.x = x;
    objectWorldBounds.y = y;
    objectScreenBounds = LevelScreen::WorldToScreen(screen, objectWorldBounds);
    switch ( objType )
    {
        case ObjectType::HOUSE:
            objectTextureBounds.y = 0 * 32;
            break;
        case ObjectType::TREE:
            objectTextureBounds.y = GetRandomValue(3,4) * 32;
            //objectTextureBounds.y = 3 * 32;
            break;
        case ObjectType::TOWER:
            objectTextureBounds.y = 1 * 32;
            break;
        case ObjectType::WALL:
            /// TODO tower is a wall now ...
            objectTextureBounds.y = 1 * 32;
            break;
        default:
            break;
    }
    Character *obj = 
            new Character("OBJECT", CharacterState::CHAR_IDLE,
                initialObjectStats,
                objectWorldBounds,
                objectScreenBounds,
                new AnimatedSprite(
                    objectTextureBounds,
                    objectScreenBounds,
                    objectTexture,
                    objectAnimations));
    obj->strategy[CharacterState::CHAR_IDLE] = strategy::idleObject;
    obj->strategy[CharacterState::CHAR_BURNING] = strategy::burningObject;
    obj->strategy[CharacterState::CHAR_CATCH_FIRE] = strategy::startBurningObject;
    /// TODO random start time for animation
    obj->sprite->animationState.activeFrame = GetRandomValue(0,obj->sprite->animations[obj->sprite->animationState.activeAnimation].frames.size());
    obj->sprite->animationState.frameDelta = 
        obj->sprite->animations[obj->sprite->animationState.activeAnimation].frames[obj->sprite->animationState.activeFrame].first
         * static_cast<float>(GetRandomValue(0,100))/100.0f;
    this->objects[(GridPos){x,y}] = obj;
    this->drawableObjects.push_back(obj);
}

void Level::removeObject(GridPos pos)
{
    std::map<GridPos, Character*>::iterator objectsIterator = this->objects.find(pos);
    if ( objectsIterator != this->objects.end() )
    {
        Character *obj = objectsIterator->second;
        std::vector<Character*>::iterator drawableIterator;
        for ( drawableIterator = this->drawableObjects.begin(); drawableIterator != this->drawableObjects.end(); ++drawableIterator)
        {
            if ( obj == *drawableIterator )
            {
                break;
            }
        }
        this->objects.erase(objectsIterator);
        this->drawableObjects.erase(drawableIterator);
        delete obj;
    }
}

void Level::updateDistanceMap(DistanceMapType selectedDistanceMap, Vector2 worldTargetPos)
{
    if ( nullptr == this->player )
    {
        return;
    }
    /// TODO: do proper flood-fill or anything better than this !!!
    for ( int y = 0; y < this->levelSize.y; ++y )
    {
        for ( int x = 0; x < this->levelSize.x; ++x )
        {
            // do manhattan distance
            Vector2 delta = {x - worldTargetPos.x, y - worldTargetPos.y};
            distanceMaps[static_cast<DistanceMapType>(selectedDistanceMap)][x][y] = fabs(delta.x) + fabs(delta.y);
        }
    }
}


void Level::movePlayer(float delta)
{
    Vector2 spdDelta = {0.0f,0.0f};
    if ( IsKeyDown(KEY_W) ) // move up
    {
        spdDelta.y -= delta * this->charAcc;
        this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_WALK_N;
    }
    else
    {
        if ( this->charSpeed.y < 0.0 )
        {
            spdDelta.y += delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_A) ) // move left
    {
        spdDelta.x -= delta * this->charAcc;
        this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_WALK_W;
    }
    else
    {
        if ( this->charSpeed.x < 0.0 )
        {
            spdDelta.x += delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_S) ) // move down
    {
        spdDelta.y += delta * this->charAcc;
        this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_WALK_S;
    }
    else
    {
        if ( this->charSpeed.y > 0.0 )
        {
            spdDelta.y -= delta * this->charAcc;
        }
    }
    if ( IsKeyDown(KEY_D) ) // move right
    {
        spdDelta.x += delta * this->charAcc;
        this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_WALK_E;
    }
    else
    {
        if ( this->charSpeed.x > 0.0 )
        {
            spdDelta.x -= delta * this->charAcc;
        }
    }
    /// NOTE: WARNING, unchecked addition - no collision-check against anything, even level boundaries
    this->charSpeed.x += spdDelta.x;
    this->charSpeed.y += spdDelta.y;
    if ( this->charSpeed.x > this->charSpeedMax )
    {
        this->charSpeed.x = this->charSpeedMax;
    }
    if ( this->charSpeed.y > this->charSpeedMax )
    {
        this->charSpeed.y = this->charSpeedMax;
    }
    if ( this->charSpeed.x < -this->charSpeedMax )
    {
        this->charSpeed.x = -this->charSpeedMax;
    }
    if ( this->charSpeed.y < -this->charSpeedMax )
    {
        this->charSpeed.y = -this->charSpeedMax;
    }

    /// TODO intermediate "solution"
    if ( fabs(this->charSpeed.x) > 1.0f )
    {
        this->player->worldBounds.x += this->charSpeed.x * delta;
    }
    if ( fabs(this->charSpeed.y) > 1.0f )
    {
        this->player->worldBounds.y += this->charSpeed.y * delta;
    }
    GridPos newPlayerGridPos = {
        this->player->worldBounds.x,
        this->player->worldBounds.y };
    Vector2 screenPos = LevelScreen::WorldToScreenPos(screen,(Vector2){this->player->worldBounds.x, this->player->worldBounds.y});
    this->player->screenBounds.x = screenPos.x;
    this->player->screenBounds.y = screenPos.y;
    this->player->sprite->screenBounds = this->player->screenBounds;

    if ( IsKeyDown(KEY_SPACE) )
    {
        Direction attackDir = Direction::DIR_E;
        switch (this->player->sprite->animationState.activeAnimation )
        {
            case CharacterState::CHAR_WALK_E:
                // fall through
            case CharacterState::CHAR_ATTACK_E:
                this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_ATTACK_E;
                attackDir = Direction::DIR_E;
                break;
            case CharacterState::CHAR_WALK_W:
                // fall through
            case CharacterState::CHAR_ATTACK_W:
                this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_ATTACK_W;
                attackDir = Direction::DIR_W;
                break;
            case CharacterState::CHAR_WALK_S:
                // fall through
            case CharacterState::CHAR_ATTACK_S:
                this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_ATTACK_S;
                attackDir = Direction::DIR_S;
                break;
            case CharacterState::CHAR_WALK_N:
                // fall through
            case CharacterState::CHAR_ATTACK_N:
                this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_ATTACK_N;
                attackDir = Direction::DIR_N;
                break;
            default:
                break;
        }
        performAttack(this->player, delta, playerAttackPattern[attackDir]);
        /// simple NPC kill
        Character *hit = this->getCollision(this->player, this->player->worldBounds);
        if ( hit )
        {
            hit->stats.HP = 0;
        }
    }
    if ( newPlayerGridPos != this->lastPlayerGridPos )
    {
        lastPlayerGridPos = newPlayerGridPos;
        updateDistanceMap(DistanceMapType::PLAYER_DISTANCE,
            {this->player->worldBounds.x+1, this->player->worldBounds.y+1},
            true, true,
            200);
    }
}

void Level::applyDmgPattern(float baseDmg, GridPos center, std::vector<GridPos> *attackPattern, bool addParticles )
{
    float dmg = baseDmg / attackPattern->size();
    for ( GridPos pos : *attackPattern )
    {
        GridPos tmp = center + pos;
        if ( inBounds(tmp) )
        {
            // damage
            if ( this->objects.count(tmp) )
            {
                this->objects[tmp]->stats.HP -= dmg;
            }
            // just fancy effects, no dmg
            if ( addParticles )
            {
                this->nextParticles.push_back(new Particle(new AnimatedSprite(
                    // texture bounds
                    {0.0,16.0,16.0,16.0},
                    // screen bounds
                    {tmp.x, tmp.y, 1, 1},
                    Datastore::getInstance().getTexture("images/ui.png"),
                    {{CharacterState::CHAR_IDLE,
                    (Animation){-1, {},
                    {   {0.2, {0.0,0.0,16.0,16.0}},
                        {0.2, {16.0,0.0,16.0,16.0}},
                        {0.2, {32.0,0.0,16.0,16.0}},
                        {0.2, {48.0,0.0,16.0,16.0}},
                        {0.2, {64.0,0.0,16.0,16.0}},
                        {0.2, {80.0,0.0,16.0,16.0}}
                    }}}}),
                    {center.x + 0.5 * pos.x, center.y + 0.5 * pos.y},
                    {pos.x*2.0f, pos.y*2.0f}, 1.0, 0, [](Particle *notUsed,Level *alsoNotUsed) { /* do nothing*/; return false; }));
            }
        }
    }
}

void Level::launchProjectile(float dmg,
                                    Rectangle start,
                                    Vector2 dir,
                                    float lifetime,
                                    AnimatedSprite *animation)
{
    /// exploding projectile
    this->nextParticles.push_back(new Particle(animation,
        start,
        dir,
        lifetime,
        dmg,
        [&](Particle *particle, Level *level) {
            float dmg = particle->dmg;
            GridPos targetPos =  {particle->pos.x, particle->pos.y};
            level->applyDmgPattern(dmg, targetPos, &explosionPattern, true);
            /* do nothing*/; return false; } ));
}

void Level::performAttack(Character *source, float delta, std::vector<GridPos> directedAttackPattern)
{
    if ( directedAttackPattern.size() == 0 )
    {
        return;
    }
    if ( nullptr == source )
    {
        return;
    }
    if ( source->stats.EP <= 0.0 )
    {
        source->stats.EP = 0.0;
        return;   
    }
    GridPos center = {source->worldBounds.x + 0.5f, source->worldBounds.y + 0.5};
    float baseDmg = source->stats.AP * delta;
    Rectangle worldBounds = LevelScreen::WorldToScreen(screen, source->worldBounds);
    applyDmgPattern(baseDmg, center, &directedAttackPattern, true); 
    source->stats.EP -= fabs(baseDmg);
}

void Level::updateDistanceMap(DistanceMapType type, GridPos pos, bool clean, bool ignoreObjects, int distMax)
{
    /// reset
    if ( clean )
    {
        std::for_each(std::execution::par_unseq,
                    this->distanceMaps[type].begin(),
                    this->distanceMaps[type].end(), 
                    [](std::vector<int> &dist){
                        std::memset(&dist[0], -1, sizeof(int)*dist.size());
                        /*
                        std::for_each(std::execution::par_unseq,
                                        dist.begin(),
                                        dist.end(),
                                        [](int &n) { n = -1; });
                                        */
        });
    }
    const int MAX_STEP = 1000;
    bool done = false;
    using Dist_t = struct {int dist; GridPos pos;};
    const GridPos directions[] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    std::queue<Dist_t> queue;
    queue.push((Dist_t){0, pos});
    for ( int step = 0; (!queue.empty()) && step < MAX_STEP; ++step )
    {
        Dist_t current = queue.front();
        if ( current.dist > distMax )
        {
            break;
        }
        queue.pop();

        for ( int d = 0; d < 4; ++d )
        {
            GridPos pos = current.pos + directions[d];
            if ( (pos.x == 0 && pos.y == 0)
                 || pos.x <= 0 || pos.x >= this->levelSize.x
                 || pos.y <= 0 || pos.y >= this->levelSize.y )
            {
                continue;
            }
            bool collision = false;
            if ( ignoreObjects && (this->objects.count(pos) > 0 || this->objects.count({pos.x,pos.y-1}) > 0))
            {
                continue;
            }
            if ( (this->distanceMaps[type][pos.x][pos.y] == -1 ||
                    current.dist < this->distanceMaps[type][pos.x][pos.y])
                )
            {
                this->distanceMaps[type][pos.x][pos.y] = current.dist;
                queue.push((Dist_t){current.dist+1, pos.x, pos.y});
            }
            else
            {
                continue;
            }
        }
    }
}

bool Level::checkCollision(Character *source, Rectangle worldBounds)
{
    /// TODO HELL NO THIS IS SLOW - USE A QUADTREE OR SOMETHING; BUT NOT THIS WAAAAAAAAH
    /// this is complexity O(n^2) ... there is a solution to do this in worst case O(nlogn)
    for ( Character *target : this->drawableObjects )
    {
        if ( target != source && CheckCollisionPointRec({worldBounds.x + 0.5f, worldBounds.y + 0.5f}, target->worldBounds))
        {
            return true;
        }
    }
    return false;
}

Character* Level::getCollision(Character *source, Rectangle worldBounds)
{
    /// TODO HELL NO THIS IS SLOW - USE A QUADTREE OR SOMETHING; BUT NOT THIS WAAAAAAAAH
    /// this is complexity O(n^2) ... there is a solution to do this in worst case O(nlogn)
    for ( Character *target : this->drawableObjects )
    {
        if ( target != source && CheckCollisionRecs(worldBounds, target->worldBounds))
        {
            return target;
        }
    }
    return nullptr;
}

void Level::update(float delta)
{
    updateObjects(delta);
    updateNPCs(delta);
    updateParticles(delta);

    // move
    movePlayer(delta);
    moveNPCs(delta);
}

bool Level::isReady()
{
    return IsTextureReady(this->npcTexture) && IsTextureReady(this->objectTexture);
}

void Level::moveNPCs(float delta)
{
    float charSpeed = 2.0;
    for ( Character *npc : this->characters )
    {
        Vector2 deltaPos = { 0, 0};
        bool isMoving = true;
        switch( npc->state )
        {
            case CharacterState::CHAR_WALK_E:
                deltaPos.x += delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_W:
                deltaPos.x -= delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_N:
                deltaPos.y -= delta * charSpeed;
                break;
            case CharacterState::CHAR_WALK_S:
                deltaPos.y += delta * charSpeed;
                break;
            default:
                isMoving = false;
                break;
        }
        if ( isMoving )
        {
            Rectangle tempBounds = npc->worldBounds;
            tempBounds.x += deltaPos.x;
            if ( not this->checkCollision(npc, tempBounds) )
            {
                npc->worldBounds = tempBounds;
                npc->screenBounds = LevelScreen::WorldToScreen(screen, tempBounds);
                npc->sprite->screenBounds = npc->screenBounds;
                TraceLog(LOG_DEBUG,"%s(%s) by {%.0f,%.0f} to screenPos {%.0f,%.0f}",
                    __func__, npc->name, deltaPos.x, deltaPos.y, npc->screenBounds.x, npc->screenBounds.y);
            }
            tempBounds.y += deltaPos.y;
            if ( not this->checkCollision(npc, tempBounds) )
            {
                npc->worldBounds = tempBounds;
                npc->screenBounds = LevelScreen::WorldToScreen(screen, tempBounds);
                npc->sprite->screenBounds = npc->screenBounds;
                TraceLog(LOG_DEBUG,"%s(%s) by {%.0f,%.0f} to screenPos {%.0f,%.0f}",
                    __func__, npc->name, deltaPos.x, deltaPos.y, npc->screenBounds.x, npc->screenBounds.y);
            }
        }
    }

}

void Level::updateNPCs(float delta)
{
    for ( Character *npc : this->characters )
    {
        if ( npc->strategy.count(npc->state) > 0 )
        {
            npc->strategy[npc->state](npc, delta, this->distanceMaps);
        }
    }
}

void Level::updateObjects(float delta)
{
    std::vector<GridPos> deleteList;
    for ( auto tmp : this->objects )
    {
        Character *obj = tmp.second;
        if ( obj->strategy.count(obj->state) > 0 )
        {
            bool stateChange = obj->strategy[obj->state](obj, delta, this->distanceMaps);
            if ( stateChange )
            {
            }
        }
        if ( obj->stats.HP <= 0 )
        {
            this->updateDistanceMap(DistanceMapType::FIRE_DISTANCE,
                tmp.first, false, false, -1);
            std::cerr << "UPDATING FIRE distance for OBJECT at " << tmp.first.x << " " << tmp.first.y << "\n";
            deleteList.push_back(tmp.first);
        }
    }
    for ( auto pos : deleteList )
    {
        this->removeObject(pos);
    }
}

void Level::updateParticles(float delta)
{
    std::vector<Particle*> keepParticles;
    std::vector<Particle*> deleteParticles;
    for ( Particle *p : this->particles )
    {
        if ( p->update(delta, this) )
        {
            keepParticles.push_back(p);
        }
        else
        {
            deleteParticles.push_back(p);
        }
    }
    this->particles = keepParticles;
    this->particles.insert(this->particles.end(), this->nextParticles.begin(), this->nextParticles.end());
    this->nextParticles.clear();
    for ( Particle *p : deleteParticles )
    {
        delete p;
    }
}


void Level::checkWinCondition()
{
    if ( player->stats.HP > 0 )
    {
        int sumRemaining = 0;
        for ( auto tmp : this->objects )
        {
            Character *obj = tmp.second;
            if ( obj->stats.HP > 0 )
            {
                ++sumRemaining;
            }
        }
        //std::cerr << "sumRemaining: " << sumRemaining << " threshold: " << this->winThreshold << "\n";
        if ( sumRemaining < this->winThreshold )
        {
            this->isDone = true;
        }
    }
    else
    {
        this->player->sprite->animationState.activeAnimation = CharacterState::CHAR_DIE;
        std::cerr << "gameover, player HP: " << player->stats.HP << "\n";
        this->isGameOver = true;
    }
}
void Level::sortDrawableObjects()
{
    // sort by y-value
    std::sort(this->drawableObjects.begin(), this->drawableObjects.end(),
        [](Character *a, Character *b) {
            return a->screenBounds.y < b->screenBounds.y;
        }
    );
}

void Level::draw(float delta, LevelScreen *screen)
{
    sortDrawableObjects();
    std::vector<GridPos> deleteList;

    for ( Character *object : this->drawableObjects )
    {
        if ( object->sprite && object->stats.HP > 0 )
        {
            object->sprite->draw(delta);
        }
        else
        {
            deleteList.push_back({object->worldBounds.x, object->worldBounds.y});
        }
    }
    for ( GridPos pos : deleteList )
    {
        this->removeObject(pos);
    }

    for (Particle *particle : this->particles)
    {
        if ( particle->isAlive() )
        {
            particle->draw(delta, screen);
        }
    }
}

