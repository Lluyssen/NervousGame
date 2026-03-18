#pragma once

#include "raylib.h"
#include <ui/UIButton.hpp>
#include <vector>
#include <core/GameContext.hpp>
#include <ui/animation/ScaleHoverAnimation.hpp>
#include <ui/animation/PixelRevealAnimation.hpp>
#include <ui/PixelButton.hpp>
#include <core/SystemManager.hpp>

// Gère la création, l’animation et l’interaction des boutons du menu principal.
class MenuButtonsSystem
{
private:
    std::vector<std::unique_ptr<UIButton>> _buttons;
    int spacing = 150;

    int _lastAction = -1;

public:
    void init(GameContext &ctx)
    {
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        int bw = 320;
        int bh = 70;
        int startY = h / 2;

        _buttons.clear();
        _buttons.reserve(3);

        _buttons.push_back(std::make_unique<PixelButton>(
            "Game",
            Rectangle{(float)(w / 2 - bw / 2), (float)startY, (float)bw, (float)bh}));

        _buttons.push_back(std::make_unique<PixelButton>(
            "Settings",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + spacing), (float)bw, (float)bh}));

        _buttons.push_back(std::make_unique<PixelButton>(
            "Quit",
            Rectangle{(float)(w / 2 - bw / 2), (float)(startY + spacing * 2), (float)bw, (float)bh}));

        for (auto &b : _buttons)
        {
            b->setEnterAnimation(std::make_unique<PixelRevealAnimation>());
            b->setHoverAnimation(std::make_unique<ScaleHoverAnimation>());
        }
    }

    int update(float dt)
    {
        Vector2 mouse = GetMousePosition();
        int hovered = -1;

        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            if (CheckCollisionPointRec(mouse, _buttons[i]->baseRect()))
            {
                hovered = (int)i;
                break;
            }
        }

        for (size_t i = 0; i < _buttons.size(); ++i)
        {
            bool someoneHover = (hovered != -1 && hovered != (int)i);

            if (_buttons[i]->update(dt, someoneHover))
                return (int)i;
        }

        return -1;
    }

    void draw(void)
    {
        for (auto &b : _buttons)
            b->draw();
    }

    void update(GameContext &, float dt)
    {
        _lastAction = update(dt);
    }

    void draw(GameContext &)
    {
        draw();
    }

    void onResize(GameContext &, int, int)
    {
        // Optionnel recalculer les positions
    }

    void unload(void) {}

    int updateOrder(void) const { return 0; }

    int renderOrder(void) const { return 100; }

    void resetAnimations(void)
    {
        for (auto &b : _buttons)
            b->resetAnimations();
    }

    bool enterFinished(void)
    {
        for (auto &b : _buttons)
            if (!b->enterFinished())
                return false;

        return true;
    }

    int consumeAction(void)
    {
        int a = _lastAction;
        _lastAction = -1;
        return a;
    }

    UIButton &operator[](size_t i)
    {
        return *_buttons[i];
    }

    size_t size(void) const
    {
        return _buttons.size();
    }
};