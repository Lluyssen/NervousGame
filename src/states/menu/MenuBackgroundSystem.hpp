#pragma once

#include "raylib.h"
#include <ui/AnimatedSprite.hpp>
#include <core/SystemManager.hpp>

// Gère le fond animé du menu avec un effet de parallaxe basé sur la souris.
class MenuBackgroundSystem : public engine::System
{
private:
    AnimatedSprite _background;
    Vector2 _offset{0.f, 0.f};

    int _w = 0;
    int _h = 0;

    int _loaded = 0;
    static constexpr int TOTAL = 41;

    bool _ready = false;
    bool _initialized = false;

    std::string _basePath = "../assets/ui/bg/frame";

public:
    bool isReady(void) const
    {
        return _ready;
    }

    float progress(void) const
    {
        if (TOTAL == 0)
            return 1.f;
        return (float)_loaded / (float)TOTAL;
    }

    void init(GameContext &ctx)
    {
        if (ctx.getBackGroundLoaded())
        {
            _loaded = TOTAL;

            for (int i = 0; i < TOTAL; ++i)
                _background.loadFrame(ctx, _basePath, i);

            _background.finalize(0.1f);

            _ready = true;
            return;
        }

        _loaded = 0;
        _ready = false;
        _initialized = true;
    }

    void update(GameContext &ctx, float dt)
    {
        if (!_ready)
        {
            double start = GetTime();

            while (_loaded < TOTAL)
            {
                _background.loadFrame(ctx, _basePath, _loaded++);
                if (GetTime() - start > 0.002)
                    break;
            }

            if (_loaded >= TOTAL)
            {
                _background.finalize(0.1f);
                _ready = true;
                ctx.setBackGroundLoaded(true);
            }

            return;
        }

        _background.update(dt);

        Vector2 mouse = GetMousePosition();

        float nx = (mouse.x / _w) - 0.5f;
        float ny = (mouse.y / _h) - 0.5f;

        constexpr float strength = 30.0f;

        _offset.x += (nx * strength - _offset.x) * 5.0f * dt;
        _offset.y += (ny * strength - _offset.y) * 5.0f * dt;
    }

    void draw(GameContext &)
    {
        _background.drawFullscreen(_w, _h, _offset.x, _offset.y);
    }

    void onResize(GameContext &, int w, int h)
    {
        _w = w;
        _h = h;
    }

    void unload(void) {}

    int updateOrder(void) const { return 0; }

    int renderOrder(void) const { return -200; }
};