#pragma once

#include "raylib.h"
#include <vector>
#include <string>
#include <algorithm>

class PetitMenu
{
private:
    Texture2D *_bg = nullptr;
    Vector2 _pos{0, 0};
    std::vector<std::string> _lines;

    bool _visible = false;

    float _textureScale = 2.f;
    float _textScale = 1.f;

    float _rotation = 0.f;

public:
    void init(Texture2D &background)
    {
        _bg = &background;
    }

    void setTextureScale(float f)
    {
        _textureScale = f;
    }

    void setTextScale(float f)
    {
        _textScale = f;
    }

    void show(Vector2 pos, const std::vector<std::string> &lines)
    {
        _pos = pos;
        _lines = lines;
        _visible = true;
    }

    void hide()
    {
        _visible = false;
    }

    void draw(int screenW, int screenH)
    {
        if (!_visible || !_bg)
            return;

        float uiScale = screenW / 1920.0f;

        int fontSize = 20 * uiScale * _textScale;
        int padding = 20 * uiScale * _textScale;

        int textWidth = 0;

        for (const auto &line : _lines)
        {
            textWidth = std::max(
                textWidth,
                MeasureText(line.c_str(), fontSize));
        }

        int textHeight = _lines.size() * (fontSize + 4);

        float w = (textWidth + padding * 2) * _textureScale;
        float h = (textHeight + padding * 2) * _textureScale;

        // centre initial
        float cx = _pos.x;
        float cy = _pos.y - h * 0.6f;

        float halfW = w * 0.5f;
        float halfH = h * 0.5f;

        // empêcher sortie écran
        cx = std::clamp(cx, halfW, screenW - halfW);
        cy = std::clamp(cy, halfH, screenH - halfH);

        // rotation vers souris
        Vector2 mouse = GetMousePosition();

        float dx = mouse.x - cx;

        _rotation = std::clamp(dx * 0.02f, -6.f, 6.f);

        Rectangle src{
            0, 0,
            (float)_bg->width,
            (float)_bg->height};

        Rectangle dst{
            cx, cy,
            w, h};

        DrawTexturePro(
            *_bg,
            src,
            dst,
            {halfW, halfH},
            _rotation,
            WHITE);

        // texte centré
        float textY = cy - textHeight * 0.5f;

        for (const auto &line : _lines)
        {
            int lineWidth = MeasureText(line.c_str(), fontSize);

            float textX = cx - lineWidth * 0.5f;

            DrawText(line.c_str(), textX, textY, fontSize, BLACK);

            textY += fontSize + 4;
        }
    }
};