#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "setup-opengl/setup-opengl.h"

#define CIRCLE_SEGMENTS 50

void generateCircleVertices(float* vertices, int segments, float radius, float x, float y){
    vertices[0] = x;// center x
    vertices[1] = y; // center y
    for(int i=0; i<=segments; i++){
        float angle = 2.0f * 3.1415926f * i / segments;
        vertices[2+2*i] = cosf(angle)*radius + x; 
        vertices[3+2*i] = sinf(angle)*radius + y; 
    }
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

    GLuint shaderProgram=createShaderProgram(vertexShaderSrc,fragmentShaderSrc);
    glUseProgram(shaderProgram);

    int vertexCount = CIRCLE_SEGMENTS+2;
    float vertices[(CIRCLE_SEGMENTS+2)*2]; // center + segments + repeat first point

    generateCircleVertices(vertices,CIRCLE_SEGMENTS,0.1f, 0.00f, 0.00f);

    GLuint VAO,VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    float transformedVertices[(CIRCLE_SEGMENTS+2)*2];

    float gravity = -0.9f;

    float posX = 0.0f, posY = 0.0f; // posição atual da bola
    float velX = 0.0f, velY = 0.0f; // velocidade atual da bola (unidades por segundo)
                                    //
    float lastFrame = 0.00f;
    float deltaTime = 0.00f;

    while(!glfwWindowShouldClose(window)){
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        glClearColor(0.1f,0.2f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        velY += gravity * deltaTime;
        
        posX += velX * deltaTime; 
        posY += velY * deltaTime; 
                                  
        if(posY <= -1 + 0.1f){
            velY = -velY - 0.1f;
            posY = -1 + 0.1f;
        }    
    

        generateCircleVertices(transformedVertices, CIRCLE_SEGMENTS, 0.1f, posX, posY);

        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(vertices), transformedVertices);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN,0,vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
