#pragma once

#include "UIAnimation.hpp"
#include "../UIButton.hpp"

class ScaleHoverAnimation : public UIAnimation
{
private:

    float scale = 1.0f;

public:

    void update(UIButton& btn, float) override
    {
        float target = btn.hover() ? 1.1f : 1.0f;

        scale += (target - scale) * 0.15f;

        Rectangle& r = btn.rect();
        Rectangle base = btn.baseRect();

        r.width = base.width * scale;
        r.height = base.height * scale;

        r.x = base.x + (base.width - r.width) / 2;
        r.y = base.y + (base.height - r.height) / 2;
    }

    void draw(UIButton& btn) override
    {
        btn.drawDefault();   // IMPORTANT
    }
};