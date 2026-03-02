#pragma once

#include <SDL2/SDL.h>

struct SpriteDef 
{
    SDL_Rect src;
};

namespace UISprites 
{
    inline const SpriteDef Play     {{ 210,  45, 117, 45 }};
    inline const SpriteDef Options {{ 178, 266, 186, 46 }};
    inline const SpriteDef Exit {{ 211, 322, 118, 44 }};
}