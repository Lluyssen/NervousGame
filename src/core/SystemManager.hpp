#pragma once
#include "GameContext.hpp"
#include <vector>
#include <algorithm>

namespace engine
{
    class ISystem
    {
    public:
        virtual ~ISystem(void) = default;
        ISystem(void) = default;
        ISystem(const ISystem &) = delete;
        ISystem &operator=(const ISystem &) = delete;
        ISystem(ISystem &&) = delete;
        ISystem &operator=(ISystem &&) = delete;

        virtual void init(GameContext &) {}
        virtual void update(GameContext &, float) {}
        virtual void draw(GameContext &) {}
        virtual void unload(void) {}
        virtual int updateOrder(void) const { return 0; }
        virtual int renderOrder(void) const { return 0; }
        virtual void onResize(GameContext &, int, int) {}
    };

    class SystemManager
    {
    private:
        std::vector<ISystem *> _updateSystems;
        std::vector<ISystem *> _renderSystems;
        bool _sorted = false;

        void sortSystems(void)
        {
            if (_sorted)
                return;

            std::sort(_updateSystems.begin(), _updateSystems.end(),
                      [](ISystem *a, ISystem *b)
                      {
                          return a->updateOrder() < b->updateOrder();
                      });

            std::sort(_renderSystems.begin(), _renderSystems.end(),
                      [](ISystem *a, ISystem *b)
                      {
                          return a->renderOrder() < b->renderOrder();
                      });

            _sorted = true;
        }

    public:
        SystemManager(void)
        {
            _updateSystems.reserve(32);
            _renderSystems.reserve(32);
        }

        void add(ISystem &sys)
        {
            auto it = std::find(_updateSystems.begin(), _updateSystems.end(), &sys);

            if (it != _updateSystems.end())
                return;
            _updateSystems.push_back(&sys);
            _renderSystems.push_back(&sys);
            _sorted = false;

            std::sort(_updateSystems.begin(), _updateSystems.end(),
                      [](ISystem *a, ISystem *b)
                      {
                          return a->updateOrder() < b->updateOrder();
                      });

            std::sort(_renderSystems.begin(), _renderSystems.end(),
                      [](ISystem *a, ISystem *b)
                      {
                          return a->renderOrder() < b->renderOrder();
                      });
        }

        void init(GameContext &ctx)
        {
            sortSystems();
            for (auto s : _updateSystems)
                s->init(ctx);
        }

        void update(GameContext &ctx, float dt)
        {
            for (auto s : _updateSystems)
                s->update(ctx, dt);
        }

        void draw(GameContext &ctx)
        {
            for (auto s : _renderSystems)
                s->draw(ctx);
        }

        void unload(void)
        {
            for (auto s : _renderSystems)
                s->unload();
        }

        void clear(void)
        {
            _updateSystems.clear();
            _renderSystems.clear();
        }

        void resize(GameContext &ctx, int w, int h)
        {
            for (auto s : _updateSystems)
                s->onResize(ctx, w, h);
        }
    };
}
