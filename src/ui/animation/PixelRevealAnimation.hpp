#pragma once

#include "UIAnimation.hpp"
#include "../UIButton.hpp"

class PixelRevealAnimation : public UIAnimation
{
private:

    float reveal = 0.0f;

public:

    void update(UIButton&, float dt) override
    {
        reveal += dt * 2.0f;

        if(reveal > 1.0f)
            reveal = 1.0f;
    }

    void draw(UIButton& btn) override
    {
        Rectangle r = btn.rect();

        int block = 6;

        Vector2 center =
        {
            r.x + r.width/2,
            r.y + r.height/2
        };

        for(int x=0;x<r.width;x+=block)
        {
            for(int y=0;y<r.height;y+=block)
            {
                float px = r.x + x;
                float py = r.y + y;

                Vector2 p =
                {
                    px + block/2,
                    py + block/2
                };

                float dist = Vector2Distance(center,p);

                float maxDist = r.width;

                if(dist < reveal * maxDist)
                {
                    DrawRectangle(px,py,block,block,Color{0,0,0,120});
                }
            }
        }

        DrawRectangleRoundedLines(r,0.3f,8,2,GRAY);

        int fs = 28;

        int tw = MeasureText(btn.text().c_str(), fs);

        DrawText(
            btn.text().c_str(),
            r.x + (r.width - tw)/2,
            r.y + (r.height - fs)/2,
            fs,
            btn.hover() ? YELLOW : WHITE
        );
    }
};