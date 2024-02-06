#ifndef LEVEL_H
#define LEVEL_H
#define MAX_LEVEL_SIZE_X 1000u
#define MAX_LEVEL_SIZE_Y 1000u

#include "character.h"
#include "particle.h"
#include <map>

/// position in 2D Grid
typedef struct {int x; int y;} GridPos;
/// less-operator for GridPos
inline bool operator<(const GridPos &a, const GridPos &b)
{
    return (a.x + MAX_LEVEL_SIZE_X * a.y) < (b.x + MAX_LEVEL_SIZE_X * b.y);
}
/// equals operator for GridPos
inline bool operator==(const GridPos &a, const GridPos &b)
{
    return (a.x + MAX_LEVEL_SIZE_X * a.y) == (b.x + MAX_LEVEL_SIZE_X * b.y);
}
/// inequality operator for GridPos
inline bool operator!=(const GridPos &a, const GridPos &b)
{
    return !(a==b);
}
/// sum operator for GridPos
inline GridPos operator+(const GridPos &a, const GridPos &b)
{
    return (GridPos){a.x+b.x, a.y+b.y};
}
/// pattern where to apply damage to
typedef std::vector<GridPos> AttackPattern;
/// map of directions to attack patterns
typedef std::map<Direction,AttackPattern> AttackPatternMap;


/// forward declaration for Level
class LevelScreen;
class Level
{
    public:
    /// @todo remove link to screen, if possible
    Level(LevelScreen *screen) :
        screen(screen),
        lastPlayerGridPos({0,0}),
        isDone(false),
        isGameOver(false) {}
    bool inBounds(GridPos p)
    {
        return p.x >= 0 && p.y >= 0 && p.x < this->levelSize.x && p.y < this->levelSize.y;
    }
    void draw(float delta, LevelScreen *screen);
    Vector2 levelSize;
    Sound fireBreath;
    Vector2 charSpeed;
    float charSpeedMax;
    float charAcc;
    std::vector<Character*> characters;
    /// TBD: is there no way to actually use the tileMap for this?
    std::map<GridPos,Character*> objects;
    std::vector<Character*> drawableObjects;
    std::vector<Particle*> particles;
    std::vector<Particle*> nextParticles;
    /// this is ugly
    GridPos lastPlayerGridPos;
    Character *player;
    void addObject(ObjectType objType, int x, int y);
    void addCharacter(CharacterType charType, int x, int y);
    void loadSounds();
    void updateNPCs(float delta);
    void updateParticles(float delta);
    Texture2D objectTexture, npcTexture;
    /// distance maps - mapped per distance-type
    MappedDistanceMaps distanceMaps;
    void performAttack(Character *source, float delta, std::vector<GridPos> directedAttackPattern);
    void applyDmgPattern(float baseDmg, GridPos pos, std::vector<GridPos> *attackPattern, bool addParticles );
    void launchProjectile(float dmg,
                Rectangle start,
                Vector2 dir,
                float lifetime,
                AnimatedSprite *animation);
    void updateObjects(float delta);
    bool checkCollision(Character *source, Rectangle worldBounds);
    Character* getCollision(Character *source, Rectangle worldBounds);
    void updateDistanceMap(DistanceMapType selectedDistanceMap, Vector2 worldTargetPos);
    void updateDistanceMap(DistanceMapType type, GridPos pos, bool clean, bool ignoreObjects, int distMax);


    void movePlayer(float delta); ///< extract input-stuff 
    void moveNPCs(float delta); ///< merge with updateNPCs

    void sortDrawableObjects();
    void update(float delta);
    bool isReady();
    void checkWinCondition();
    bool isDone;
    bool isGameOver;
private:
    LevelScreen *screen;
    int winThreshold;
};

#endif // LEVEL_H