#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <cstdlib>

class AmbientParticles
{
private:
    enum class ParticleType
    {
        Dust,
        Pollen,
        Firefly
    };

    struct Particle
    {
        float x;
        float y;

        float speed;
        float drift;

        float size;
        float alpha;

        ParticleType type;
    };

    std::vector<Particle> particles;

public:
    void init(int count, int w, int h)
    {
        particles.clear();

        for (int i = 0; i < count; i++)
        {
            Particle p;

            p.x = rand() % w;
            p.y = rand() % h;

            int t = rand() % 3;

            if (t == 0)
            {
                p.type = ParticleType::Dust;
                p.speed = 10;
                p.size = 2;
                p.alpha = 60;
            }
            else if (t == 1)
            {
                p.type = ParticleType::Pollen;
                p.speed = 5;
                p.size = 3;
                p.alpha = 120;
            }
            else
            {
                p.type = ParticleType::Firefly;
                p.speed = 3;
                p.size = 4;
                p.alpha = 200;
            }

            p.drift = ((rand() % 200) - 100) / 100.f;

            particles.push_back(p);
        }
    }

    void update(float dt, int w, int h)
    {
        for (auto &p : particles)
        {
            p.y -= p.speed * dt;

            p.x += p.drift * dt * 20;

            if (p.y < -10)
            {
                p.y = h + 10;
                p.x = rand() % w;
            }
        }
    }

    void render(SDL_Renderer *renderer)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (auto &p : particles)
        {
            if (p.type == ParticleType::Dust)
                SDL_SetRenderDrawColor(renderer, 200, 200, 255, p.alpha);

            if (p.type == ParticleType::Pollen)
                SDL_SetRenderDrawColor(renderer, 255, 220, 120, p.alpha);

            if (p.type == ParticleType::Firefly)
                SDL_SetRenderDrawColor(renderer, 255, 255, 160, p.alpha);

            SDL_Rect r{
                (int)p.x,
                (int)p.y,
                (int)p.size,
                (int)p.size};

            SDL_RenderFillRect(renderer, &r);
        }
    }
};