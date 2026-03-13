#pragma once

#include "raylib.h"
#include <math.h>
#include <vector>
#include <ui/animation/Particles.hpp>

class AmbientParticleSystem : public engine::ISystem
{
private:
    static constexpr size_t MAX_PARTICLES = 256;

    std::array<Particle, MAX_PARTICLES> _goldParticles;
    std::array<Particle, MAX_PARTICLES> _redParticles;

    size_t _goldCount = 0;
    size_t _redCount = 0;

public:
    AmbientParticleSystem(void) = default;

    void init(GameContext &) override
    {
        _goldCount = 0;
        _redCount = 0;
    }

    void update(GameContext &, float dt) override
    {
        updateParticles(_goldParticles, _goldCount, dt);
        updateParticles(_redParticles, _redCount, dt);
    }

    void draw(GameContext &) override
    {
        drawParticles(_goldParticles, _goldCount, GOLD, YELLOW, WHITE);
        drawParticles(_redParticles, _redCount, MAROON, RED, ORANGE);
    }

    int renderOrder(void) const override
    {
        return 20;
    }

    void spawnGold(Vector2 pos, float scale)
    {
        spawnParticle(pos, _goldParticles, _goldCount, scale);
    }

    void spawnRed(Vector2 pos, float scale)
    {
        spawnParticle(pos, _redParticles, _redCount, scale);
    }

private:
    void spawnParticle(Vector2 pos, std::array<Particle, MAX_PARTICLES> &container, size_t &count, float scale)
    {
        if (count >= MAX_PARTICLES)
            return;

        Particle &p = container[count++];
        p.pos = pos;
        p.vel = {GetRandomValue(-20, 20) * scale, GetRandomValue(-40, -10) * scale};
        p.life = 0.f;
        p.maxLife = GetRandomValue(1, 3);
    }

    void updateParticles(std::array<Particle, MAX_PARTICLES> &particles, size_t &count, float dt)
    {
        for (size_t i = 0; i < count;)
        {
            Particle &p = particles[i];

            p.pos.x += p.vel.x * dt;
            p.pos.y += p.vel.y * dt;
            p.life += dt;

            if (p.life >= p.maxLife)
            {
                particles[i] = particles[count - 1];
                count--;
                continue;
            }

            i++;
        }
    }

    void drawParticles(std::array<Particle, MAX_PARTICLES> &particles, size_t count, Color c1, Color c2, Color c3)
    {
        for (size_t i = 0; i < count; i++)
        {
            Particle &p = particles[i];

            float t = p.life / p.maxLife;

            Color color;

            if (t < 0.33f)
                color = c1;
            else if (t < 0.66f)
                color = c2;
            else
                color = c3;

            DrawCircleV(p.pos, 2.f, Fade(color, 1.f - t));
        }
    }
};