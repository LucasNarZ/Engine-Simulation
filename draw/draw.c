#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "draw.h"

void generateCircleVertices(float* vertices, int segments, float radius, float x, float y){
    vertices[0] = x;// center x
    vertices[1] = y; // center y
    for(int i=0; i<=segments; i++){
        float angle = 2.0f * 3.1415926f * i / segments;
        vertices[2+2*i] = cosf(angle)*radius + x; 
        vertices[3+2*i] = sinf(angle)*radius + y; 
    }
}

void generateRectangleVertices(float* vertices, float centerX, float centerY, float width, float height, float angle){
    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    float local[8] = {
        -halfW, -halfH, // bottom-left
         halfW, -halfH, // bottom-right
         halfW,  halfH, // top-right
        -halfW,  halfH  // top-left
    };

    for(int i=0; i<4; i++){
        float x = local[2*i];
        float y = local[2*i+1];
        float rotatedX = x * cosf(angle) - y * sinf(angle);
        float rotatedY = x * sinf(angle) + y * cosf(angle);
        vertices[2*i] = rotatedX + centerX;
        vertices[2*i+1] = rotatedY + centerY;
    }
}

Shape *createShape(int vertexCount, GLenum drawMode){
    GLuint VAO, VBO;

    Shape *shape = malloc(sizeof(Shape));
    shape->vertices = malloc(vertexCount * 2 * sizeof(float));
    shape->vertexCount = vertexCount;
    shape->drawMode = drawMode;
    shape->VAO = VAO;
    shape->VBO = VBO;

    glGenVertexArrays(1, &shape->VAO);
    glGenBuffers(1, &shape->VBO);

    glBindVertexArray(shape->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, shape->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 2 * sizeof(float), shape->vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

    return shape;
}

void updateShapeVertices(Shape* shape) {
    glBindBuffer(GL_ARRAY_BUFFER, shape->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, shape->vertexCount * 2 * sizeof(float), shape->vertices);
}

void drawShape(Shape* shape, GLuint shaderProgram){
    glUseProgram(shaderProgram);
    glBindVertexArray(shape->VAO);
    glDrawArrays(shape->drawMode, 0, shape->vertexCount);
    glBindVertexArray(0);
}

void cleanupShape(Shape* shape){
    glDeleteVertexArrays(1,&shape->VAO);
    glDeleteBuffers(1,&shape->VBO);
}

