#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "GameState.hpp"

class MenuState : public IGameState 
{
public:
    void handleEvent(StateManager& sm, const SDL_Event& e) override 
    {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
            sm.changeState<GameState>();
    }

    void update(StateManager&, float) override {}

    void render(StateManager&) override
    {
        // draw menu (text, buttons…)
    }
};