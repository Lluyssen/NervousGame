#pragma once

#include <SDL2/SDL.h>
#include "../engine/AssetManager.hpp"

// Le GameContext contient tous les services globaux du jeu.
// Il ne contient AUCUNE logique de gameplay.
struct GameContext 
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    AssetManager* assets = nullptr;

    // Extensions futures possibles :
    // InputManager* input = nullptr;
    // AudioManager* audio = nullptr;
    // Scene* gameScene = nullptr;
};