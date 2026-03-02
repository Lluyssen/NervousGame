#pragma once

#include <SDL2/SDL_image.h>

struct Sprite 
{
    SDL_Texture* texture;
    int frameW = 32;
    int frameH = 32;
    int frame = 0;
};