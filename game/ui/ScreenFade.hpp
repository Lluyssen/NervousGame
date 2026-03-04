#pragma once
#include <SDL2/SDL.h>

class ScreenFade
{
private:

    float alpha = 0;
    float duration = 0.5f;
    bool active = false;

public:

    void start()
    {
        alpha = 0;
        active = true;
    }

    bool finished() const
    {
        return alpha >= 255.f;
    }

    bool isActive() const
    {
        return active;
    }

    void update(float dt)
    {
        if(!active) return;

        alpha += dt * 255.f / duration;

        if(alpha > 255)
            alpha = 255;
    }

    void render(SDL_Renderer* renderer, int w, int h)
    {
        if(!active) return;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer,0,0,0,(Uint8)alpha);

        SDL_Rect r{0,0,w,h};
        SDL_RenderFillRect(renderer,&r);
    }
};