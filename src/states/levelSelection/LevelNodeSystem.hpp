#pragma once

#include "raylib.h"
#include <math.h>
#include <vector>
#include "LevelNode.hpp"
#include <core/StateManager.hpp>
#include "TooltipSystem.hpp"
#include "AmbientParticleSystem.hpp"

class LevelNodeSystem
{
private:
    std::vector<LevelNode> _levels;
    std::vector<Vector2> _screenPos;

    AnimatedSprite _whiteIdle;
    AnimatedSprite _redIdle;

    Texture2D _ruinTexture{};
    Texture2D _haloTexture{};
    Texture2D _arrowTexture{};

    TooltipSystem *_tooltip = nullptr;
    AmbientParticleSystem *_particles = nullptr;
    StateManager *_stateManager = nullptr;

    float _pulseTimer = 0.f;
    float _arrowTimer = 0.f;
    float _uiScale = 1.f;

public:
    void setup(StateManager &sm, TooltipSystem &tooltip, AmbientParticleSystem &particles, const std::array<LevelNode, 5> &levels, float uiScale)
    {
        _stateManager = &sm;
        _tooltip = &tooltip;
        _particles = &particles;

        _uiScale = uiScale;

        _levels.assign(levels.begin(), levels.end());
        _screenPos.resize(_levels.size());
    }

    void init(GameContext &ctx)
    {
        _whiteIdle.setScale(.75f * _uiScale);
        _whiteIdle.loadAtlas(ctx, "../assets/ui/levelSelect/wIdle.png",
                             "../assets/ui/levelSelect/wIdle.json", 0.15f);

        _redIdle.setScale(.75f * _uiScale);
        _redIdle.loadAtlas(ctx, "../assets/ui/levelSelect/redIdle.png",
                           "../assets/ui/levelSelect/redIdle.json", 0.15f);

        _ruinTexture = LoadTexture("../assets/ui/levelSelect/ruineTexture.png");
        _haloTexture = LoadTexture("../assets/ui/levelSelect/halo.png");
        _arrowTexture = LoadTexture("../assets/ui/levelSelect/Arrow.png");
    }

    void unload(void)
    {
        utils::safeUnload(_ruinTexture);
        utils::safeUnload(_haloTexture);
        utils::safeUnload(_arrowTexture);
    }

    void update(GameContext &ctx, float dt)
    {
        Vector2 mouse = GetMousePosition();

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        int unlocked = ctx.getHighestUnlockedLevel();
        bool hoveredFound = false;

        for (size_t i = 0; i < _levels.size(); i++)
        {
            auto &node = _levels[i];
            Vector2 pos = _screenPos[i];

            bool playable = node.id() == unlocked;

            if (node.update(mouse, pos, playable))
            {
                ctx.setSelectedLevel(node.id());

                if (_stateManager)
                    _stateManager->changeState<MapState>();
            }

            if (node.isHovered())
            {
                if (_tooltip)
                    _tooltip->showLevel(node, pos, w, h);

                hoveredFound = true;
            }

            int id = node.id();

            if (_particles)
            {
                if (id == unlocked && GetRandomValue(0, 100) < 10)
                    _particles->spawnGold(pos, _uiScale);
                else if (id > unlocked && GetRandomValue(0, 100) < 6)
                    _particles->spawnRed(pos, _uiScale);
            }
        }

        if (!hoveredFound && _tooltip)
            _tooltip->hide();

        _pulseTimer += dt;
        _arrowTimer += dt;

        _whiteIdle.update(dt);
        _redIdle.update(dt);
    }

    void draw(GameContext &ctx)
    {
        int unlocked = ctx.getHighestUnlockedLevel();

        float pulse = 1.f + sinf(_pulseTimer * 3.f) * 0.08f;
        float scale = .75f * _uiScale;

        for (size_t i = 0; i < _levels.size(); i++)
        {
            Vector2 pos = _screenPos[i];

            if (_levels[i].id() == unlocked)
                drawActive(pos, pulse);
            else
                drawLocked(pos, scale);
        }
    }

    void onResize(GameContext &, int w, int h)
    {
        for (size_t i = 0; i < _levels.size(); i++)
            _screenPos[i] = _levels[i].getScreenPos(w, h);
    }

    int updateOrder(void) const { return 0; }
    int renderOrder(void) const { return 0; }

private:
    void drawActive(Vector2 pos, float pulse)
    {
        float haloScale = (1.2f + sinf(_pulseTimer * 2.f) * 0.1f) * _uiScale * 0.15f;

        float halfW = (_haloTexture.width * haloScale) * 0.5f;
        float halfH = (_haloTexture.height * haloScale) * 0.5f;

        DrawTextureEx(_haloTexture, {pos.x - halfW, pos.y - halfH}, 0, haloScale, Fade(GOLD, 0.7f));
        DrawTextureEx(_haloTexture, {pos.x - halfW * 1.4f, pos.y - halfH * 1.4f}, 0, haloScale * 1.4f, Fade(YELLOW, 0.25f));

        _whiteIdle.setScale(.75f * pulse * _uiScale);
        _whiteIdle.draw(pos);
        _whiteIdle.setScale(.75f * _uiScale);

        drawArrow(pos);
    }

    void drawLocked(Vector2 pos, float scale)
    {
        DrawTextureEx(_ruinTexture,
                      {pos.x - (_ruinTexture.width * scale) * 0.5f,
                       pos.y - (_ruinTexture.height * scale) * 0.5f - 50.f * _uiScale},
                      0, scale, WHITE);

        _redIdle.draw(pos);
    }

    void drawArrow(Vector2 pos)
    {
        if (!_arrowTexture.id)
            return;

        float bob = sinf(_arrowTimer * 3.f) * 12.f * _uiScale;
        float scale = 3.f * _uiScale;

        float x = pos.x - (_arrowTexture.width * scale) * 0.5f;
        float y = pos.y - 90.f * _uiScale + bob;

        Color c = Fade(YELLOW, 0.8f + sinf(_arrowTimer * 4.f) * 0.2f);

        DrawTextureEx(_arrowTexture, {x, y}, 0, scale, c);
    }
};