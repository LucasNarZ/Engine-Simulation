#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "setup-opengl/setup-opengl.h"
#include "draw/draw.h"
#include "physics/physics.h"

#define CIRCLE_SEGMENTS 50

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


    float gravity = -0.1f;

    float lastFrame = 0.00f;
    float deltaTime = 0.00f;

    Shape *circle = createShape(CIRCLE_SEGMENTS + 2, GL_TRIANGLE_FAN);
    Body *circleBody = createBody(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.0f, 1, circle);
    Shape *rect = createShape(4, GL_TRIANGLE_FAN);
    Body *rectBody = createBody(10.0f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.4f, 3.1415f / 2, 0, rect);
    while(!glfwWindowShouldClose(window)){
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        updatePhysics(circleBody, deltaTime, gravity);
        updatePhysics(rectBody, deltaTime, gravity);

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
