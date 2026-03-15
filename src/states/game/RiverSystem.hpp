#pragma once

#include <vector>
#include <ui/animation/Particles.hpp>
#include <core/Utils.hpp>
#include <array>
#include <core/SystemManager.hpp>

class RiverSystem : public engine::ISystem
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

public:
    void setup(const std::array<Vector2, 6> &path)
    {
        _path = path;
    }

    void init(GameContext &ctx) override
    {
        _particles.reserve(MAX_PARTICLES);

        //onResize(ctx.getWidth(), ctx.getHeight());
        onResize(GetScreenWidth(), GetScreenHeight());
    }

    void update(GameContext &ctx, float dt) override
    {
        _spawnTimer += dt;

        while (_spawnTimer > 0.02f)
        {
            spawn();
            _spawnTimer -= 0.02f;
        }

        for (size_t i = 0; i < _particles.size();)
        {
            auto &p = _particles[i];

            p.t += p.speed * dt;
            p.life += dt;

            if (p.t > 1.f)
            {
                p.t = 0.f;
                p.segment++;

                if (p.segment >= (int)_segments.size())
                {
                    _particles[i] = _particles.back();
                    _particles.pop_back();
                    continue;
                }
            }

            if (p.life > p.maxLife)
            {
                _particles[i] = _particles.back();
                _particles.pop_back();
                continue;
            }

            i++;
        }
    }

    void draw(GameContext &) override
    {
        for (auto &p : _particles)
        {
            Vector2 pos = computePosition(p);

            float alpha = 1.f - (p.life / p.maxLife);

            DrawCircleV(pos, 2.5f, Fade(SKYBLUE, alpha));
        }
    }

    int renderOrder(void) const override
    {
        return 5;
    }

    void onResize(int w, int h)
    {
        for (size_t i = 0; i < _path.size(); i++)
            _screenPath[i] = utils::normalizedToScreen(_path[i], w, h);

        for (size_t i = 0; i < _segments.size(); i++)
        {
            Vector2 a = _screenPath[i];
            Vector2 b = _screenPath[i + 1];
            Vector2 dir{ b.x - a.x, b.y - a.y};

            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);

            if (len > 0.0001f)
            {
                dir.x /= len;
                dir.y /= len;
            }

            Vector2 normal{ -dir.y, dir.x};
            _segments[i] = {a, b, normal, len};
        }
    }

private:
    void spawn()
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

    Vector2 computePosition(const RiverParticle &p)
    {
        const RiverSegment &s = _segments[p.segment];

        Vector2 pos{ s.a.x + (s.b.x - s.a.x) * p.t, s.a.y + (s.b.y - s.a.y) * p.t};
        pos.x += s.normal.x * p.offset;
        pos.y += s.normal.y * p.offset;

        return pos;
    }
};