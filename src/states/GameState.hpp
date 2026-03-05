#pragma once

#include "../utils/StateManager.hpp"
#include "MenuState.hpp"
#include "raylib.h"

class MenuState;

class GameState : public IGameState
{
public:

    void onEnter(StateManager&) override
    {
        // Initialisation du gameplay
    }

    void onExit(StateManager&) override
    {
        // Nettoyage si nécessaire
    }

    void update(StateManager& sm, float) override
    {
        // Retour au menu
        if (IsKeyPressed(KEY_ESCAPE))
        {
            sm.changeState<MenuState>();
        }
    }

    void render(StateManager&) override
    {
        DrawText("GAME RUNNING", 450, 300, 40, GREEN);
        DrawText("Press ESC to return to menu", 380, 360, 20, LIGHTGRAY);
    }
};