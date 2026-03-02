#pragma once

#include <SDL2/SDL.h>

class StateManager;

class IGameState 
{
public:
    virtual ~IGameState() = default;

    virtual void onEnter(StateManager&) {}
    virtual void onExit(StateManager&) {}

    virtual void handleEvent(StateManager&, const SDL_Event&) = 0;
    virtual void update(StateManager&, float dt) = 0;
    virtual void render(StateManager&) = 0;
};