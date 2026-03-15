#pragma once

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "raylib.h"
#include "MusicManager.hpp"

/*
    Contexte global partagé par tous les states.
    Permet d'accéder aux ressources du jeu.
*/

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

    // Cache de textures
    std::unordered_map<std::string, Texture2D> _textures;

    MusicManager _music;
    bool _musicLoaded = false;

    LevelCompletion _levelCompletion;

public:
    GameContext(void) = default;

    ~GameContext()
    {
        unloadAllTextures();
    }

    // -------- Screen --------

    void setSelectedLevel(int id) { _selectedLevel = id; }
    int getSelectedLevel(void) const { return _selectedLevel; }

    // -------- Progression --------

    int getHighestUnlockedLevel(void) const
    {
        return _highestUnlockedLevel;
    }

    void setHighestUnlockedLevel(int level)
    {
        _highestUnlockedLevel = level;
    }

    // -------- Texture Management --------

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

    void unloadAllTextures()
    {
        for (auto &[path, tex] : _textures)
            UnloadTexture(tex);

        _textures.clear();
    }

    // -------- Music --------

    void initMusic(const std::string &path)
    {
        if (!_musicLoaded)
        {
            _music.load(path);
            _musicLoaded = true;
        }
    }

    void updateMusic()
    {
        if (_musicLoaded)
            _music.update();
    }

    // -------- Level --------

    void setLevelCompleted(int id)
    {
        _levelCompletion.pending = true;
        _levelCompletion.completedLevel = id;
    }

    bool hasPendingLevelCompletion(void) const
    {
        return _levelCompletion.pending;
    }

    int getCompletedLevel(void) const
    {
        return _levelCompletion.completedLevel;
    }

    void clearLevelCompletion(void)
    {
        _levelCompletion.pending = false;
    }
};