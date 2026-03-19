#pragma once

#include "raylib.h"
#include <math.h>
#include <vector>
#include "LevelNode.hpp"
#include <ui/petitMenu/PetitMenu.hpp>
#include <ui/animation/PixelRevealTextureAnimation.hpp>
#include <core/Utils.hpp>
#include <core/SystemManager.hpp>

class TooltipSystem : public engine::System
{
private:
    PetitMenu _tooltip;
    PixelRevealTextureAnimation _reveal;

    Sound _openSound{};
    Texture2D _bannerTexture{};

    std::vector<std::string> _lines;

    bool _visible = false;

    void buildLevelText(const LevelNode &node)
    {
        _lines.clear();

        _lines.emplace_back("Level " + std::to_string(node.id() + 1));
        _lines.emplace_back("Difficulty: Easy");
        _lines.emplace_back("Reward: 200 gold");
    }

public:
    void init(GameContext &)
    {
        _lines.reserve(4);

        _bannerTexture = LoadTexture("../assets/ui/levelSelect/banner.png");
        _openSound = LoadSound("../assets/audio/banner_open.wav");

        _tooltip.setTextureScale(1.2f);
        _tooltip.setAnimation(_reveal);

        _tooltip.init(_bannerTexture, _openSound);
    }

    void unload(void)
    {
        utils::safeUnload(_bannerTexture);
        UnloadSound(_openSound);
    }

    void update(GameContext &, float dt)
    {
        _tooltip.update(dt);
    }

    void draw(GameContext &)
    {
        _tooltip.draw(GetScreenWidth(), GetScreenHeight());
    }

    void onResize(GameContext &, int, int) {}

    int updateOrder(void) const { return 0; }
    int renderOrder(void) const { return 100; }

    void showLevel(const LevelNode &node, Vector2 pos, int w, int h)
    {
        buildLevelText(node);

        pos.y -= 60.f;

        if (!_tooltip.isVisible())
            _tooltip.show(pos, _lines, w, h);
        else
            _tooltip.setPosition(pos);

        _visible = true;
    }

    void hide(void)
    {
        if (!_visible)
            return;

        _tooltip.hide();
        _visible = false;
    }
};