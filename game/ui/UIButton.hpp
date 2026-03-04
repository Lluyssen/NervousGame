#pragma once

#include "PixelRevealEffect.hpp"
#include <functional>

class UIButton
{
private:

    SDL_Rect rect{};
    SDL_Rect src{};
    SDL_Rect visualRect{};

    PixelRevealEffect reveal;

    bool hovered = false;
    bool revealFinished = false;

    float scale = 1.0f;

public:

    std::function<void()> onClick;

public:

    void setSprite(const SDL_Rect &s)
    {
        src = s;
    }

    void setRect(const SDL_Rect &r)
    {
        rect = r;

        float s = std::min(
            float(rect.w) / src.w,
            float(rect.h) / src.h);

        int drawW = std::round(src.w * s);
        int drawH = std::round(src.h * s);

        visualRect =
        {
            rect.x + (rect.w - drawW) / 2,
            rect.y + (rect.h - drawH) / 2,
            drawW,
            drawH
        };

        reveal.generate(src, rect);
    }

public:

    bool contains(int x,int y) const
    {
        return x >= rect.x &&
               x <= rect.x + rect.w &&
               y >= rect.y &&
               y <= rect.y + rect.h;
    }

    void setHovered(bool h)
    {
        hovered = h;
    }

    void click()
    {
        if(onClick)
            onClick();
    }

public:

    void render(SDL_Renderer* renderer, SDL_Texture* texture, float t)
    {
        float time = SDL_GetTicks() * 0.002f;

        float breathe = hovered ? 0.f : sin(time) * 0.03f;

        float targetScale = hovered ? 1.08f : 1.f;

        scale += (targetScale - scale) * 0.15f;

        float finalScale = scale + breathe;

        SDL_Rect scaled = visualRect;

        scaled.w *= finalScale;
        scaled.h *= finalScale;

        scaled.x -= (scaled.w - visualRect.w) / 2;
        scaled.y -= (scaled.h - visualRect.h) / 2;

        /*
            Shadow
        */

        int offset = hovered ? 6 : 4;
        int alpha  = hovered ? 110 : 80;

        SDL_Rect shadow = scaled;

        shadow.x += offset;
        shadow.y += offset;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, &shadow);

        /*
            Button
        */

        if (!revealFinished)
        {
            reveal.render(renderer, texture, t);

            if (t >= 1.f)
                revealFinished = true;
        }
        else
        {
            SDL_RenderCopy(renderer, texture, &src, &scaled);
        }

        /*
            Hover glow
        */

        if (hovered)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 40);
            SDL_RenderFillRect(renderer, &scaled);
        }
    }

public:

    void resetReveal()
    {
        reveal.generate(src, rect);
        revealFinished = false;
    }
};