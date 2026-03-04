#pragma once
#include <SDL2/SDL.h>
#include <vector>

class AnimatedBackground
{
private:
    struct Layer
    {
        std::vector<SDL_Texture *> frames;
        int frame = 0;
        float speed = 0.3f;
        float strength = 1.f;
    };

    std::vector<Layer> layers;

    float timer = 0.f;

public:
    void addLayer(float strength)
    {
        layers.push_back({});
        layers.back().strength = strength;
    }

    void addFrame(int layer, SDL_Texture *tex)
    {
        if (layer >= (int)layers.size())
            return;
        layers[layer].frames.push_back(tex);
    }

    void update(float dt)
    {
        timer += dt;

        for (auto &l : layers)
        {
            if (l.frames.empty())
                continue;

            if (timer >= l.speed)
            {
                l.frame = (l.frame + 1) % l.frames.size();
            }
        }

        if (!layers.empty() && timer >= layers[0].speed)
            timer = 0.f;
    }

    void render(SDL_Renderer *renderer, int w, int h, float px, float py)
    {
        for (auto &l : layers)
        {
            if (l.frames.empty())
                continue;

            SDL_Rect dst{
                (int)(-px * l.strength),
                (int)(-py * l.strength),
                w + 80,
                h + 80};

            SDL_RenderCopy(renderer, l.frames[l.frame], nullptr, &dst);
        }
    }

    void reset()
    {
        timer = 0.f;

        for (auto &l : layers)
            l.frame = 0;
    }
};