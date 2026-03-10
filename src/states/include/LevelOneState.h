#ifndef _LEVEL_ONE_STATE_H_
#define _LEVEL_ONE_STATE_H_

#include "raylib.h"
#include "../game/LevelNode.hpp"
#include "../../ui/petitMenu/PetitMenu.hpp"
#include "../PauseMenuState.hpp"
#include "../MapState.hpp"

class LevelOneState : public IGameState
{
public:
    LevelOneState();
    ~LevelOneState();
    void onEnter(StateManager &sm) override;
    void update(StateManager &sm, float dt) override;
    void render(StateManager &sm) override;
    void onExit(StateManager &) override;

private:
    Texture2D _mapTexture{};
    AnimatedSprite _lockAnim;

    std::vector<LevelNode> _levels;

    Vector2 _bgOffset{0, 0};

    PetitMenu _tooltip;
    Texture2D _tooltipTexture;

    float _zoom = 1.05f;

    // conversion map -> écran
    Vector2 worldToScreen(Vector2 world, int w, int h, float drawW, float drawH)
    {
        float scaleX = drawW / (float)_mapTexture.width;
        float scaleY = drawH / (float)_mapTexture.height;

        return {
            world.x * scaleX + (w - drawW) * 0.5f + _bgOffset.x,
            world.y * scaleY + (h - drawH) * 0.5f + _bgOffset.y};
    }
};

#endif