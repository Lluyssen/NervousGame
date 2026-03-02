#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "../game/GameContext.h"
#include "State.hpp"

class IGameState;

class StateManager
{

private:
    GameContext &context;
    std::unique_ptr<IGameState> current;

public:
    explicit StateManager(GameContext &ctx) : context(ctx) {}

    template <typename T, typename... Args>
    void changeState(Args &&...args)
    {
        if (current)
            current->onExit(*this);

        current = std::make_unique<T>(std::forward<Args>(args)...);
        current->onEnter(*this);
    }

    void handleEvent(const SDL_Event &e)
    {
        if (current)
            current->handleEvent(*this, e);
    }

    void update(float dt)
    {
        if (current)
            current->update(*this, dt);
    }

    void render()
    {
        if (current)
            current->render(*this);
    }

    GameContext &getContext() { return context; }
};