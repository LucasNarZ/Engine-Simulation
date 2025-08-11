#include "vectors.h"

double cross2(Vec2 a, Vec2 b) {
    return a.x*b.y - a.y*b.x;
}

Vec2 vec2_add(Vec2 a, Vec2 b) { return (Vec2){a.x+b.x, a.y+b.y}; }
Vec2 vec2_sub(Vec2 a, Vec2 b) { return (Vec2){a.x-b.x, a.y-b.y}; }
Vec2 vec2_scale(Vec2 v, double s) { return (Vec2){v.x*s, v.y*s}; }
