#pragma once

#include <core/StateManager.hpp>
#include "raylib.h"
#include <states/game/LevelNode.hpp>
#include <ui/petitMenu/PetitMenu.hpp>
#include <states/PauseMenuState.hpp>
#include <states/MapState.hpp>
#include <ui/animation/PixelRevealTextureAnimation.hpp>
#include <ui/animation/Particles.hpp>

class LevelSelectionState : public IGameState
{
private:
    std::vector<Texture2D> _maps;

    Texture2D _ruinTexture{};
    Texture2D _bannerTexture{};
    Texture2D _fogTexture{};
    Texture2D _haloTexture{};

    AnimatedSprite _whiteIdle;
    AnimatedSprite _redIdle;

    std::vector<LevelNode> _levels;

    PetitMenu _tooltip;
    Sound _tooltipOpenSound{};

    PixelRevealTextureAnimation reveal;

    std::vector<std::string> _tooltipLines;

    float _pulseTimer = 0.f;
    float _uiScale = 1.f;

    float _fogOffsetX = 0.f;
    float _fogOffsetY = 0.f;

    std::vector<Particle> _particles;
    std::vector<Particle> _redParticles;

    static constexpr float REF_WIDTH = 1920.f;
    static constexpr float REF_HEIGHT = 1080.f;

private:
    // UTILITIES
    void safeUnload(Texture2D &tex)
    {
        if (tex.id != 0)
        {
            UnloadTexture(tex);
            tex.id = 0;
        }
    }

    // FOG
    void updateFog(float dt)
    {
        _fogOffsetX += dt * 10.f;
        _fogOffsetY += dt * 4.f;
    }

    void drawFog(int w, int h)
    {
        if (_fogTexture.id == 0)
            return;

        float scale = 2.f;

        float wTex = _fogTexture.width * scale;
        float hTex = _fogTexture.height * scale;

        for (float x = -wTex; x < w + wTex; x += wTex)
        {
            for (float y = -hTex; y < h + hTex; y += hTex)
                DrawTextureEx(_fogTexture, {x + fmod(_fogOffsetX, wTex), y + fmod(_fogOffsetY, hTex)}, 0, scale, Fade(WHITE, 0.35f));
        }
    }

    // BackGround
    void drawMap(Texture2D &map, int w, int h)
    {
        if (map.id == 0)
            return;

        Rectangle src{0, 0, (float)map.width, (float)map.height};
        Rectangle dst{0, 0, (float)w, (float)h};

        DrawTexturePro(map, src, dst, {0, 0}, 0, WHITE);
    }

    // NODE
    void drawActiveNode(Vector2 pos, float pulse)
    {
        float haloScale = (1.2f + sinf(_pulseTimer * 2.f) * 0.1f) * _uiScale * 0.15;

        DrawTextureEx(_haloTexture, {pos.x - (_haloTexture.width * haloScale) * 0.5f, pos.y - (_haloTexture.height * haloScale) * 0.5f}, 0, haloScale, Fade(GOLD, 0.7f));
        DrawTextureEx(_haloTexture, {pos.x - (_haloTexture.width * haloScale * 1.4f) * 0.5f, pos.y - (_haloTexture.height * haloScale * 1.4f) * 0.5f}, 0, haloScale * 1.4f, Fade(YELLOW, 0.25f));

        _whiteIdle.setScale(.75f * pulse * _uiScale);
        _whiteIdle.draw(pos);
        _whiteIdle.setScale(.75f * _uiScale);
    }

    void drawLockedNode(Vector2 pos)
    {
        float scale = 0.75f * _uiScale;
        DrawTextureEx(_ruinTexture, {pos.x - (_ruinTexture.width * scale) * 0.5f, pos.y - (_ruinTexture.height * scale) * 0.5f - 50.f * _uiScale}, 0, scale, WHITE);
        _redIdle.draw(pos);
    }

    void drawNodes(int w, int h, int unlocked)
    {
        float pulse = 1.f + sinf(_pulseTimer * 3.f) * 0.08f;

        for (auto &node : _levels)
        {
            Vector2 pos = node.getScreenPos(w, h);

            if (node.id() == unlocked)
                drawActiveNode(pos, pulse);
            else
                drawLockedNode(pos);
        }
    }

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        float scaleX = ctx.getWidth() / REF_WIDTH;
        float scaleY = ctx.getHeight() / REF_HEIGHT;

        _uiScale = std::min(scaleX, scaleY);

