#pragma once

#include "../../../utils/StateManager.hpp"
#include "raylib.h"
#include "LevelState.hpp"
#include "LevelNode.hpp"

class GameState : public IGameState
{
private:
    Texture2D _mapTexture{};
    AnimatedSprite _lockAnim;

    std::vector<LevelNode> _levels;

    Vector2 _bgOffset{0, 0};

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _mapTexture = LoadTexture("../assets/ui/map.png");
        _lockAnim.loadSheet(ctx,"../assets/ui/flag/Graysprite.png", 6, 0.2f);

        _levels =
            {
                {0, {160, 120}},
                {1, {700, 650}},
                {2, {1100, 250}},
                {3, {1020, 740}},
                {4, {300, 700}}};

        for (auto &n : _levels)
            n.init(ctx);
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        Vector2 mouse = GetMousePosition();

        int unlocked = ctx.gethighestUnlockedLevel();
        ctx.updateMusic();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        float zoom = 1.05f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        float scaleX = drawW / (float)_mapTexture.width;
        float scaleY = drawH / (float)_mapTexture.height;

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 pos{
                node.position().x * scaleX + (w - drawW) * 0.5f + _bgOffset.x,
                node.position().y * scaleY + (h - drawH) * 0.5f + _bgOffset.y};

            if (node.update(dt, mouse, pos, unlockedNode))
                sm.changeState<LevelState>(node.id());
        }

        // effet parallaxe souris
        float nx = (mouse.x / w) - 0.5f;
        float ny = (mouse.y / h) - 0.5f;

        _bgOffset.x += (nx * 40.0f - _bgOffset.x) * 4.0f * dt;
        _bgOffset.y += (ny * 40.0f - _bgOffset.y) * 4.0f * dt;
        _lockAnim.update(dt);
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        float zoom = 1.05f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        float scaleX = drawW / (float)_mapTexture.width;
        float scaleY = drawH / (float)_mapTexture.height;

        ClearBackground(BLACK);

        Rectangle src{
            0,
            0,
            (float)_mapTexture.width,
            (float)_mapTexture.height};

        Rectangle dst{
            (w - drawW) * 0.5f + _bgOffset.x,
            (h - drawH) * 0.5f + _bgOffset.y,
            drawW,
            drawH};

        DrawTexturePro(_mapTexture, src, dst, {0, 0}, 0, WHITE);

        int unlocked = ctx.gethighestUnlockedLevel();

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 pos{
                node.position().x * scaleX + (w - drawW) * 0.5f + _bgOffset.x,
                node.position().y * scaleY + (h - drawH) * 0.5f + _bgOffset.y};

            node.draw(pos, _lockAnim, unlockedNode);
        }
    }

    void onExit(StateManager &) override
    {
        UnloadTexture(_mapTexture);
    }
};