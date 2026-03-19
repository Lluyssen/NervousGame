#pragma once

#include "GameContext.hpp"

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <typeindex>
#include <cassert>

namespace engine
{

    struct System
    {
        void init(GameContext &) {}
        void update(GameContext &, float) {}
        void draw(GameContext &) {}
        void onResize(GameContext &, int, int) {}
        void unload(void) {}
    };

    class SystemManager
    {
    private:
        struct Entry
        {
            void *instance;
            int priority;

            void (*init)(void *, GameContext &);
            void (*update)(void *, GameContext &, float);
            void (*draw)(void *, GameContext &);
            void (*resize)(void *, GameContext &, int, int);
            void (*unload)(void *);
        };

        std::vector<Entry> _systems;
        std::unordered_map<std::type_index, void *> _lookup;

        template <typename T>
        static Entry makeEntry(T &sys, int priority)
        {
            Entry e;
            e.instance = &sys;
            e.priority = priority;
            e.init = [](void *p, GameContext &ctx) { static_cast<T *>(p)->init(ctx); };
            e.update = [](void *p, GameContext &ctx, float dt) { static_cast<T *>(p)->update(ctx, dt); };
            e.draw = [](void *p, GameContext &ctx) { static_cast<T *>(p)->draw(ctx); };
            e.resize = [](void *p, GameContext &ctx, int w, int h) { static_cast<T *>(p)->onResize(ctx, w, h); };
            e.unload = [](void *p) { static_cast<T *>(p)->unload(); };
            return e;
        }

        template <typename T>
        T *sys_ptr(void)
        {
            auto it = _lookup.find(typeid(T));
            if (it == _lookup.end())
                return nullptr;

            return static_cast<T *>(it->second);
        }

    public:
        SystemManager(void)
        {
            _systems.reserve(32);
        }

        template <typename T>
        void add(T &sys, int priority = 0)
        {
            static_assert(std::is_base_of<System, T>::value, "System must inherit from engine::System");

            if (_lookup.find(typeid(T)) != _lookup.end())
                return;

            _lookup[typeid(T)] = &sys;

            _systems.push_back(makeEntry(sys, priority));
            
            std::sort(_systems.begin(), _systems.end(), [](const Entry &a, const Entry &b) { return a.priority < b.priority; });

            assert(sys_ptr<T>() != nullptr);
        }

        template <typename T>
        T *get(void)
        {
            return sys_ptr<T>();
        }

        void init(GameContext &ctx)
        {
            for (auto &e : _systems)
                e.init(e.instance, ctx);
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

        void resize(GameContext &ctx, int w, int h)
        {
            for (auto &e : _systems)
                e.resize(e.instance, ctx, w, h);
        }

        void shutdown(void)
        {
            for (auto &e : _systems)
                e.unload(e.instance);
        }

        void clear(void)
        {
            _systems.clear();
            _lookup.clear();
        }
    };
}