        _particles.clear();
        _redParticles.clear();

        _tooltipLines.reserve(4);

        _tooltipOpenSound = LoadSound("../assets/audio/banner_open.wav");

        _tooltip.setTextureScale(1.2f);
        _tooltip.setAnimation(&reveal);

        for (auto &t : _maps)
            safeUnload(t);

        _maps.clear();

        _maps.push_back(LoadTexture("../assets/ui/levelSelect/map/map.png"));

        for (int i = 1; i < 6; i++)
            _maps.push_back(LoadTexture(TextFormat("../assets/ui/levelSelect/map/map%d.png", i)));

        _whiteIdle.setScale(.75f * _uiScale);
        _whiteIdle.loadAtlas(ctx, "../assets/ui/levelSelect/wIdle.png", "../assets/ui/levelSelect/wIdle.json", 0.15f);

        _redIdle.setScale(.75f * _uiScale);
        _redIdle.loadAtlas(ctx, "../assets/ui/levelSelect/redIdle.png", "../assets/ui/levelSelect/redIdle.json", 0.15f);

        safeUnload(_bannerTexture);

        _ruinTexture = LoadTexture("../assets/ui/levelSelect/ruineTexture.png");

        _bannerTexture = LoadTexture("../assets/ui/levelSelect/banner.png");

        _fogTexture = LoadTexture("../assets/ui/levelSelect/fog.png");

        _haloTexture = LoadTexture("../assets/ui/levelSelect/halo.png");

        _tooltip.init(_bannerTexture, _tooltipOpenSound);

        _levels = {
            {0, {0.139f, 0.215f}},
            {1, {0.173f, 0.720f}},
            {2, {0.467f, 0.610f}},
            {3, {0.696f, 0.230f}},
            {4, {0.671f, 0.730f}}};
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        Vector2 mouse = GetMousePosition();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int unlocked = ctx.getHighestUnlockedLevel();

        bool changeStateRequested = false;

        LevelNode *hovered = nullptr;

        for (auto &node : _levels)
        {
            Vector2 pos = node.getScreenPos(w, h);

            bool playable = node.id() == unlocked;

            if (node.update(mouse, pos, playable))
            {
                ctx.setSelectedLevel(node.id());
                changeStateRequested = true;
            }

            if (node.isHovered())
                hovered = &node;

            if (node.id() == unlocked)
            {
                if (GetRandomValue(0, 100) < 10)
                    spawnParticle(pos, _particles, _uiScale);
            }
            else if (node.id() > unlocked)
            {
                if (GetRandomValue(0, 100) < 6)
                    spawnParticle(pos, _redParticles, _uiScale);
            }
        }

        if (changeStateRequested)
        {
            sm.changeState<MapState>();
            return;
        }

        // Tooltips
        if (hovered)
        {
            _tooltipLines.clear();
            _tooltipLines.push_back(TextFormat("Level %d", hovered->id() + 1));
            _tooltipLines.emplace_back("Difficulty: Easy");
            _tooltipLines.emplace_back("Reward: 200 gold");

            Vector2 pos = hovered->getScreenPos(w, h);
            pos.y -= 60.f;

            if (!_tooltip.isVisible())
                _tooltip.show(pos, _tooltipLines, w, h);
            else
                _tooltip.setPosition(pos);
        }
        else
            _tooltip.hide();

        // Particles
        updateParticles(_particles, dt);
        updateParticles(_redParticles, dt);

        updateFog(dt);

        _pulseTimer += dt;

        _whiteIdle.update(dt);
        _redIdle.update(dt);
        _tooltip.update(dt);

        ctx.updateMusic();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int unlocked = ctx.getHighestUnlockedLevel();

        if (_maps.empty())
            return;

        int index = std::min(unlocked, (int)_maps.size() - 1);

        Texture2D &map = _maps[index];

        ClearBackground(BLACK);

        drawMap(map, w, h);

        drawFog(w, h);

        drawNodes(w, h, unlocked);

        drawParticles(_particles, GOLD, YELLOW, WHITE);
        drawParticles(_redParticles, MAROON, RED, ORANGE);

        _tooltip.draw(w, h);
    }

    void onExit(StateManager &) override
    {
        for (auto &t : _maps)
            safeUnload(t);

        safeUnload(_bannerTexture);
        safeUnload(_ruinTexture);
        safeUnload(_fogTexture);
        safeUnload(_haloTexture);
        UnloadSound(_tooltipOpenSound);
    }
};