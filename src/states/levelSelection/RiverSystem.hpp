#pragma once

#include <vector>
#include <ui/animation/Particles.hpp>
#include <core/Utils.hpp>
#include <array>
#include <core/SystemManager.hpp>
#include "raylib.h"

class RiverSystem : public engine::System
{
private:
    struct RiverParticle
    {
        int segment = 0;
        float t = 0.f;
        float speed = 0.f;
        float offset = 0.f;
        float life = 0.f;
        float maxLife = 0.f;
    };

    struct RiverSegment
    {
        Vector2 a;
        Vector2 b;
        Vector2 normal;
        float length;
    };

    std::vector<RiverParticle> _particles;

    std::array<Vector2, 6> _path{};
    std::array<Vector2, 6> _screenPath{};
    std::array<RiverSegment, 5> _segments{};

    float _spawnTimer = 0.f;

    static constexpr float RIVER_WIDTH = 20.f;
    static constexpr size_t MAX_PARTICLES = 256;

    void spawn(void)
    {
        if (_particles.size() >= MAX_PARTICLES)
            return;

        RiverParticle p;

        p.segment = 0;
        p.t = GetRandomValue(0, 1000) / 1000.f;
        p.speed = GetRandomValue(20, 40) / 100.f;
        p.offset = GetRandomValue(-100, 100) / 100.f * RIVER_WIDTH;
        p.life = 0.f;
        p.maxLife = GetRandomValue(3, 6);

        _particles.push_back(p);
    }

    void remove(size_t i)
    {
        _particles[i] = _particles.back();
        _particles.pop_back();
    }

    void computeSegments(int w, int h)
    {
        for (size_t i = 0; i < _path.size(); ++i)
            _screenPath[i] = utils::normalizedToScreen(_path[i], w, h);

        for (size_t i = 0; i < _segments.size(); ++i)
        {
            Vector2 a = _screenPath[i];
            Vector2 b = _screenPath[i + 1];

            Vector2 dir{b.x - a.x, b.y - a.y};

            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);

            if (len > 0.0001f)
            {
                dir.x /= len;
                dir.y /= len;
            }

            Vector2 normal{-dir.y, dir.x};

            _segments[i] = {a, b, normal, len};
        }
    }

    Vector2 computePosition(const RiverParticle &p) const
    {
        const RiverSegment &s = _segments[p.segment];

        Vector2 pos{s.a.x + (s.b.x - s.a.x) * p.t, s.a.y + (s.b.y - s.a.y) * p.t};

        pos.x += s.normal.x * p.offset;
        pos.y += s.normal.y * p.offset;

        return pos;
    }

public:
    void setup(const std::array<Vector2, 6> &path)
    {
        _path = path;
    }

    void init(GameContext &ctx)
    {
        _particles.clear();
        _particles.reserve(MAX_PARTICLES);

        onResize(ctx, GetScreenWidth(), GetScreenHeight());
    }

    void unload(void) {}

    void update(GameContext &, float dt)
    {
        _spawnTimer += dt;

        // spawn rate fixe
        while (_spawnTimer > 0.02f)
        {
            spawn();
            _spawnTimer -= 0.02f;
        }

        // update particles
        for (size_t i = 0; i < _particles.size();)
        {
            auto &p = _particles[i];

            p.t += p.speed * dt;
            p.life += dt;

            // segment suivant
            if (p.t > 1.f)
            {
                p.t = 0.f;
                p.segment++;

                if (p.segment >= (int)_segments.size())
                {
                    remove(i);
                    continue;
                }
            }

            // mort
            if (p.life > p.maxLife)
            {
                remove(i);
                continue;
            }

            ++i;
        }
    }

    void draw(GameContext &)
    {
        for (const auto &p : _particles)
        {
            Vector2 pos = computePosition(p);

            float alpha = 1.f - (p.life / p.maxLife);

            DrawCircleV(pos, 2.5f, Fade(SKYBLUE, alpha));
        }
    }

    void onResize(GameContext &, int w, int h)
    {
        computeSegments(w, h);
    }

    int updateOrder(void) const { return 0; }
    int renderOrder(void) const { return 5; }
};