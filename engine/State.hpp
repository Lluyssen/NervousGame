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
    virtual void update(StateManager&, float) = 0;
    virtual void render(StateManager&) = 0;

    virtual bool allowUpdateBelow() const { return false; }
    virtual bool allowRenderBelow() const { return true; }
};