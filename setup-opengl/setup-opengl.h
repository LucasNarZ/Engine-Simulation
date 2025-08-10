#ifndef LEW_H
#define LEW_H

#include <GLFW/glfw3.h>

extern const char* vertexShaderSrc;
extern const char* fragmentShaderSrc;

void checkCompileErrors(GLuint shader, const char* type);
GLuint compileShader(const char* source, GLenum type);
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc);

#endif
