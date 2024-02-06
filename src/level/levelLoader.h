#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

#include <string>

/// forward declaration for level
class Level;
/// forward declaration for screen
class LevelScreen;

/// factory/loader for levels
class LevelLoader
{
public:
    static Level *loadLevelFromImage(std::string path, LevelScreen *screen);
};

#endif // LEVEL_LOADER_H