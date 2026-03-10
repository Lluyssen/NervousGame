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

        int w = ctx.getWidth();
        int h = ctx.getHeight();
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

            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);

            if (node.update(dt, mouse, pos, unlockedNode))
                sm.changeState<MapState>();

            if (!tooltipVisible && CheckCollisionPointCircle(mouse, pos, 30))
            {
                std::vector<std::string> lines = {
                    TextFormat("Level %d", node.id() + 1),
                    "Difficulty: Easy",
                    "Reward: 200 gold"};

                _tooltip.show({pos.x, pos.y - 40}, lines);

                tooltipVisible = true;
            }
        }

        if (!tooltipVisible)
            _tooltip.hide();

        // parallaxe souris
        float nx = (mouse.x / w) - 0.5f;
        float ny = (mouse.y / h) - 0.5f;

        _bgOffset.x += (nx * 40.f - _bgOffset.x) * 4.f * dt;
        _bgOffset.y += (ny * 40.f - _bgOffset.y) * 4.f * dt;

        _lockAnim.update(dt);
    }

    void LevelOneState::render(StateManager &sm)
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

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