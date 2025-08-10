#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../draw/draw.h"

#ifndef PHYSICS
#define PHYSICS

typedef struct Body {
    float mass;
    float posX, posY;
    float velX, velY;
    float accX, accY;
    float angle;           // ângulo atual (em radianos)
    float angularVel;      // velocidade angular (rad/s)
    float angularAcc;      // aceleração angular (rad/s²)
    float momentOfInertia; // momento de inércia do corpo (constante)
    float torque;    
    float width, height;
    int isStatic;
    Shape *shape;
} Body;

Body *createBody(float mass, float initialPosX, float initialPosY, float initialVelX, float initialVelY, float initialAccX, float initialAccY, float width, float height, float angle, int isStatic, Shape *shape);

void updatePhysics(Body *body, float deltaTime, float gravity);

#endif
