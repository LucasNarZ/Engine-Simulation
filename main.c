#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "setup-opengl/setup-opengl.h"
#include "draw/draw.h"
#include "physics/physics.h"
#include "vectors/vectors.h"

#define CIRCLE_SEGMENTS 50
void localToWorld(Body* b, float localX, float localY, float* outX, float* outY){
    float c = cosf(b->angle);
    float s = sinf(b->angle);
    *outX = b->posX + localX * c - localY * s;
    *outY = b->posY + localX * s + localY * c;
}

void velocityAtPoint(Body* b, float localX, float localY, float* outVX, float* outVY){
    float c = cosf(b->angle);
    float s = sinf(b->angle);
    float rx = localX * c - localY * s;
    float ry = localX * s + localY * c;
    *outVX = b->velX - b->angularVel * ry;
    *outVY = b->velY + b->angularVel * rx;
}

void solveHingeJoint(Body* A, Body* B, float pivotLocalX_A, float pivotLocalY_A, float pivotLocalX_B, float pivotLocalY_B){
    float pivotWorldAx, pivotWorldAy, pivotWorldBx, pivotWorldBy;
    localToWorld(A, pivotLocalX_A, pivotLocalY_A, &pivotWorldAx, &pivotWorldAy);
    localToWorld(B, pivotLocalX_B, pivotLocalY_B, &pivotWorldBx, &pivotWorldBy);

    float vAx, vAy, vBx, vBy;
    velocityAtPoint(A, pivotLocalX_A, pivotLocalY_A, &vAx, &vAy);
    velocityAtPoint(B, pivotLocalX_B, pivotLocalY_B, &vBx, &vBy);

    float relVX = vAx - vBx;
    float relVY = vAy - vBy;

    float rxA = pivotWorldAx - A->posX;
    float ryA = pivotWorldAy - A->posY;
    float rxB = pivotWorldBx - B->posX;
    float ryB = pivotWorldBy - B->posY;

    float mA_inv = 1.0f / A->mass;
    float mB_inv = 1.0f / B->mass;
    float iA_inv = 1.0f / A->momentOfInertia;
    float iB_inv = 1.0f / B->momentOfInertia;

    float k11 = mA_inv + mB_inv + ryA*ryA * iA_inv + ryB*ryB * iB_inv;
    float k12 = -rxA*ryA * iA_inv - rxB*ryB * iB_inv;
    float k21 = k12;
    float k22 = mA_inv + mB_inv + rxA*rxA * iA_inv + rxB*rxB * iB_inv;

    float det = k11*k22 - k12*k21;
    if(fabs(det) < 1e-6f) return;

    float invDet = 1.0f/det;
    float lambdaX = -invDet * (k22*relVX - k12*relVY);
    float lambdaY = -invDet * (-k21*relVX + k11*relVY);

    A->velX += lambdaX * mA_inv;
    A->velY += lambdaY * mA_inv;
    A->angularVel += iA_inv * (rxA * lambdaY - ryA * lambdaX);

    B->velX -= lambdaX * mB_inv;
    B->velY -= lambdaY * mB_inv;
    B->angularVel -= iB_inv * (rxB * lambdaY - ryB * lambdaX);
}


