#include "../include/LevelOneState.h"

LevelOneState::LevelOneState(){}
LevelOneState::~LevelOneState(){}

void LevelOneState::onEnter(StateManager &sm)
    {
        auto &ctx = sm.getContext();

        _mapTexture = LoadTexture("../assets/ui/map.png");

        _lockAnim.loadSheet(ctx, "../assets/ui/flag/Graysprite.png", 6, 0.2f);

        _tooltipTexture = LoadTexture("../assets/ui/papyrus.png");
        _tooltip.init(_tooltipTexture);

        _levels =
            {
                {0, {160, 120}},
                {1, {700, 650}},
                {2, {1100, 250}},
                {3, {1020, 740}},
                {4, {300, 700}}};

        for (auto &n : _levels)
            n.init(ctx);
    }

    void LevelOneState::update(StateManager &sm, float dt)
    {
        auto &ctx = sm.getContext();

        Vector2 mouse = GetMousePosition();

        int unlocked = ctx.getHighestUnlockedLevel();

        ctx.updateMusic();

        
        
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        float drawW = w * _zoom;
        float drawH = h * _zoom;

        bool tooltipVisible = false;

        if (IsKeyPressed(KEY_ESCAPE))
        {
            sm.pushState<PauseMenuState>();
            return;
        }

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 origin = { dest.width / 2, dest.height / 2 };

            DrawTexturePro(_towerWheelChoiceTexture, source, dest, origin, 0.0f, WHITE);
        }
    }
    else
    {
        _bDoTowerWheelAnimationStarted = true;
        displayTowerWheelAnimation(dt);
    }
}

// true -> on affiche la tour || false -> on degage la roue (dans tous les cas elle degage en fait)
bool LevelOneState::didPlayerClickedOnWheel()
{
    bool retVal { false };

    float wheelSize = 256;
    Rectangle topLeft = { _wheelCenter.x - 64, _wheelCenter.y - wheelSize/2, 64, 64 };
    Rectangle topRight = { _wheelCenter.x, _wheelCenter.y - wheelSize/2, 64, 64 };
    Rectangle bottom = { _wheelCenter.x - 32, _wheelCenter.y + wheelSize/2 - 64, 64, 64 };
    Rectangle left = { _wheelCenter.x - wheelSize/2, _wheelCenter.y - 32, 64, 64 };
    Rectangle right = { _wheelCenter.x + wheelSize/2 - 64, _wheelCenter.y - 32, 64, 64 };

    DrawCircleV(_mousePosition, 5, BLUE);
    DrawRectangleLinesEx(topLeft, 2, RED);
    DrawRectangleLinesEx(topRight, 2, RED);
    DrawRectangleLinesEx(bottom, 2, RED);
    DrawRectangleLinesEx(left, 2, RED);
    DrawRectangleLinesEx(right, 2, RED);

    std::cout << "normalement le top c'est x = " << _mousePosition.x - 32 << " y = " << _mousePosition.y << '\n';

    std::cout << "_mousePosition pour savoir si on clique dans le carre rouge = x" << _mousePosition.x << ", y = " << _mousePosition.y << '\n';

    // la on check si un click dans une des boites est fait pour afficher la tour
    if (CheckCollisionPointRec(_mousePosition, topLeft))
    {
        std::cout << "TOP LEFT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, topRight))
    {
        std::cout << "TOP RIGHT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, bottom))
    {
        std::cout << "BOTTOM\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, left))
    {
        std::cout << "LEFT\n";
        retVal = true;
    }
    if (CheckCollisionPointRec(_mousePosition, right))
    {
        std::cout << "RIGHT\n";
        retVal = true;
    }

    return retVal;
}

// true -> on affiche la wheel || false y a r
bool LevelOneState::didPlayerClickedOnTowerPlace()
{
    bool retVal { false };

    if ((_mousePosition.x >= 539 && _mousePosition.x <= 611) && (_mousePosition.y >= 413 && _mousePosition.y <= 452))
    {
        retVal = true;
    }

    void LevelOneState::render(StateManager &sm)
    {
        auto &ctx = sm.getContext();

        
        
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        float zoom = 1.05f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        ClearBackground(BLACK);

        Rectangle src{
            0, 0,
            (float)_mapTexture.width,
            (float)_mapTexture.height};

        Rectangle dst{
            (w - drawW) * 0.5f + _bgOffset.x,
            (h - drawH) * 0.5f + _bgOffset.y,
            drawW,
            drawH};

        DrawTexturePro(_mapTexture, src, dst, {0, 0}, 0, WHITE);

        int unlocked = ctx.getHighestUnlockedLevel();

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;

            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);

            node.draw(pos, _lockAnim, unlockedNode);
        }

        _tooltip.draw(w, h);
    }

    void LevelOneState::onExit(StateManager &)
    {
        UnloadTexture(_mapTexture);
        UnloadTexture(_tooltipTexture);
    }