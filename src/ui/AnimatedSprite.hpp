#pragma once

#include "raylib.h"
#include <vector>
#include <string>

class GameContext;

// Gère un sprite animé constitué d’une séquence de textures.
class AnimatedSprite
{
private:
    std::vector<Texture2D *> _frames;
    int _currentFrame = 0;
    float _timer = 0.f;
    float _frameTime = 0.05f;
    bool _ready = false;

public:
    AnimatedSprite() = default;

    // Chargement classique (tout en une fois)
    void load(GameContext &ctx, const std::string &basePath, int frameCount, float frameTime = 0.05f)
    {
        _frames.clear();
        _frameTime = frameTime;

        for (int i = 0; i < frameCount; i++)
        {
            std::string path = basePath + std::to_string(i) + ".png";
            Texture2D &tex = ctx.loadTexture(path);
            _frames.push_back(&tex);
        }
        _ready = true;
    }

    // Charge une seule frame (utilisé pour le loading progressif)
    void loadFrame(GameContext &ctx, const std::string &basePath, int index)
    {
        std::string path = basePath + std::to_string(index) + ".png";
        Texture2D &tex = ctx.loadTexture(path);
        _frames.push_back(&tex);
    }

    // Appelé lorsque toutes les frames sont chargées
    void finalize(float frameTime)
    {
        _frameTime = frameTime;
        _ready = true;
        _currentFrame = 0;
        _timer = 0.f;
    }

    // Mise à jour de l’animation
    void update(float dt)
    {
        if (!_ready || _frames.empty())
            return;

        _timer += dt;
        if (_timer >= _frameTime)
        {
            _currentFrame++;
            if (_currentFrame >= (int)_frames.size())
                _currentFrame = 0;
            _timer = 0.f;
        }
    }

    // Dessin fullscreen
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

    // Dessin fullscreen avec parallaxe
    void drawFullscreen(int w, int h, float offsetX, float offsetY)
    {
        if (!_ready || _frames.empty())
            return;
        Texture2D &tex = *_frames[_currentFrame];
        float zoom = 1.1f;
        float drawW = w * zoom;
        float drawH = h * zoom;
        Rectangle src{0, 0, (float)tex.width, (float)tex.height};
        Rectangle dst{
            (w - drawW) * 0.5f + offsetX,
            (h - drawH) * 0.5f + offsetY,
            drawW,
            drawH};
        DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, WHITE);
    }
};