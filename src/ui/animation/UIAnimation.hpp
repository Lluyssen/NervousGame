#pragma once

class UIButton;

class UIAnimation
{
public:
    virtual ~UIAnimation() = default;

virtual void reset(void) {}

    virtual void update(class UIButton&, float) {}

    virtual void draw(class UIButton&) {}
};