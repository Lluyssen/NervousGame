#pragma once

#include "raylib.h"
#include <ui/AnimatedSprite.hpp>
#include <core/SystemManager.hpp>

// Gère le fond animé du menu avec un effet de parallaxe basé sur la souris.
class MenuBackgroundSystem
{
private:
    AnimatedSprite _background;
    Vector2 _offset{0.f, 0.f};
    bool _ready = false;

    int _w = 0;
    int _h = 0;

public:
    void loadFrame(GameContext &ctx, const std::string &basePath, int index)
    {
        _background.loadFrame(ctx, basePath, index);
    }

    void finalize()
    {
        _background.finalize(0.1f);
        _ready = true;
    }

    void update(float dt)
    {
        if (!_ready)
            return;

        _background.update(dt);

        Vector2 mouse = GetMousePosition();

        float nx = (mouse.x / _w) - 0.5f;
        float ny = (mouse.y / _h) - 0.5f;

        constexpr float strength = 30.0f;

        _offset.x += (nx * strength - _offset.x) * 5.0f * dt;
        _offset.y += (ny * strength - _offset.y) * 5.0f * dt;
    }

    void draw(int w, int h)
    {
        if (!_ready)
            return;

        _background.drawFullscreen(w, h, _offset.x, _offset.y);
    }

    void init(GameContext &) {}

    void update(GameContext &, float dt)
    {
        update(dt);
    }

    void draw(GameContext &)
    {
        draw(_w, _h);
    }

    void onResize(GameContext &, int w, int h)
    {
        _w = w;
        _h = h;
    }

    void unload(void) {}

    int updateOrder(void) const
    {
        return 0;
    }

    int renderOrder(void) const
    {
        return -200;
    }
};