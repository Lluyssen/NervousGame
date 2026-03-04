#pragma once
#include <SDL2/SDL.h>
#include <cmath>

class MenuTitle
{
private:
    SDL_Rect rect{};

    float baseScale = 1.0f;

public:
    void setPosition(int screenW)
    {
        int w = 400;
        int h = 120;

        rect = {
            screenW / 2 - w / 2,
            80,
            w,
            h};
    }
    void render(SDL_Renderer *renderer, SDL_Texture *texture)
    {
        float time = SDL_GetTicks() * 0.0015f;
        float breathe = sin(time) * 0.04f;
        float floatY = sin(time * 1.3f) * 6;

        SDL_Rect r = rect;

        r.w = rect.w * (1.0f + breathe);
        r.h = rect.h * (1.0f + breathe);

        r.x -= (r.w - rect.w) / 2;
        r.y -= (r.h - rect.h) / 2;

        r.y += floatY;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        float glowPulse = (sin(time * 2.f) + 1.f) * 0.5f;

        for (int i = 0; i < 3; i++)
        {
            SDL_Rect g = r;

            int expand = 6 + i * 4;

            g.x -= expand;
            g.y -= expand;
            g.w += expand * 2;
            g.h += expand * 2;

            Uint8 alpha = 40 + glowPulse * 40;

            SDL_SetTextureAlphaMod(texture, alpha / (i + 1));

            SDL_RenderCopy(renderer, texture, nullptr, &g);
        }

        SDL_SetTextureAlphaMod(texture, 255);

        SDL_RenderCopy(renderer, texture, nullptr, &r);
    }
};