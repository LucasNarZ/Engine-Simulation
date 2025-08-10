#ifndef DRAW
#define DRAW

typedef struct Shape {
    float *vertices;
    GLuint VAO;
    GLuint VBO;
    int vertexCount;
    GLenum drawMode;
} Shape;

void generateCircleVertices(float* vertices, int segments, float radius, float x, float y);
void generateRectangleVertices(float* vertices, float centerX, float centerY, float width, float height, float angle);
Shape *createShape(int vertexCount, GLenum drawMode);
void updateShapeVertices(Shape* shape);
void drawShape(Shape* shape, GLuint shaderProgram);
void cleanupShape(Shape* shape);

#endif
