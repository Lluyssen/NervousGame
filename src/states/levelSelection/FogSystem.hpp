#pragma once

#include "raylib.h"
#include <math.h>
#include <core/SystemManager.hpp>

class FogSystem : public engine::System
{
private:
    Texture2D texture{};
    const char *_path = nullptr;

    float offsetX = 0.f;
    float offsetY = 0.f;

    float texW = 0.f;
    float texH = 0.f;

    static constexpr float SPEED_X = 10.f;
    static constexpr float SPEED_Y = 4.f;
    static constexpr float SCALE = 2.f;

public:
    void setTexture(const char *path)
    {
        _path = path;
    }

    void init(GameContext &)
    {
        if (!_path)
            return;

        texture = LoadTexture(_path);

        texW = texture.width * SCALE;
        texH = texture.height * SCALE;
    }

    void unload(void)
    {
        if (texture.id)
        {
            UnloadTexture(texture);
            texture = {};
        }
    }

    void update(GameContext &, float dt)
    {
        offsetX += dt * SPEED_X;
        offsetY += dt * SPEED_Y;

        offsetX = fmod(offsetX, texW);
        offsetY = fmod(offsetY, texH);
    }

    void draw(GameContext &)
    {
        if (!texture.id)
            return;

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        for (float x = -texW; x < w; x += texW)
        {
            for (float y = -texH; y < h; y += texH)
                DrawTextureEx(texture, {x + offsetX, y + offsetY}, 0.f, SCALE, Fade(WHITE, 0.35f));
        }
    }

    void onResize(GameContext &, int, int) {}
};