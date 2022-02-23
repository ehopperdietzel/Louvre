#include "WOpenGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

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
