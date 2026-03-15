#pragma once

#include "raylib.h"
#include <math.h>
#include <vector>
#include "LevelNode.hpp"
#include <ui/petitMenu/PetitMenu.hpp>
#include <ui/animation/PixelRevealTextureAnimation.hpp>
#include <core/Utils.hpp>

class TooltipSystem : public engine::ISystem
{
private:
    PetitMenu _tooltip;
    PixelRevealTextureAnimation _reveal;

    Sound _openSound{};
    Texture2D _bannerTexture{};

    std::vector<std::string> _lines;

    bool _visible = false;

public:
    void init(GameContext &) override
    {
        _lines.reserve(4);

        _bannerTexture = LoadTexture("../assets/ui/levelSelect/banner.png");
        _openSound = LoadSound("../assets/audio/banner_open.wav");

        _tooltip.setTextureScale(1.2f);
        _tooltip.setAnimation(_reveal);

        _tooltip.init(_bannerTexture, _openSound);
    }

    void update(GameContext &, float dt) override
    {
        _tooltip.update(dt);
    }

    void draw(GameContext &ctx) override
    {
        //_tooltip.draw(ctx.getWidth(), ctx.getHeight());
        _tooltip.draw(GetScreenWidth(), GetScreenHeight());
    }

    void unload(void) override
    {
        utils::safeUnload(_bannerTexture);
        UnloadSound(_openSound);
    }

    int renderOrder(void) const override
    {
        return 100;
    }

    void showLevel(const LevelNode &node, Vector2 pos, int w, int h)
    {
        std::string title = "Level " + std::to_string(node.id() + 1);

        _lines.clear();
        _lines.emplace_back(title);
        _lines.emplace_back("Difficulty: Easy");
        _lines.emplace_back("Reward: 200 gold");

        pos.y -= 60.f;

        if (!_tooltip.isVisible())
            _tooltip.show(pos, _lines, w, h);
        else
            _tooltip.setPosition(pos);

        _visible = true;
    }

    void hide(void)
    {
        if (_visible)
        {
            _tooltip.hide();
            _visible = false;
        }
    }
};