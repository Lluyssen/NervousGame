#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "raylib.h"
#include "MusicManager.hpp"

struct LevelCompletion
{
    bool pending = false;
    int completedLevel = -1;
};

class GameContext
{
private:
    int _highestUnlockedLevel = 0;
    int _selectedLevel = -1;
    LevelCompletion _levelCompletion;

    std::unordered_map<std::string, Texture2D> _textures;

    MusicManager _music;
    bool _musicLoaded = false;

    bool _menuBackgroundLoaded = false;

public:
    GameContext(void) = default;

    ~GameContext(void)
    {
        unloadAllTextures();
    }

    void setBackGroundLoaded(bool bi) { _menuBackgroundLoaded = bi; }
    bool getBackGroundLoaded(void) { return _menuBackgroundLoaded; }

    void setSelectedLevel(int id) { _selectedLevel = id; }
    int getSelectedLevel(void) const { return _selectedLevel; }

    int getHighestUnlockedLevel(void) const
    {
        return _highestUnlockedLevel;
    }

    void setHighestUnlockedLevel(int level)
    {
        _highestUnlockedLevel = level;
    }

    Texture2D &loadTexture(const std::string &path)
    {
        if (!IsWindowReady())
        {
            throw std::runtime_error(
                "Attempted to LoadTexture before InitWindow()");
        }

        auto it = _textures.find(path);
        if (it != _textures.end())
            return it->second;

        Texture2D tex = LoadTexture(path.c_str());
        auto [iter, inserted] = _textures.emplace(path, tex);

        return iter->second;
    }

    Texture2D &getTexture(const std::string &path)
    {
        return loadTexture(path);
    }

    void unloadAllTextures(void)
    {
        for (auto &[_, tex] : _textures)
            UnloadTexture(tex);

        _textures.clear();
    }

    void initMusic(const std::string &path)
    {
        if (!_musicLoaded)
        {
            _music.load(path);
            _musicLoaded = true;
        }
    }

    void updateMusic(void)
    {
        if (_musicLoaded)
            _music.update();
    }

    void setLevelCompleted(int id)
    {
        _levelCompletion.pending = true;
        _levelCompletion.completedLevel = id;
    }

    bool hasPendingLevelCompletion(void) const { return _levelCompletion.pending; }

    int getCompletedLevel(void) const { return _levelCompletion.completedLevel; }

    void clearLevelCompletion(void) { _levelCompletion.pending = false; }
};