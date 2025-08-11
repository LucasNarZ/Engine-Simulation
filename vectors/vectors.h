#ifndef Vectors
#define Vectors

typedef struct Vec2{
    float x;
    float y;
} Vec2;


double cross2(Vec2 a, Vec2 b);

Vec2 vec2_add(Vec2 a, Vec2 b); 
Vec2 vec2_sub(Vec2 a, Vec2 b); 
Vec2 vec2_scale(Vec2 v, double s); 

#endif
