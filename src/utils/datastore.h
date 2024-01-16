#ifndef DATASTORE_H
#define DATASTORE_H
#include "raylib.h"
#include <string>
#include <map>

class Datastore
{
public:
    static Datastore &getInstance()
    {
        static Datastore singleton;
        return singleton;
    }
    Texture getTexture(std::string path)
    {
        if ( this->textures.count(path) == 0 )
        {
            this->textures[path] = LoadTexture(path.c_str());
        }
        return textures[path];
    }
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
    std::map<std::string, Texture> textures;
    std::map<std::string, Sound> sounds;
};
#endif // DATASTORE_H