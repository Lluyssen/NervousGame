#pragma once

#include <array>
#include "raylib.h"
#include <core/Utils.hpp>


class MapRenderer
{
private:
    std::array<Texture2D, 6> _maps{};

public:
    MapRenderer(void) = default;

    void load(void)
    {
        _maps[0] = LoadTexture("../assets/ui/levelSelect/map/map.png");

        for (int i = 1; i < 6; i++)
            _maps[i] = LoadTexture(TextFormat("../assets/ui/levelSelect/map/map%d.png", i));
    }

    void draw(int unlockedLevel, int w, int h)
    {
        int index = std::clamp(unlockedLevel, 0, (int)_maps.size() - 1);

        Texture2D &map = _maps[index];

        if (map.id == 0)
            return;

        Rectangle src{0, 0, (float)map.width, (float)map.height};
        Rectangle dst{0, 0, (float)w, (float)h};

        DrawTexturePro(map, src, dst, {0, 0}, 0.f, WHITE);
    }

    void unload(void)
    {
        for (auto &t : _maps)
            utils::safeUnload(t);
    }
};