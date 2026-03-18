#pragma once

#include "raylib.h"
#include <vector>
#include <math.h>

struct Particle
{
    Vector2 pos;
    Vector2 vel;
    float life;
    float maxLife;
    float size;
};

static constexpr int MAX_PARTICLES = 60;