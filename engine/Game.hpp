#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include "../ecs/ECS.hpp"
#include "../game/Components.hpp"
#include "../game/Systems.hpp"

class Game
{
public:
    bool init(int width = 800, int height = 600)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        window = SDL_CreateWindow("SDL ECS Visualization",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  width, height, 0);
        if (!window)
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            return false;
        }

        running = true;
        previousTime = SDL_GetTicks();

        // === Scenes ECS ===
        scenePosVel = &manager.createScene<TypeList<Position, Velocity>>("PosVelScene", true);
        sceneHealMana = &manager.createScene<TypeList<Heal, Mana>>("HealManaScene", true);

        // Position/Velocity entity
        Entity e1 = scenePosVel->getRegistry().create();
        scenePosVel->getRegistry().add<Position>(e1, {100,100});
        scenePosVel->getRegistry().add<Velocity>(e1, {50,30});
        scenePosVel->addSystem(new MovementSystem);

        // Heal/Mana entities
        Entity e2 = sceneHealMana->getRegistry().create();
        sceneHealMana->getRegistry().add<Heal>(e2, {20});
        sceneHealMana->getRegistry().add<Mana>(e2, {15});

        Entity e22 = sceneHealMana->getRegistry().create();
        sceneHealMana->getRegistry().add<Heal>(e22, {10});
        sceneHealMana->getRegistry().add<Mana>(e22, {25});

        sceneHealMana->addSystem(new RegenSystem, 10);
        sceneHealMana->addSystem(new PrintSystem, 50);

        return true;
    }

    void run()
    {
        const float fixedDelta = 1.0f / 60.0f;
        float accumulator = 0.f;

        while (running)
        {
            Uint32 current = SDL_GetTicks();
            float frameTime = (current - previousTime) / 1000.f;
            previousTime = current;
            accumulator += frameTime;

            handleEvents();

            while (accumulator >= fixedDelta)
            {
                update(fixedDelta);
                accumulator -= fixedDelta;
            }

            render();
        }
    }

    void clean()
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;
    Uint32 previousTime = 0;

    GameManager manager;
    Scene<TypeList<Position, Velocity>>* scenePosVel;
    Scene<TypeList<Heal, Mana>>* sceneHealMana;

    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
        }
    }

    void update(float dt)
    {
        scenePosVel->update(dt);
        sceneHealMana->update(dt);
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 30,30,30,255);
        SDL_RenderClear(renderer);

        // --- Draw Position/Velocity entities ---
        auto& regPV = scenePosVel->getRegistry();
        for (Entity e : regPV.getAliveEntities())
        {
            if (auto* pos = regPV.getIf<Position>(e))
            {
                SDL_Rect rect = { int(pos->x), int(pos->y), 20, 20 };
                SDL_SetRenderDrawColor(renderer, 100,200,150,255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // --- Draw Heal/Mana entities as bars ---
        auto& regHM = sceneHealMana->getRegistry();
        int yOffset = 400;
        int barWidth = 100, barHeight = 10, spacing = 5;
        for (Entity e : regHM.getAliveEntities())
        {
            if (auto* heal = regHM.getIf<Heal>(e))
            {
                auto* mana = regHM.getIf<Mana>(e);
                int hpWidth = std::min(heal->hp*5, barWidth);
                int mpWidth = std::min(mana->mp*5, barWidth);

                SDL_Rect hpBar = {50, yOffset, hpWidth, barHeight};
                SDL_Rect mpBar = {50, yOffset+barHeight+spacing, mpWidth, barHeight};

                SDL_SetRenderDrawColor(renderer, 200,50,50,255); // red HP
                SDL_RenderFillRect(renderer, &hpBar);

                SDL_SetRenderDrawColor(renderer, 50,50,200,255); // blue MP
                SDL_RenderFillRect(renderer, &mpBar);

                yOffset += 2*(barHeight+spacing);
            }
        }

        SDL_RenderPresent(renderer);
    }
};