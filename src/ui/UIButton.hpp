#pragma once

#include "raylib.h"
#include <string>
#include <memory>
#include "animation/UIAnimation.hpp"

class UIButton
{
private:

    Rectangle _baseRect;
    Rectangle _drawRect;

    std::string _text;

    bool _hover = false;

    std::unique_ptr<UIAnimation> _animation;

public:

    UIButton(const std::string& text, Rectangle rect)
        : _baseRect(rect), _drawRect(rect), _text(text)
    {}

    void setAnimation(std::unique_ptr<UIAnimation> anim)
    {
        _animation = std::move(anim);
    }

    bool update(float dt)
    {
        Vector2 mouse = GetMousePosition();

        _hover = CheckCollisionPointRec(mouse, _baseRect);

        _drawRect = _baseRect;

        if(_animation)
            _animation->update(*this, dt);

        if (_hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            return true;

        return false;
    }

    void draw()
    {
        drawDefault();
    }

    void drawDefault()
    {
        DrawRectangleRounded(_drawRect,0.3f,8,Color{0,0,0,120});
        DrawRectangleRoundedLines(_drawRect,0.3f,8,2,GRAY);

        int fs = 28;

        int tw = MeasureText(_text.c_str(), fs);

        DrawText(
            _text.c_str(),
            _drawRect.x + (_drawRect.width - tw)/2,
            _drawRect.y + (_drawRect.height - fs)/2,
            fs,
            _hover ? YELLOW : WHITE
        );
    }

    Rectangle& rect() { return _drawRect; }

    const Rectangle& baseRect() const { return _baseRect; }

    const std::string& text() const { return _text; }

    bool hover() const { return _hover; }
};