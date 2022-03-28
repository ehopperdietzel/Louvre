#include "WOpenGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>

using namespace WaylandPlus;

char *WOpenGL::openShader(const char *fname)
{
    FILE *fp;
    fp = fopen(fname, "r");

    if (fp == NULL)
    {
        printf("Error while opening the file:%s.\n",fname);
        exit(EXIT_FAILURE);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    char *data = (char*)malloc(fileSize+1);

    fread(data,fileSize,1,fp);

    data[fileSize] = '\0';

    fclose(fp);
    return data;
}

void WOpenGL::checkGLError(const char *msg)
{
    GLenum errCode;

    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR: %i %s\n",errCode,msg);
        exit(-1);
    }
}

GLuint WOpenGL::getMaxTextureUnits()
{
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&maxUnits);
    return maxUnits;
}

GLuint WOpenGL::LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled)
    {
        GLint infoLen = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

        GLchar errorLog[infoLen];

        glGetShaderInfoLog(shader, infoLen, &infoLen, errorLog);

        printf("%s",errorLog);

        glDeleteShader(shader);

        return 0;
    }

    return shader;
}
