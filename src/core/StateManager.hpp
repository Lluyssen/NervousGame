#pragma once

#include <vector>
#include <memory>
#include <functional>
#include "GameContext.hpp"
#include "IGameState.hpp"
#include <functional>
#include <tuple>
#include <algorithm>

class IGameState;
class GameContext;

class StateManager
{
private:
    GameContext &_context;

    std::vector<std::unique_ptr<IGameState>> _stack;
    std::vector<std::function<void(void)>> _pending;

    enum class TransitionPhase
    {
        None,
        Closing,
        Hold,
        Opening
    };

    struct Transition
    {
        TransitionPhase phase = TransitionPhase::None;
        std::unique_ptr<IGameState> next;

        float timer = 0.f;
        bool justStarted = false; // FIX IMPORTANT
    };

    Transition _transition;

    Texture2D _texture{};

    static constexpr float CLOSING_DURATION = 0.5f;
    static constexpr float OPENING_DURATION = 0.8f;

    void applyPending(void)
    {
        for (auto &cmd : _pending)
            cmd();
        _pending.clear();
    }

    void clearImmediate(void)
    {
        while (!_stack.empty())
        {
            _stack.back()->onExit(*this);
            _stack.pop_back();
        }
    }

    void updateTransition(float dt)
    {
        switch (_transition.phase)
        {
        case TransitionPhase::Closing:
        {
            _transition.timer += dt;

            if (_transition.timer >= CLOSING_DURATION)
            {
                _transition.phase = TransitionPhase::Hold;
                _transition.timer = 0.f;

                if (_transition.next)
                    _transition.next->startLoading(*this);
            }
            break;
        }

        case TransitionPhase::Hold:
        {
            if (_transition.next)
                _transition.next->updateLoading(*this, dt);

            if (_transition.next && _transition.next->isReady())
            {
                clearImmediate();

                _transition.next->onEnter(*this);
                _stack.push_back(std::move(_transition.next));

                _transition.phase = TransitionPhase::Opening;
                _transition.timer = 0.f;
                _transition.justStarted = true;
            }
            break;
        }

        case TransitionPhase::Opening:
        {
            if (_transition.justStarted)
            {
                _transition.justStarted = false;
                break;
            }

            _transition.timer += dt;

            if (_transition.timer >= OPENING_DURATION)
            {
                _transition.phase = TransitionPhase::None;

                if (_texture.id)
                {
                    UnloadTexture(_texture);
                    _texture = {};
                }
            }
            break;
        }

        default:
            break;
        }
    }

    void renderTransition(void)
    {
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        if (!_texture.id)
            return;

        if (_transition.phase == TransitionPhase::Hold)
        {
            DrawTexturePro(
                _texture,
                {0, 0, (float)_texture.width, (float)_texture.height},
                {0, 0, (float)w, (float)h},
                {0, 0},
                0.f,
                WHITE);
            return;
        }

        float duration = (_transition.phase == TransitionPhase::Closing) ? CLOSING_DURATION : OPENING_DURATION;

        float raw = std::clamp(_transition.timer / duration, 0.f, 1.f);
        float t = raw * raw * (3.f - 2.f * raw);

        float halfW = w * 0.5f;
        float offset;

        if (_transition.phase == TransitionPhase::Closing)
            offset = halfW * (1.f - t);
        else
            offset = halfW * t;

        Rectangle srcLeft = {
            0.f, 0.f,
            _texture.width * 0.5f,
            (float)_texture.height};

        Rectangle srcRight = {
            _texture.width * 0.5f, 0.f,
            _texture.width * 0.5f,
            (float)_texture.height};

        Rectangle dstLeft = {
            -offset, 0.f,
            halfW, (float)h};

        Rectangle dstRight = {
            halfW + offset, 0.f,
            halfW, (float)h};

        DrawTexturePro(_texture, srcLeft, dstLeft, {0, 0}, 0.f, WHITE);
        DrawTexturePro(_texture, srcRight, dstRight, {0, 0}, 0.f, WHITE);
    }

public:
    explicit StateManager(GameContext &ctx) : _context(ctx)
    {
        _stack.reserve(8);
        _pending.reserve(8);
    }

    GameContext &getContext()
    {
        return _context;
    }

    template <typename T, typename... Args>
    void changeState(Args &&...args)
    {
        if (_transition.phase != TransitionPhase::None)
            return;

        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

        _pending.emplace_back(
            [this, argsTuple = std::move(argsTuple)]() mutable
            {
                _transition.next = std::apply(
                    [](auto &&...unpackedArgs)
                    {
                        return std::make_unique<T>(
                            std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
                    },
                    std::move(argsTuple));

                _transition.phase = TransitionPhase::Closing;
                _transition.timer = 0.f;

                _texture = LoadTexture("../assets/ui/loading.png");
            });
    }

    void update(float dt)
    {
        applyPending();

        if (_transition.phase != TransitionPhase::None)
        {
            updateTransition(dt);
            return;
        }

        for (int i = (int)_stack.size() - 1; i >= 0; --i)
        {
            _stack[i]->update(*this, dt);

            if (!_stack[i]->allowUpdateBelow())
                break;
        }
    }

    void render(void)
    {
        for (auto &s : _stack)
            s->render(*this);

        if (_transition.phase == TransitionPhase::Hold && _transition.next)
            _transition.next->render(*this);

        if (_transition.phase != TransitionPhase::None)
            renderTransition();
    }

    IGameState *top(void)
    {
        if (_stack.empty())
            return nullptr;

        return _stack.back().get();
    }

    void clear(void)
    {
        _pending.emplace_back([this]() { clearImmediate(); });
    }

    template <typename T, typename... Args>
    void pushState(Args &&...args)
    {
        if (_transition.phase != TransitionPhase::None)
            return;

        auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

        _pending.emplace_back(
            [this, argsTuple = std::move(argsTuple)]() mutable
            {
                auto state = std::apply([](auto &&...unpackedArgs)
                                        { return std::make_unique<T>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...); },
                                        std::move(argsTuple));

                state->onEnter(*this);
                _stack.push_back(std::move(state));
            });
    }

    void popState(void)
    {
        if (_transition.phase != TransitionPhase::None)
            return;

        _pending.emplace_back([this]()
                              {
            if (_stack.empty())
                return;

            _stack.back()->onExit(*this);
            _stack.pop_back(); });
    }
};