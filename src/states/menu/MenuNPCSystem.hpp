#pragma once

#include "raylib.h"
#include <string>
#include <core/SystemManager.hpp>

// Gère l'affichage d'un PNJ avec texte après un certain temps d'inactivité.
class MenuNPCSystem
{
private:
    Texture2D _npc;

    float _idleTimer = 0.0f;
    bool _visible = false;

    std::string _text = "La Chine c'était mieux";

    int _h = 0;

public:
    void init(GameContext &)
    {
        _npc = LoadTexture("../assets/ui/guigui.png");
    }

    void update(float dt)
    {
        _idleTimer += dt;

        if (!_visible && _idleTimer > 15.0f)
            _visible = true;
    }

    void draw(int h)
    {
        if (!_visible)
            return;

        int px = 80;
        int py = h - _npc.height - 20;

        DrawTexture(_npc, px, py, WHITE);

        Rectangle bubble{(float)px + 80, (float)py - 80, 300, 70};

        DrawRectangleRounded(bubble, 0.4f, 8, Color{255, 255, 255, 230});
        DrawRectangleRoundedLinesEx(bubble, 0.4f, 8, 2, BLACK);
        DrawText(_text.c_str(), bubble.x + 10, bubble.y + 20, 20, BLACK);
    }

    void update(GameContext &, float dt)
    {
        update(dt);
    }

    void draw(GameContext &)
    {
        draw(_h);
    }

    void onResize(GameContext &, int, int h)
    {
        _h = h;
    }

    void unload(void)
    {
        UnloadTexture(_npc);
    }

    int updateOrder(void) const { return 0; }

    int renderOrder(void) const { return 50; }
};