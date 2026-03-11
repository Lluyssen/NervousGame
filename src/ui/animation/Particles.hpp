#pragma once

#include "raylib.h"
#include <vector>
#include <math.h>

struct Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
};

static constexpr int MAX_PARTICLES = 60;

// PARTICLES
void spawnParticle(Vector2 center, std::vector<Particle> &container, float uiScale)
{
    if (container.size() >= MAX_PARTICLES)
        return;

    float angle = GetRandomValue(0, 360) * DEG2RAD;
    float speed = GetRandomValue(10, 40);
    float radius = 30.f * uiScale;

    Particle p;

    p.pos = {center.x + cosf(angle) * radius, center.y + sinf(angle) * radius};
    p.vel = {cosf(angle) * speed, sinf(angle) * speed};

    p.life = 0.f;
    p.maxLife = GetRandomValue(800, 1400) / 1000.f;

    container.push_back(p);
}

void updateParticles(std::vector<Particle> &particles, float dt)
{
    for (auto it = particles.begin(); it != particles.end();)
    {
        it->life += dt;

        it->pos.x += it->vel.x * dt;
        it->pos.y += it->vel.y * dt;

        if (it->life >= it->maxLife)
            it = particles.erase(it);
        else
            ++it;
    }
}

void drawParticles(const std::vector<Particle> &particles, Color c1, Color c2, Color c3)
{
    for (const auto &p : particles)
    {
        float t = p.life / p.maxLife;
        float alpha = 1.f - t;
        float size = (1.5f - t);

        DrawCircleV(p.pos, 4 * size, Fade(c1, alpha * 0.6f));
        DrawCircleV(p.pos, 2 * size, Fade(c2, alpha));
        DrawCircleV(p.pos, 1 * size, Fade(c3, alpha));
    }
}