#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>

const char* vertexShaderSrc="#version 330 core\nlayout(location=0) in vec2 aPos;void main(){gl_Position=vec4(aPos,0.0,1.0);}";
const char* fragmentShaderSrc="#version 330 core\nout vec4 FragColor;void main(){FragColor=vec4(0.2,0.7,0.9,1.0);}";

void checkCompileErrors(GLuint shader,const char* type){
    GLint success;GLchar infoLog[1024];
    if(strcmp(type,"PROGRAM")!=0){
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
        if(!success){
            glGetShaderInfoLog(shader,1024,NULL,infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n",type,infoLog);
        }
    } else{
        glGetProgramiv(shader,GL_LINK_STATUS,&success);
        if(!success){
            glGetProgramInfoLog(shader,1024,NULL,infoLog);
            printf("ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n",type,infoLog);
        }
    }
}

GLuint compileShader(const char* source, GLenum type){
    GLuint shader=glCreateShader(type);
    glShaderSource(shader,1,&source,NULL);
    glCompileShader(shader);
    checkCompileErrors(shader,type==GL_VERTEX_SHADER?"VERTEX":"FRAGMENT");
    return shader;
}

GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc){
    GLuint vertexShader=compileShader(vertexSrc,GL_VERTEX_SHADER);
    GLuint fragmentShader=compileShader(fragmentSrc,GL_FRAGMENT_SHADER);
    GLuint program=glCreateProgram();
    glAttachShader(program,vertexShader);
    glAttachShader(program,fragmentShader);
    glLinkProgram(program);
    checkCompileErrors(program,"PROGRAM");
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}


