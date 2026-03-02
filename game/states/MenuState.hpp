#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "GameState.hpp"
#include <SDL2/SDL_ttf.h>

class MenuState : public IGameState {
public:
    MenuState() {
        buttons = {
            { ButtonType::Play,    "Jeu" },
            { ButtonType::Settings,"Settings" },
            { ButtonType::Credits, "Credits" }
        };
    }

    ~MenuState() override {
        for (auto& b : buttons) {
            if (b.texture) SDL_DestroyTexture(b.texture);
        }
        if (font) TTF_CloseFont(font);
    }

    void onEnter(StateManager& sm) override {
        SDL_ShowCursor(SDL_ENABLE);
        layoutDirty = true;

        // Charger la font
        font = TTF_OpenFont("assets/fonts/alagard.ttf", 28);
        if (!font) {
            SDL_Log("Failed to load font: %s", TTF_GetError());
            return;
        }

        // Créer les textures texte
        auto* renderer = sm.getContext().renderer;
        SDL_Color white{255,255,255,255};

        for (auto& b : buttons) {
            SDL_Surface* surf =
                TTF_RenderUTF8_Blended(font, b.label.c_str(), white);
            b.texture = SDL_CreateTextureFromSurface(renderer, surf);
            b.textW = surf->w;
            b.textH = surf->h;
            SDL_FreeSurface(surf);
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
            e.button.button == SDL_BUTTON_LEFT) {

            if (hovered == ButtonType::Play)
                sm.changeState<GameState>();
            else if (hovered == ButtonType::Settings)
                SDL_Log("Settings clicked");
            else if (hovered == ButtonType::Credits)
                SDL_Log("Credits clicked");
        }
    }

    void update(StateManager&, float) override {}

    void render(StateManager& sm) override {
        auto* renderer = sm.getContext().renderer;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        if (layoutDirty || w != lastW || h != lastH) {
            cachedRects = computeLayout(w, h);
            lastW = w;
            lastH = h;
            layoutDirty = false;
        }

        // Fond
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
        SDL_RenderClear(renderer);

        // Boutons + texte
        for (size_t i = 0; i < cachedRects.size(); ++i) {
            const auto& rect = cachedRects[i];
            const auto& b = buttons[i];

            bool isHover = (hovered == b.type);

            if (isHover)
                SDL_SetRenderDrawColor(renderer, 180, 180, 220, 255);
            else
                SDL_SetRenderDrawColor(renderer, 100, 100, 140, 255);

            SDL_RenderFillRect(renderer, &rect);

            // --- Texte centré ---
            if (b.texture) {
                SDL_Rect textRect;
                textRect.w = b.textW;
                textRect.h = b.textH;
                textRect.x = rect.x + (rect.w - b.textW) / 2;
                textRect.y = rect.y + (rect.h - b.textH) / 2;

                SDL_RenderCopy(renderer, b.texture, nullptr, &textRect);
            }
        }
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
        SDL_Texture* texture = nullptr;
        int textW = 0;
        int textH = 0;
    };

    std::vector<Button> buttons;
    std::vector<SDL_Rect> cachedRects;

    TTF_Font* font = nullptr;

    bool layoutDirty = true;
    int lastW = 0;
    int lastH = 0;

    ButtonType hovered = ButtonType::None;

    std::vector<SDL_Rect> computeLayout(int windowW, int windowH) const {
        std::vector<SDL_Rect> rects;

        int buttonW = windowW / 4;
        int buttonH = windowH / 12;
        int spacing = buttonH / 2;

        int totalH =
            static_cast<int>(buttons.size()) * buttonH +
            static_cast<int>(buttons.size() - 1) * spacing;

        int startY = (windowH - totalH) / 2;
        int centerX = windowW / 2;

        for (size_t i = 0; i < buttons.size(); ++i) {
            rects.push_back({
                centerX - buttonW / 2,
                startY + static_cast<int>(i) * (buttonH + spacing),
                buttonW,
                buttonH
            });
        }

        return rects;
    }

    ButtonType getButtonAt(int x, int y) const {
        for (size_t i = 0; i < cachedRects.size(); ++i) {
            const auto& r = cachedRects[i];
            if (x >= r.x && x <= r.x + r.w &&
                y >= r.y && y <= r.y + r.h) {
                return buttons[i].type;
            }
        }
        return ButtonType::None;
    }
};