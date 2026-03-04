#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "../../engine/UISprites.hpp"
#include "GameState.hpp"

#include "../ui/UIButton.hpp"
#include "../ui/AnimatedBackGround.hpp"
#include "../ui/ScreenFade.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <string>
#include <algorithm>

constexpr int TILE_SIZE = 32;

class MenuState : public IGameState
{
private:
    std::vector<UIButton> buttons;

    AnimatedBackground background;

    ScreenFade fade;

    SDL_Texture *uiTexture = nullptr;

    float appearTimer = 0.f;
    const float appearDuration = 1.f;

    bool layoutDirty = true;

    Mix_Music *bgMusic = nullptr;

    bool requestPlay = false;

private:
    std::vector<SDL_Rect> computeLayout(int w, int h)
    {
        std::vector<SDL_Rect> rects;

        int bw = w / 3;
        int bh = TILE_SIZE * 2;
        int spacing = TILE_SIZE;

        int totalH = buttons.size() * bh + (buttons.size() - 1) * spacing;

        int y = (h - totalH) / 2;
        int cx = w / 2;

        for (size_t i = 0; i < buttons.size(); i++)
        {
            rects.push_back({cx - bw / 2,
                             y + int(i) * (bh + spacing),
                             bw,
                             bh});
        }

        return rects;
    }

    void rebuildLayout(SDL_Renderer *renderer)
    {
        if (!layoutDirty)
            return;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        auto rects = computeLayout(w, h);

        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i].setRect(rects[i]);

        layoutDirty = false;
    }

public:
    MenuState()
    {
        buttons.resize(3);

        buttons[0].setSprite(UISprites::Play.src);
        buttons[1].setSprite(UISprites::Options.src);
        buttons[2].setSprite(UISprites::Exit.src);
    }

    ~MenuState()
    {
        if (bgMusic)
        {
            Mix_HaltMusic();
            Mix_FreeMusic(bgMusic);
        }
    }

    void onEnter(StateManager &sm) override
    {
        SDL_ShowCursor(SDL_ENABLE);

        auto *assets = sm.getContext().assets;
        auto *renderer = sm.getContext().renderer;

        appearTimer = 0.f;
        layoutDirty = true;

        // UI texture
        if (!uiTexture)
        {
            assets->loadTexture("ui", "assets/ui/spriteMenu.png");
            uiTexture = assets->getTexture("ui");
        }

        // Background frames
        background = AnimatedBackground();

        for (int i = 0;; i++)
        {
            std::string file = "assets/ui/bg/frame" + std::to_string(i) + ".png";

            SDL_Surface *surf = IMG_Load(file.c_str());
            if (!surf)
                break;

            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);

            if (!tex)
                break;

            background.addFrame(tex);
        }

        // Music
        if (!bgMusic)
        {
            bgMusic = Mix_LoadMUS("assets/audio/menu_music.mp3");
            if (bgMusic)
                Mix_PlayMusic(bgMusic, -1);
        }

        // Callbacks
        buttons[0].onClick = [this]()
        {
            requestPlay = true;
        };

        buttons[1].onClick = []()
        {
            SDL_Log("OPTIONS MENU");
        };

        buttons[2].onClick = []()
        {
            SDL_Event e;
            e.type = SDL_QUIT;
            SDL_PushEvent(&e);
        };

        rebuildLayout(renderer);
    }

    void onExit(StateManager &) override
    {
        if (bgMusic)
            Mix_HaltMusic();
    }

    void handleEvent(StateManager &, const SDL_Event &e) override
    {
        if (fade.isActive())
            return;

        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            layoutDirty = true;
        }

        if (e.type == SDL_MOUSEMOTION)
        {
            int x = e.motion.x;
            int y = e.motion.y;

            for (auto &b : buttons)
                b.setHovered(b.contains(x, y));
        }

        if (e.type == SDL_MOUSEBUTTONDOWN &&
            e.button.button == SDL_BUTTON_LEFT)
        {
            int x = e.button.x;
            int y = e.button.y;

            for (auto &b : buttons)
            {
                if (b.contains(x, y))
                {
                    fade.start();
                    b.click();
                    break;
                }
            }
        }
    }

    void update(StateManager &sm, float dt) override
    {
        background.update(dt);

        appearTimer = std::min(appearTimer + dt, appearDuration);

        fade.update(dt);

        if (requestPlay && fade.finished())
        {
            sm.changeState<GameState>();
            return;
        }
    }

    void render(StateManager &sm) override
    {
        auto *renderer = sm.getContext().renderer;

        rebuildLayout(renderer);

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        // background
        background.render(renderer, w, h);

        float appear = appearTimer / appearDuration;

        // buttons
        for (auto &b : buttons)
            b.render(renderer, uiTexture, appear);

        // transition
        fade.render(renderer, w, h);
    }

    bool allowUpdateBelow() const override
    {
        return false;
    }

    bool allowRenderBelow() const override
    {
        return false;
    }
};