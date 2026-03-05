#pragma once

class UIButton;

class UIAnimation
{
public:

    virtual ~UIAnimation() = default;

    virtual void update(UIButton&, float) = 0;

    virtual void draw(UIButton&) {}
};