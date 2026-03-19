#pragma once

#include <core/StateManager.hpp>
#include "raylib.h"
#include <states/MapState.hpp>
#include <core/Utils.hpp>
#include <states/levelSelection/RiverSystem.hpp>
#include <states/levelSelection/FogSystem.hpp>
#include <states/levelSelection/Maprenderer.hpp>
#include <states/levelSelection/LevelNodeSystem.hpp>
#include <states/levelSelection/AmbientParticleSystem.hpp>
#include <states/levelSelection/LiquidDecorationSystem.hpp>
#include <states/levelSelection/TooltipSystem.hpp>
#include <core/SystemManager.hpp>

// Niveaux définis en coordonnées normalisées (0..1)
// Cela permet de rendre la map indépendante de la résolution écran.
static const std::array<LevelNode, 5> LEVELS = {{{0, {0.139f, 0.215f}},
                                                 {1, {0.173f, 0.720f}},
                                                 {2, {0.467f, 0.610f}},
                                                 {3, {0.696f, 0.230f}},
                                                 {4, {0.671f, 0.730f}}}};

static const std::array<Vector2, 6> RIVER_POS = {{{0.366146f, 0.0824074f},
                                                  {0.214583f, 0.280556f},
                                                  {0.183854f, 0.392593f},
                                                  {0.238021f, 0.509259f},
                                                  {0.358333f, 0.687037f},
                                                  {0.379167f, 0.837963f}}};

class LevelSelectionState : public IGameState
{
private:
    engine::SystemManager _systems;

    MapRenderer _map;
    FogSystem _fog;
    RiverSystem _river;
    LevelNodeSystem _nodes;
    LiquidDecorationSystem _liquid;
    AmbientParticleSystem _ambient;
    TooltipSystem _tooltip;

    float _uiScale = 1.f;

    int _prevW = 0;
    int _prevH = 0;

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        const int w = GetScreenWidth();
        const int h = GetScreenHeight();

        _prevW = w;
        _prevH = h;

        _uiScale = std::min((float)w / 1920.f, (float)h / 1080.f);

        _map.load();
        _fog.setTexture("../assets/ui/levelSelect/fog.png");
        _river.setup(RIVER_POS);
        _nodes.setup(sm, _tooltip, _ambient, LEVELS, _uiScale);

        _systems.clear();

        _systems.add(_tooltip);
        _systems.add(_fog);
        _systems.add(_river);
        _systems.add(_liquid);
        _systems.add(_nodes);
        _systems.add(_ambient);

        _systems.init(ctx);
        _systems.resize(ctx, w, h);
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        const int w = GetScreenWidth();
        const int h = GetScreenHeight();

        if (w != _prevW || h != _prevH)
        {
            _systems.resize(ctx, w, h);

            _prevW = w;
            _prevH = h;
        }

        _systems.update(ctx, dt);

        ctx.updateMusic();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        const int w = GetScreenWidth();
        const int h = GetScreenHeight();

        ClearBackground(BLACK);

        _map.draw(ctx.getCompletedLevel(), w, h);

        _systems.draw(ctx);
    }

    void onExit(StateManager &) override
    {
        _systems.shutdown();
        _map.unload();
    }
};