#pragma once
#include <unordered_map>
#include <string>
#include "raylib.h"

class AssetManager
{
private:
    std::unordered_map<std::string, Texture2D> _textures;
    std::unordered_map<std::string, Sound> _sounds;

public:

    Texture2D& getTexture(const std::string& path)
    {
        auto it = _textures.find(path);

        if (it != _textures.end())
            return it->second;

        Texture2D tex = LoadTexture(path.c_str());
        _textures[path] = tex;

        return _textures[path];
    }

    Sound& getSound(const std::string& path)
    {
        auto it = _sounds.find(path);

        if (it != _sounds.end())
            return it->second;

        Sound snd = LoadSound(path.c_str());
        _sounds[path] = snd;

        return _sounds[path];
    }

    void unload()
    {
        for (auto& [path, tex] : _textures)
            UnloadTexture(tex);

        for (auto& [path, snd] : _sounds)
            UnloadSound(snd);
    }
};