#ifndef DATASTORE_H
#define DATASTORE_H
#include "raylib.h"
#include <string>
#include <map>

/// singleton to manage media
/// TODO add functions to remove from database
/// TODO do some reference-counting
/// TODO add "wait-for" functions that check readyness of a list of elements?
class Datastore
{
public:
    /// singleton function
    static Datastore &getInstance()
    {
        static Datastore singleton;
        return singleton;
    }
    /// get texture @param path path @note uses lazy loading
    Texture getTexture(std::string path)
    {
        if ( this->textures.count(path) == 0 )
        {
            this->textures[path] = LoadTexture(path.c_str());
        }
        return textures[path];
    }
    /// get sound @param path path @note uses lazy loading
    Sound getSound(std::string path)
    {
        if ( this->sounds.count(path) == 0 )
        {
            this->sounds[path] = LoadSound(path.c_str());
        }
        return sounds[path];
    }
private:
    Datastore() {}
    Datastore(Datastore const&);
    /// texture database
    std::map<std::string, Texture> textures;
    /// sound database
    std::map<std::string, Sound> sounds;
};
#endif // DATASTORE_H