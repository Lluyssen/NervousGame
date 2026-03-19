#pragma once

#include "raylib.h"
#include <core/SystemManager.hpp>

// Affiche le titre du menu avec un effet de glow et d’ombre.
class MenuTitleSystem : public engine::System
{
private:
    int _w = 0;
    int _h = 0;

public:
    void draw(int w, int h)
    {
        const char *title = "NERVOUS GAME";
        int ts = 80;
        int tw = MeasureText(title, ts);
        int x = (w - tw) / 2;
        int y = h / 4;

        // glow
        for (int i = 8; i > 0; i--)
            DrawText(title, x, y, ts, Color{255, 255, 255, (unsigned char)(10 * i)});

        // shadow
        DrawText(title, x + 6, y + 6, ts, Color{0, 0, 0, 150});

        // main text
        DrawText(title, x, y, ts, Color{255, 220, 120, 255});
    }

    void init(GameContext &) {}

    void update(GameContext &, float) {}

    void draw(GameContext &)
    {
        draw(_w, _h);
    }

    void onResize(GameContext &, int w, int h)
    {
        _w = w;
        _h = h;
    }

    void unload(void) {}

    int updateOrder(void) const { return 0; }

    int renderOrder(void) const { return 10; }
};
