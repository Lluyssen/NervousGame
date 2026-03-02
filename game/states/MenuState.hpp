#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "GameState.hpp"
#include <SDL2/SDL_ttf.h>

constexpr int TILE_SIZE = 32;

class MenuState : public IGameState {
public:
    MenuState() {
        buttons = {
            { ButtonType::Play,    "JEU",      nullptr },
            { ButtonType::Settings,"SETTINGS", nullptr },
            { ButtonType::Credits, "CREDITS",  nullptr }
        };
    }

    void onEnter(StateManager& sm) override {
        SDL_ShowCursor(SDL_ENABLE);

        appearStep = 0;
        hoverTick = 0;
        transitioning = false;
        transitionAlpha = 0;
        layoutDirty = true;

        auto* renderer = sm.getContext().renderer;
        auto* assets   = sm.getContext().assets;

        // --- Load textures via AssetManager (OWNERSHIP CENTRAL) ---
        assets->loadTexture("menu_play",     "assets/ui/menu_play.png");
        assets->loadTexture("menu_settings", "assets/ui/menu_settings.png");
        assets->loadTexture("menu_credits",  "assets/ui/menu_credits.png");

        buttons[0].texture = assets->getTexture("menu_play");
        buttons[1].texture = assets->getTexture("menu_settings");
        buttons[2].texture = assets->getTexture("menu_credits");

        // --- Create background render target (OWNED by this state) ---
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        backgroundTarget = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            w, h
        );
    }

    void onExit(StateManager&) override {
        // IMPORTANT : on détruit UNIQUEMENT ce que le state possède
        if (backgroundTarget) {
            SDL_DestroyTexture(backgroundTarget);
            backgroundTarget = nullptr;
        }
    }

    void handleEvent(StateManager& sm, const SDL_Event& e) override {
        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            layoutDirty = true;
        }

        if (e.type == SDL_MOUSEMOTION) {
            hovered = getButtonAt(e.motion.x, e.motion.y);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN &&
            e.button.button == SDL_BUTTON_LEFT &&
            !transitioning) {

            if (hovered == ButtonType::Play) {
                transitioning = true;
                transitionAlpha = 0;
            }
        }
    }

    void update(StateManager& sm, float) override {
        if (appearStep < maxAppearStep)
            appearStep++;

        hoverTick = (hoverTick + 1) % 30;

        if (transitioning) {
            transitionAlpha = std::min<Uint8>(255, transitionAlpha + 32);
            if (transitionAlpha >= 255) {
                sm.changeState<GameState>();
            }
        }
    }

    void render(StateManager& sm) override {
        auto* renderer = sm.getContext().renderer;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        if (layoutDirty) {
            cachedRects = computeLayout(w, h);
            layoutDirty = false;
        }

        renderBackground(renderer, w, h);

        // --- Render buttons ---
        for (size_t i = 0; i < cachedRects.size(); ++i) {
            SDL_Rect rect = cachedRects[i];
            auto& btn = buttons[i];

            int slide = (maxAppearStep - appearStep) * 4;
            Uint8 alpha = appearStep * 32;
            rect.y += slide;

            bool isHover = (hovered == btn.type);
            bool pulse = hoverTick < 15;

            Uint8 r = 100, g = 100, b = 140;
            if (isHover && pulse) {
                r = 180; g = 180; b = 220;
            }

            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, r, g, b, alpha);
            SDL_RenderFillRect(renderer, &rect);

            // --- Sprite bouton (pixel) ---
            if (btn.texture) {
                SDL_SetTextureAlphaMod(btn.texture, alpha);
                SDL_RenderCopy(renderer, btn.texture, nullptr, &rect);
            }
        }

        // --- Transition pixel ---
        if (transitioning) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, transitionAlpha);
            SDL_Rect full{0, 0, w, h};
            SDL_RenderFillRect(renderer, &full);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

private:
    enum class ButtonType {
        None,
        Play,
        Settings,
        Credits
    };

    struct Button {
        ButtonType type;
        std::string label;
        SDL_Texture* texture; // NON-OWNING (AssetManager)
    };

    // UI
    std::vector<Button> buttons;
    std::vector<SDL_Rect> cachedRects;
    ButtonType hovered = ButtonType::None;

    // Animation pixel
    int appearStep = 0;
    const int maxAppearStep = 8;
    int hoverTick = 0;

    // Transition
    bool transitioning = false;
    Uint8 transitionAlpha = 0;

    // Background (OWNED by MenuState)
    SDL_Texture* backgroundTarget = nullptr;

    bool layoutDirty = true;

    // --- Helpers ---

    std::vector<SDL_Rect> computeLayout(int w, int h) const {
        std::vector<SDL_Rect> r;

        int bw = w / 3;
        int bh = TILE_SIZE * 2;
        int spacing = TILE_SIZE;

        int totalH = buttons.size() * bh + (buttons.size() - 1) * spacing;
        int y = (h - totalH) / 2;
        int cx = w / 2;

        for (size_t i = 0; i < buttons.size(); ++i) {
            r.push_back({
                cx - bw / 2,
                y + int(i) * (bh + spacing),
                bw,
                bh
            });
        }
        return r;
    }

    ButtonType getButtonAt(int x, int y) const {
        for (size_t i = 0; i < cachedRects.size(); ++i) {
            const auto& r = cachedRects[i];
            if (x >= r.x && x <= r.x + r.w &&
                y >= r.y && y <= r.y + r.h)
                return buttons[i].type;
        }
        return ButtonType::None;
    }

    void renderBackground(SDL_Renderer* renderer, int w, int h) {
        SDL_SetRenderTarget(renderer, backgroundTarget);

        for (int y = 0; y < h; y += 8) {
            for (int x = 0; x < w; x += 8) {
                Uint8 v = Uint8((x + y + appearStep * 12) % 40);
                SDL_SetRenderDrawColor(renderer, v, v, v + 20, 255);
                SDL_Rect r{x, y, 8, 8};
                SDL_RenderFillRect(renderer, &r);
            }
        }

        SDL_SetRenderTarget(renderer, nullptr);
        SDL_RenderCopy(renderer, backgroundTarget, nullptr, nullptr);
    }
};