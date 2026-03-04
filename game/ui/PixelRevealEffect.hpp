#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
#include <cmath>

struct PixelBlock
{
    SDL_Rect src;
    SDL_Rect dst;
    float revealTime;
};

class PixelRevealEffect
{
private:
    std::vector<PixelBlock> blocks;
    std::mt19937 rng{std::random_device{}()};

public:
    void generate(const SDL_Rect &src, const SDL_Rect &dst, int blockSize = 4)
    {
        blocks.clear();

        float scale = std::min(
            float(dst.w) / src.w,
            float(dst.h) / src.h);

        int drawW = std::round(src.w * scale);
        int drawH = std::round(src.h * scale);

        int offsetX = dst.x + (dst.w - drawW) / 2;
        int offsetY = dst.y + (dst.h - drawH) / 2;

        int centerX = src.w / 2;
        int centerY = src.h / 2;

        std::map<int, std::vector<PixelBlock>> rings;

        for (int y = 0; y < src.h + src.h; y += blockSize)
        {
            for (int x = 0; x < src.w + src.w; x += blockSize)
            {
                int bw = std::min(blockSize, src.w - x);
                int bh = std::min(blockSize, src.h - y);

                PixelBlock b;

                b.src = {src.x + x, src.y + y, bw, bh};

                int x1 = offsetX + int((x)*scale);
                int y1 = offsetY + int((y)*scale);
                int x2 = offsetX + int((x + bw) * scale);
                int y2 = offsetY + int((y + bh) * scale);

                b.dst =
                    {
                        x1,
                        y1,
                        x2 - x1,
                        y2 - y1};

                float dx = (x + bw / 2) - centerX;
                float dy = (y + bh / 2) - centerY;

                float dist2 = dx * dx + dy * dy;
                int ring = int(dist2 / (blockSize * blockSize));

                rings[ring].push_back(b);
            }
        }

        for (auto &[ring, vec] : rings)
        {
            std::shuffle(vec.begin(), vec.end(), rng);

            for (auto &b : vec)
            {
                b.revealTime = float(ring) / rings.size();
                blocks.push_back(b);
            }
        }
    }
    void render(SDL_Renderer *renderer, SDL_Texture *texture, float t)
    {
        for (const auto &b : blocks)
        {
            if (t >= b.revealTime)
                SDL_RenderCopy(renderer, texture, &b.src, &b.dst);
        }
    }
};