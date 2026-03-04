#pragma once
#include <vector>
#include <SDL2/SDL_rect.h>

class UILayout
{
public:
    enum class Direction
    {
        Vertical,
        Horizontal
    };

private:
    Direction direction = Direction::Vertical;

    int spacing = 10;

    int elementWidth = 200;
    int elementHeight = 60;

    std::vector<SDL_Rect> rects;

public:
    void setDirection(Direction d)
    {
        direction = d;
    }

    void setSpacing(int s)
    {
        spacing = s;
    }

    void setElementSize(int w, int h)
    {
        elementWidth = w;
        elementHeight = h;
    }

    const std::vector<SDL_Rect> &compute(int screenW, int screenH, int count)
    {
        rects.clear();
        rects.reserve(count);

        if (direction == Direction::Vertical)
        {
            int totalH = count * elementHeight + (count - 1) * spacing;
            int startY = (screenH - totalH) / 2;
            int centerX = screenW / 2;

            for (int i = 0; i < count; i++)
            {
                rects.push_back(
                    {centerX - elementWidth / 2,
                     startY + i * (elementHeight + spacing),
                     elementWidth,
                     elementHeight});
            }
        }
        else
        {
            int totalW = count * elementWidth + (count - 1) * spacing;
            int startX = (screenW - totalW) / 2;
            int centerY = screenH / 2;

            for (int i = 0; i < count; i++)
            {
                rects.push_back(
                    {startX + i * (elementWidth + spacing),
                     centerY - elementHeight / 2,
                     elementWidth,
                     elementHeight});
            }
        }

        return rects;
    }
};