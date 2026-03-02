#pragma once

#include <SDL2/SDL.h>

// Le GameContext contient tous les services globaux du jeu.
// Il ne contient AUCUNE logique de gameplay.
struct GameContext 
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Extensions futures possibles :
    // InputManager* input = nullptr;
    // AssetManager* assets = nullptr;
    // AudioManager* audio = nullptr;
    // Scene* gameScene = nullptr;
};