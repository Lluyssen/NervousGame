#pragma once
#include "../ecs/ECS.hpp"


struct Position
{
    float x;
    float y;
};

struct Velocity
{
    float vx;
    float vy;
};

struct Heal { int hp; };
struct Mana { int mp; };