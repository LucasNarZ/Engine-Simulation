#include "./physics.h"
#include <stdlib.h>

Body *createBody(float mass, float initialPosX, float initialPosY, float initialVelX, float initialVelY, float initialAccX, float initialAccY, float width, float height, float angle, int isStatic, Shape *shape){
    Body *body = malloc(sizeof(Body));
    body->mass = mass;
    body->posX = initialPosX;
    body->posY = initialPosY;
    body->velX = initialVelX;
    body->velY = initialVelY;
    body->accX = initialAccX;
    body->accY = initialAccY;
    body->angle = angle;
    body->angularVel = 0.0f;
    body->angularAcc = 0.0f;
    body->momentOfInertia = (mass / 12.0f) * (width * width + height * height);
    body->torque = 0.0f;
    body->width = width;
    body->height = height;
    body->isStatic = isStatic;
    body->shape = shape;

    return body;
}

void updatePhysics(Body *body, float deltaTime, float gravity){
    body->accY += gravity;
    body->velX += body->accX * deltaTime;
    body->velY += body->accY * deltaTime;
    if(!body->isStatic){
        body->posX += body->velX * deltaTime;
        body->posY += body->velY * deltaTime;
    }

    body->angularAcc = body->torque / body->momentOfInertia;

    // Atualiza velocidade angular e ângulo
    body->angularVel += body->angularAcc * deltaTime;
    body->angle += body->angularVel * deltaTime;

    if(body->posY <= -1 + body->height/2){
        body->posY = -1 + body->height/2;
        body->velY = -body->velY;
    }

    if(body->posY >= 1 - body->height/2){
        body->posY = 1 - body->height/2;
        body->velY = -body->velY;
    }

    if(body->posX <= -1 + body->width/2){
        body->posX = -1 + body->width/2;
        body->velX = -body->velX;
    }

    if(body->posX >= 1 - body->width/2){
        body->posX = 1 - body->width/2;
        body->velX = -body->velX;
    }

    body->torque = 0.0f;
}
