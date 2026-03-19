#pragma once

#include <array>
#include "raylib.h"
#include <ui/AnimatedSprite.hpp>
#include <ui/animation/BulleAnimation.hpp>
#include <core/GameContext.hpp>
#include <core/SystemManager.hpp>

class LiquidDecorationSystem : public engine::System
{
private:
    std::array<AnimatedSprite, 4> _liquid;
    std::array<BubbleAnimation, 4> _bubbles;

    std::array<Vector2, 4> _pos{{{0.658854f, 0.918519f},
                                 {0.747396f, 0.791667f},
                                 {0.732292f, 0.600926f},
                                 {0.784375f, 0.728704f}}};

    std::array<Vector2, 4> _screenPos{};
    std::array<float, 4> _scale{1.f, .8f, .6f, .8f};

    float _uiScale = 1.f;

public:
    void setup(float uiScale)
    {
        _uiScale = uiScale;
    }

    void init(GameContext &ctx)
    {
        for (auto &b : _bubbles)
            b.reserve(128);

        for (size_t i = 0; i < _liquid.size(); i++)
        {
            _liquid[i].setScale(.75f * _scale[i] * _uiScale);
            _liquid[i].loadAtlas(ctx, "../assets/ui/levelSelect/liquidSprite.png", "../assets/ui/levelSelect/liquidSprite.json", 0.15f);
            _liquid[i].setPos(_pos[i]);
        }

        onResize(ctx, GetScreenWidth(), GetScreenHeight());
    }

    void unload(void)
    {
        for (auto &l : _liquid)
            l.unload();
    }

    void update(GameContext &, float dt)
    {
        for (auto &l : _liquid)
            l.update(dt);

        for (size_t i = 0; i < _bubbles.size(); i++)
        {
            auto &b = _bubbles[i];

            b.setPos(_screenPos[i]);

            if (GetRandomValue(0, 20) == 0)
                b.spawn();

            b.update(dt);
        }
    }

    void draw(GameContext &)
    {
        for (size_t i = 0; i < _liquid.size(); i++)
            _liquid[i].draw(_screenPos[i]);

        for (auto &b : _bubbles)
            b.draw(GREEN, LIME);
    }

    void onResize(GameContext &, int w, int h)
    {
        for (size_t i = 0; i < _pos.size(); i++)
            _screenPos[i] = utils::normalizedToScreen(_pos[i], w, h);
    }

    int updateOrder(void) const { return 0; }
    int renderOrder(void) const { return 15; }
};