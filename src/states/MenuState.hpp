#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/Starfield.hpp"
#include "menu/MenuBackground.hpp"
#include "menu/MenuButtons.hpp"
#include "menu/MenuNPC.hpp"
#include "menu/MenuTitle.hpp"
#include "menu/MenuMusic.hpp"

// État du menu principal : orchestre fond, étoiles, UI, PNJ et musique.
#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/Starfield.hpp"
#include "menu/MenuBackground.hpp"
#include "menu/MenuButtons.hpp"
#include "menu/MenuNPC.hpp"
#include "menu/MenuTitle.hpp"
#include "menu/MenuMusic.hpp"

// État du menu principal : fond animé, étoiles, UI, PNJ et musique.
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
    float _timer = 0;

    // Sous-composants du menu
    MenuBackground _background;
    MenuButtons _buttons;
    MenuTitle _title;
    MenuNPC _npc;
    MenuMusic _music;

    // ECS étoiles
    Registry<StarComponents> _starRegistry;
    std::unique_ptr<StarfieldSystem> _farStars;
    std::unique_ptr<StarfieldSystem> _midStars;
    std::unique_ptr<StarfieldSystem> _nearStars;

    // Loading screen
    Texture2D _loadingTexture{};
    bool _loading = true;

    int _bgFramesLoaded = 0;
    const int _bgTotalFrames = 41;

public:
    MenuState() = default;

    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _loadingTexture = LoadTexture("../assets/ui/loading.png");

        _loading = true;
        _bgFramesLoaded = 0;

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        _buttons.init(ctx);
        _npc.init();
        _music.load();

        // ECS étoiles
        _starRegistry = Registry<StarComponents>();

        _farStars = std::make_unique<StarfieldSystem>(w, h);
        _midStars = std::make_unique<StarfieldSystem>(w, h);
        _nearStars = std::make_unique<StarfieldSystem>(w, h);

        spawnStarfield(_starRegistry, 80, w, h, 1.f, 2.f);
        spawnStarfield(_starRegistry, 40, w, h, 2.f, 3.f);
        spawnStarfield(_starRegistry, 20, w, h, 3.f, 5.f);

        _phase = Phase::Loading;
        _timer = 0;
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        // Chargement progressif du background
        if (_loading)
        {
            if (_bgFramesLoaded < _bgTotalFrames)
            {
                _background.loadFrame(ctx, "../assets/ui/bg/frame", _bgFramesLoaded);
                _bgFramesLoaded++;
            }
            else
            {
                _background.finalize();
                _loading = false;
            }

            return;
        }

        dt = std::min(dt, 0.05f);

        _background.update(dt);
        _music.update();
        _npc.update(dt);

        _farStars->update(dt, _starRegistry);
        _midStars->update(dt, _starRegistry);
        _nearStars->update(dt, _starRegistry);

        switch (_phase)
        {
        case Phase::Loading:

            _timer += dt;

            if (_timer > 0.2f)
            {
                _buttons.resetAnimations();
                _phase = Phase::UIReveal;
            }

            break;

        case Phase::UIReveal:

            _buttons.update(dt);

            if (_buttons.enterFinished())
                _phase = Phase::Idle;

            break;

        case Phase::Idle:
        {
            int action = _buttons.update(dt);

            if (action >= 0)
                activate(sm, action);
        }
        break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Loading screen
        if (_loading)
        {
            ClearBackground(BLACK);

            Rectangle src{
                0,
                0,
                (float)_loadingTexture.width,
                (float)_loadingTexture.height};

            Rectangle dst{
                0,
                0,
                (float)w,
                (float)h};

            DrawTexturePro(
                _loadingTexture,
                src,
                dst,
                {0, 0},
                0,
                WHITE);

            float progress = (float)_bgFramesLoaded / (float)_bgTotalFrames;
            progress = std::clamp(progress, 0.0f, 1.0f);

            int barWidth = 400;
            int barHeight = 20;

            int x = w / 2 - barWidth / 2;
            int y = h / 2 + 240;

            DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);
            DrawRectangle(x, y, barWidth * progress, barHeight, GOLD);
            DrawRectangleLines(x, y, barWidth, barHeight, WHITE);

            DrawText(
                TextFormat("Loading %d%%", (int)(progress * 100)),
                x + barWidth / 2 - 50,
                y + 30,
                20,
                WHITE);

            return;
        }

        _background.draw(w, h);

        _farStars->draw(_starRegistry);
        _midStars->draw(_starRegistry);
        _nearStars->draw(_starRegistry);

        _title.draw(w, h);

        if (_phase == Phase::UIReveal || _phase == Phase::Idle)
            _buttons.draw();

        _npc.draw(h);
    }

    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.changeState<GameState>();
            break;

        case 2:
            CloseWindow();
            break;
        }
    }

    void onExit(StateManager &) override
    {
        _music.unload();
        UnloadTexture(_loadingTexture);
    }
};