void positionalCorrection(Body* A, Body* B, float pivotLocalX_A, float pivotLocalY_A, float pivotLocalX_B, float pivotLocalY_B){
    float pivotWorldAx, pivotWorldAy, pivotWorldBx, pivotWorldBy;
    localToWorld(A, pivotLocalX_A, pivotLocalY_A, &pivotWorldAx, &pivotWorldAy);
    localToWorld(B, pivotLocalX_B, pivotLocalY_B, &pivotWorldBx, &pivotWorldBy);

    float dx = pivotWorldAx - pivotWorldBx;
    float dy = pivotWorldAy - pivotWorldBy;

    float mA_inv = (A->isStatic) ? 0.0f : 1.0f / A->mass;
    float mB_inv = (B->isStatic) ? 0.0f : 1.0f / B->mass;
    float iA_inv = (A->isStatic) ? 0.0f : 1.0f / A->momentOfInertia;
    float iB_inv = (B->isStatic) ? 0.0f : 1.0f / B->momentOfInertia;

    float totalMass = mA_inv + mB_inv;
    if(totalMass == 0.0f) return;

    float rAx = pivotWorldAx - A->posX;
    float rAy = pivotWorldAy - A->posY;
    float rBx = pivotWorldBx - B->posX;
    float rBy = pivotWorldBy - B->posY;

    float percent = 0.8f;
    float slop = 0.01f;

    float errLen = sqrtf(dx*dx + dy*dy);
    if(errLen < slop) return;

    float correctionX = (percent * dx) / totalMass;
    float correctionY = (percent * dy) / totalMass;

    if(!A->isStatic){
        A->posX -= correctionX * mA_inv;
        A->posY -= correctionY * mA_inv;
    }
    if(!B->isStatic){
        B->posX += correctionX * mB_inv;
        B->posY += correctionY * mB_inv;
    }

    float angularCorrectionA = (rAx * correctionY - rAy * correctionX) * iA_inv;
    float angularCorrectionB = (rBx * correctionY - rBy * correctionX) * iB_inv;
    if(!A->isStatic) A->angle -= angularCorrectionA;
    if(!B->isStatic) B->angle += angularCorrectionB;
}


int main(){
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window=glfwCreateWindow(640,640,"Bouncing Ball",NULL,NULL);
    if(!window){glfwTerminate();return -1;}
    glfwMakeContextCurrent(window);
    if(glewInit()!=GLEW_OK){printf("GLEW init failed\n");return -1;}

    GLuint vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSrc,NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSrc,NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    float gravity = -0.6f;
    float lastFrame = 0.00f;
    float deltaTime = 0.00f;

    Shape *circle = createShape(CIRCLE_SEGMENTS + 2, GL_TRIANGLE_FAN);
    Body *circleBody = createBody(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.0f, 1, circle);
    Shape *rect = createShape(4, GL_TRIANGLE_FAN);
    Body *rectBody = createBody(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.7f, 3.1415f / 1.42f, 0, rect);
    while(!glfwWindowShouldClose(window)){
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float pivotX, pivotY, comX, comY;
        localToWorld(rectBody, 0.0f, rectBody->height / 2.0f, &pivotX, &pivotY);
        localToWorld(rectBody, 0.0f, 0.0f, &comX, &comY);
        float forceX = 0.0f;
        float forceY = rectBody->mass * gravity; // gravity negativo já
        float rX = comX - pivotX;
        float rY = comY - pivotY;
        float torque = rX * forceY - rY * forceX;
        rectBody->torque += torque;

        updatePhysics(circleBody, deltaTime, gravity);
        updatePhysics(rectBody, deltaTime, gravity);

        rectBody->velX *= 0.98f;
        rectBody->velY *= 0.98f;
        // rectBody->angularVel *= 0.98f;

        for(int i=0; i<100; i++){
            // solveHingeJoint(rectBody, circleBody, 0.0f, rectBody->width/2.0f, 0.0f, -circleBody->height/2.0f);
        }

        for(int i=0; i<100; i++){
            positionalCorrection(rectBody, circleBody, 0.0f, rectBody->width/2.0f, 0.0f, -circleBody->height/2.0f);
        }

        integratePositions(rectBody, deltaTime);

        generateCircleVertices(circle->vertices, CIRCLE_SEGMENTS, 0.1f, circleBody->posX, circleBody->posY);
        updateShapeVertices(circle);
        drawShape(circle, shaderProgram);

        generateRectangleVertices(rect->vertices, rectBody->posX, rectBody->posY, rectBody->width, rectBody->height, rectBody->angle);
        updateShapeVertices(rect);
        drawShape(rect, shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanupShape(circle);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
