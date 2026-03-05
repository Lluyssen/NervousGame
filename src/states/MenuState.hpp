#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/AnimatedSprite.hpp"
#include "../ui/UIButton.hpp"
#include "../ui/animation/ScaleHoverAnimation.hpp"
#include <vector>
#include <string>

class GameState;

class MenuState : public IGameState
{
private:
    AnimatedSprite _background;

    std::vector<UIButton> _buttons;

    float _uiDelay = 1.0f; // temps avant apparition UI
    float _timer = 0.0f;
    bool _uiActive = false;

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        // reset UI timing
        _timer = 0.0f;
        _uiActive = false;

        // fond animé
        _background.load(ctx, "../assets/ui/bg/frame", 41, 0.1f);

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        int bw = 320;
        int bh = 70;

        int startY = h / 2;

        _buttons.clear();

        _buttons.emplace_back(
            "Game",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY), (float)bw, (float)bh});

        _buttons.emplace_back(
            "Settings",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 90), (float)bw, (float)bh});

        _buttons.emplace_back(
            "Quit",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + 180), (float)bw, (float)bh});

        for (auto &b : _buttons)
        {
            b.setAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    void update(StateManager &sm, float dt) override
    {
        _background.update(dt);

        _timer += dt;

        if (!_uiActive && _timer >= _uiDelay)
            _uiActive = true;

        if (!_uiActive)
            return;

        for (unsigned int i = 0; i < _buttons.size(); i++)
        {
            if (_buttons[i].update(dt))
                activate(sm, i);
        }

        if (IsKeyPressed(KEY_ESCAPE))
            CloseWindow();
    }

    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.changeState<GameState>();
            break;

        case 1:
            // futur SettingsState
            break;

        case 2:
            CloseWindow();
            break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        _background.drawFullscreen(w, h);

        const char *title = "TOWER DEFENSE";

        int ts = 70;
        int tw = MeasureText(title, ts);

        DrawText(title, (w - tw) / 2, h / 4, ts, WHITE);

        if (_uiActive)
        {
            for (auto &b : _buttons)
                b.draw();
        }
    }
};