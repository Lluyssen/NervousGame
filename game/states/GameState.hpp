#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "PauseState.hpp"

class GameState : public IGameState
{
private:
    Scene<TypeList<Position, Velocity>> *scenePosVel;
    Scene<TypeList<Heal, Mana>> *sceneHealMana;
    GameManager manager;

public:

    void onEnter(StateManager&) override 
    {
        std::cout << "[GameState] Enter\n";
        // === Scenes ECS ===
        
        scenePosVel = &manager.createScene<TypeList<Position, Velocity>>("PosVelScene", true);
        sceneHealMana = &manager.createScene<TypeList<Heal, Mana>>("HealManaScene", true);

        // Position/Velocity entity
        Entity e1 = scenePosVel->getRegistry().create();
        scenePosVel->getRegistry().add<Position>(e1, {100, 100});
        scenePosVel->getRegistry().add<Velocity>(e1, {50, 30});
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
    }

    void onExit(StateManager&) override 
    {
        std::cout << "[GameState] Exit\n";
    }

    void handleEvent(StateManager &sm, const SDL_Event &e) override
    {

        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            sm.changeState<PauseState>();
    }

    void update(StateManager &, float dt) override
    {
        scenePosVel->update(dt);
        sceneHealMana->update(dt);
    }

    void render(StateManager &sm) override
    {
        auto* renderer = sm.getContext().renderer;
        // --- Draw Position/Velocity entities ---
        auto &regPV = scenePosVel->getRegistry();
        for (Entity e : regPV.getAliveEntities())
        {
            if (auto *pos = regPV.getIf<Position>(e))
            {
                SDL_Rect rect = {int(pos->x), int(pos->y), 20, 20};
                SDL_SetRenderDrawColor(renderer, 100, 200, 150, 255);
                SDL_RenderFillRect(renderer, &rect);
            }
        }

        // --- Draw Heal/Mana entities as bars ---
        auto &regHM = sceneHealMana->getRegistry();
        int yOffset = 400;
        int barWidth = 100, barHeight = 10, spacing = 5;
        for (Entity e : regHM.getAliveEntities())
        {
            if (auto *heal = regHM.getIf<Heal>(e))
            {
                auto *mana = regHM.getIf<Mana>(e);
                int hpWidth = std::min(heal->hp * 5, barWidth);
                int mpWidth = std::min(mana->mp * 5, barWidth);

                SDL_Rect hpBar = {50, yOffset, hpWidth, barHeight};
                SDL_Rect mpBar = {50, yOffset + barHeight + spacing, mpWidth, barHeight};

                SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255); // red HP
                SDL_RenderFillRect(renderer, &hpBar);

                SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255); // blue MP
                SDL_RenderFillRect(renderer, &mpBar);

                yOffset += 2 * (barHeight + spacing);
            }
        }
    }
};