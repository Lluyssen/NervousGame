#pragma once

#include "raylib.h"
#include <vector>
#include <string>

class GameContext;

class AnimatedSprite
{
private:
    // Mode séquence d'images
    std::vector<Texture2D *> _frames;

    // Mode spritesheet
    Texture2D *_sheet = nullptr;
    bool _isSheet = false;

    int _frameWidth = 0;
    int _frameHeight = 0;
    int _sheetFrameCount = 0;

    int _currentFrame = 0;

    float _timer = 0.f;
    float _frameTime = 0.05f;

    bool _ready = false;

public:
    AnimatedSprite() = default;

    /*
        Chargement classique :
        frame0.png
        frame1.png
        frame2.png
    */
    void load(GameContext &ctx,
              const std::string &basePath,
              int frameCount,
              float frameTime = 0.05f)
    {
        _frames.clear();
        _isSheet = false;
        _frameTime = frameTime;

        for (int i = 0; i < frameCount; i++)
        {
            std::string path = basePath + std::to_string(i) + ".png";
            Texture2D &tex = ctx.loadTexture(path);
            _frames.push_back(&tex);
        }

        _ready = true;
    }

    /*
        Chargement spritesheet :

        animation.png
        ┌────┬────┬────┬────┐
        │ f0 │ f1 │ f2 │ f3 │
        └────┴────┴────┴────┘
    */
    void loadSheet(GameContext &ctx,
                   const std::string &path,
                   int frameCount,
                   float frameTime = 0.05f)
    {
        _frames.clear();

        _sheet = &ctx.loadTexture(path);
        _isSheet = true;

        _sheetFrameCount = frameCount;
        _frameTime = frameTime;

        _frameWidth = _sheet->width / frameCount;
        _frameHeight = _sheet->height;

        _currentFrame = 0;
        _timer = 0;

        _ready = true;
    }

    // Chargement progressif (utilisé dans ton menu)
    void loadFrame(GameContext &ctx,
                   const std::string &basePath,
                   int index)
    {
        std::string path = basePath + std::to_string(index) + ".png";
        Texture2D &tex = ctx.loadTexture(path);
        _frames.push_back(&tex);
    }

    void finalize(float frameTime)
    {
        _frameTime = frameTime;
        _ready = true;
        _currentFrame = 0;
        _timer = 0.f;
    }

    void update(float dt)
    {
        if (!_ready)
            return;

        int maxFrames = _isSheet ? _sheetFrameCount : _frames.size();

        if (maxFrames == 0)
            return;

        _timer += dt;

        if (_timer >= _frameTime)
        {
            _timer = 0.f;
            _currentFrame++;

            if (_currentFrame >= maxFrames)
                _currentFrame = 0;
        }
    }

    // Draw à une position (pour LevelNode, UI, ennemis…)
    void draw(Vector2 pos, float scale = 1.0f)
    {
        if (!_ready)
            return;

        if (_isSheet)
        {
            Rectangle src{
                (float)(_frameWidth * _currentFrame),
                0,
                (float)_frameWidth,
                (float)_frameHeight};

            Rectangle dst{
                pos.x - (_frameWidth * scale) * 0.5f,
                pos.y - (_frameHeight * scale) * 0.5f,
                _frameWidth * scale,
                _frameHeight * scale};

            DrawTexturePro(*_sheet, src, dst, {0, 0}, 0, WHITE);
        }
        else
        {
            if (_frames.empty())
                return;

            Texture2D &tex = *_frames[_currentFrame];

            float w = tex.width * scale;
            float h = tex.height * scale;

            DrawTexturePro(
                tex,
                {0, 0, (float)tex.width, (float)tex.height},
                {pos.x - w * 0.5f, pos.y - h * 0.5f, w, h},
                {0, 0},
                0,
                WHITE);
        }
    }

    // Draw fullscreen
    void drawFullscreen(int width, int height)
    {
        if (!_ready || _frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];

        DrawTexturePro(
            tex,
            {0, 0, (float)tex.width, (float)tex.height},
            {0, 0, (float)width, (float)height},
            {0, 0},
            0,
            WHITE);
    }

    // Draw fullscreen avec parallaxe
    void drawFullscreen(int w, int h, float offsetX, float offsetY)
    {
        if (!_ready || _frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];

        float zoom = 1.1f;

        float drawW = w * zoom;
        float drawH = h * zoom;

        Rectangle src{
            0, 0,
            (float)tex.width,
            (float)tex.height};

        Rectangle dst{
            (w - drawW) * 0.5f + offsetX,
            (h - drawH) * 0.5f + offsetY,
            drawW,
            drawH};

        DrawTexturePro(tex, src, dst, {0, 0}, 0, WHITE);
    }
};