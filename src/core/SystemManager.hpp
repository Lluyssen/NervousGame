#pragma once
#include "GameContext.hpp"
#include <vector>
#include <algorithm>

#pragma once

#include <vector>
#include <algorithm>

namespace engine
{
    class SystemManager
    {
    private:
        struct Entry
        {
            void *instance;

            void (*init)(void *, GameContext &);
            void (*update)(void *, GameContext &, float);
            void (*draw)(void *, GameContext &);
            void (*resize)(void *, GameContext &, int, int);
            void (*unload)(void *);
        };

        std::vector<Entry> _systems;

    public:
        SystemManager(void)
        {
            _systems.reserve(32);
        }

        template <typename T>
        void add(T &sys)
        {
            for (auto &e : _systems)
                if (e.instance == &sys)
                    return;

            Entry e;
            e.instance = &sys;

            e.init = [](void *p, GameContext &ctx)
            {
                static_cast<T *>(p)->init(ctx);
            };

            e.update = [](void *p, GameContext &ctx, float dt)
            {
                static_cast<T *>(p)->update(ctx, dt);
            };

            e.draw = [](void *p, GameContext &ctx)
            {
                static_cast<T *>(p)->draw(ctx);
            };

            e.resize = [](void *p, GameContext &ctx, int w, int h)
            {
                static_cast<T *>(p)->onResize(ctx, w, h);
            };

            e.unload = [](void *p)
            {
                static_cast<T *>(p)->unload();
            };

            _systems.push_back(e);
        }

        void init(GameContext &ctx)
        {
            for (auto &e : _systems)
                e.init(e.instance, ctx);
        }

        void resize(GameContext &ctx, int w, int h)
        {
            for (auto &e : _systems)
                e.resize(e.instance, ctx, w, h);
        }

        void update(GameContext &ctx, float dt)
        {
            for (auto &e : _systems)
                e.update(e.instance, ctx, dt);
        }

        void draw(GameContext &ctx)
        {
            for (auto &e : _systems)
                e.draw(e.instance, ctx);
        }

        void unload(void)
        {
            for (auto &e : _systems)
                e.unload(e.instance);
        }

        void clear()
        {
            _systems.clear();
        }
    };
}