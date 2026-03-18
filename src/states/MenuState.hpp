#pragma once

#include "raylib.h"
#include <ui/Starfield.hpp>
#include <states/LevelSelectionState.hpp>
#include <states/menu/MenuBackgroundSystem.hpp>
#include <states/menu/MenuButtonsSystem.hpp>
#include <states/menu/MenuNPCSystem.hpp>
#include <states/menu/MenuTitleSystem.hpp>
#include <core/MusicManager.hpp>
#include <core/StateManager.hpp>

// État du menu principal : gère fond animé, étoiles, UI, PNJ et musique.
class MenuState : public IGameState
{
private:
    enum class Phase
    {
        Loading,
        UIReveal,
        Idle
    };

    Phase _phase = Phase::Loading;
    float _timer = 0.f;

    engine::SystemManager _systems;

    MenuBackgroundSystem _background;
    MenuButtonsSystem _buttons;
    MenuTitleSystem _title;
    MenuNPCSystem _npc;

    Registry<StarComponents> _starRegistry;
    std::unique_ptr<StarfieldSystem> _farStars;
    std::unique_ptr<StarfieldSystem> _midStars;
    std::unique_ptr<StarfieldSystem> _nearStars;

    Texture2D *_loadingTexture = nullptr;
    int _bgFramesLoaded = 0;

    static constexpr int BG_TOTAL_FRAMES = 41;

    enum class ButtonID
    {
        Play = 0,
        Options = 1,
        Quit = 2
    };

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _systems.clear();

        _loadingTexture = &ctx.loadTexture("../assets/ui/loading.png");
        _bgFramesLoaded = 0;

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        _background.onResize(ctx, w, h);
        _npc.onResize(ctx, w, h);
        _title.onResize(ctx, w, h);

        ctx.initMusic("../assets/audio/menu_music.mp3");

        _starRegistry = Registry<StarComponents>();

        _farStars = std::make_unique<StarfieldSystem>(w, h);
        _midStars = std::make_unique<StarfieldSystem>(w, h);
        _nearStars = std::make_unique<StarfieldSystem>(w, h);

        _farStars->setRegistry(_starRegistry);
        _midStars->setRegistry(_starRegistry);
        _nearStars->setRegistry(_starRegistry);

        spawnStarfield(_starRegistry, 80, w, h, 1.f, 2.f);
        spawnStarfield(_starRegistry, 40, w, h, 2.f, 3.f);
        spawnStarfield(_starRegistry, 20, w, h, 3.f, 5.f);

        _systems.add(_background);
        _systems.add(_buttons);
        _systems.add(_npc);
        _systems.add(_title);
        _systems.add(*_farStars);
        _systems.add(*_midStars);
        _systems.add(*_nearStars);

        _systems.init(ctx);

        _phase = Phase::Loading;
        _timer = 0.f;
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();
        dt = std::min(dt, 0.05f);

        if (_bgFramesLoaded < BG_TOTAL_FRAMES)
        {
            for (int i = 0; i < 2 && _bgFramesLoaded < BG_TOTAL_FRAMES; ++i)
                _background.loadFrame(ctx, "../assets/ui/bg/frame", _bgFramesLoaded++);

            if (_bgFramesLoaded == BG_TOTAL_FRAMES)
                _background.finalize();
            return;
        }

        ctx.updateMusic();
        _systems.update(ctx, dt);

        switch (_phase)
        {
        case Phase::Loading:
            _timer += dt;
            if (_timer > 0.2f)
                _phase = Phase::UIReveal;
            break;

        case Phase::UIReveal:
            if (_buttons.enterFinished())
                _phase = Phase::Idle;
            break;

        case Phase::Idle:
        {
            int action = _buttons.consumeAction();
            if (action >= 0)
                activate(sm, static_cast<ButtonID>(action));
        }
        break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        if (_bgFramesLoaded < BG_TOTAL_FRAMES && _loadingTexture)
        {
            ClearBackground(BLACK);

            Rectangle src{0.f, 0.f, (float)_loadingTexture->width, (float)_loadingTexture->height};
            Rectangle dst{0.f, 0.f, (float)w, (float)h};
            DrawTexturePro(*_loadingTexture, src, dst, {0, 0}, 0.f, WHITE);

            float progress = std::clamp((float)_bgFramesLoaded / BG_TOTAL_FRAMES, 0.f, 1.f);

            int barWidth = (int)(w * 0.4f);
            int barHeight = (int)(h * 0.03f);
            int x = w / 2 - barWidth / 2;
            int y = (int)(h * 0.85f);

            DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);
            DrawRectangle(x, y, (int)(barWidth * progress), barHeight, GOLD);
            DrawRectangleLines(x, y, barWidth, barHeight, WHITE);

            const int percent = (int)(progress * 100);
            const std::string text = TextFormat("Loading %d%%", percent);
            int textWidth = MeasureText(text.c_str(), 20);

            DrawText(text.c_str(), x + barWidth / 2 - textWidth / 2, y + barHeight + 10, 20, WHITE);

            _background.draw(ctx);
            return;
        }
        _systems.draw(ctx);
    }

    void activate(StateManager &sm, ButtonID id)
    {
        switch (id)
        {
        case ButtonID::Play:
            sm.changeState<LevelSelectionState>();
            break;

        case ButtonID::Quit:
            exit(0);
            break;

        default:
            break;
        }
    }

    void onExit(StateManager &) override
    {
        _systems.unload();
    }
};