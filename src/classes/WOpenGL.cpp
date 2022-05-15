#include "WOpenGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <WRect.h>
#include <WTexture.h>
#include <SOIL/SOIL.h>
#include <WOutput.h>

using namespace Wpp;

WOpenGL::WOpenGL()
{
    GLuint vertexShader,fragmentShader,programObject;

    // Open the vertex/fragment shaders
    GLchar *vShaderStr = WOpenGL::openShader("/opt/MyCompositor/res/shaders/Vertex.glsl");
    GLchar *fShaderStr = WOpenGL::openShader("/opt/MyCompositor/res/shaders/Fragment.glsl");

    // Load the vertex/fragment shaders
    vertexShader = WOpenGL::LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = WOpenGL::LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    free(vShaderStr);
    free(fShaderStr);

    // Create the program object
    programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vertexPosition");

    // Link the program
    glLinkProgram(programObject);

    GLint linked;

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteProgram(programObject);
        exit(-1);
    }

    // Enable alpha blending
    glEnable(GL_BLEND);

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

    // Use the program object
    glUseProgram(programObject);

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    screenSizeUniform = glGetUniformLocation(programObject, "screenSize");
    texSizeUniform = glGetUniformLocation(programObject, "texSize");
    srcRectUniform = glGetUniformLocation(programObject, "srcRect");
    dstRectUniform = glGetUniformLocation(programObject, "dstRect");
    activeTextureUniform = glGetUniformLocation(programObject, "tex");
    modeUniform = glGetUniformLocation(programObject, "mode");
    colorUniform = glGetUniformLocation(programObject, "colorRGBA");

}

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

WTexture *WOpenGL::loadTexture(const char *fileName)
{
    int w,h;
    WTexture *texture = new WTexture();
    unsigned char * img = SOIL_load_image(fileName, &w, &h, 0, SOIL_LOAD_RGBA);
    texture->setData(w,h,img);
    SOIL_free_image_data(img);
    return texture;
}


void WOpenGL::drawTexture(WTexture *texture, WRect src, WRect dst)
{
    glActiveTexture(GL_TEXTURE0 + texture->textureUnit());
    glUniform1i(modeUniform,0);
    glUniform1i(activeTextureUniform,texture->textureUnit());
    glBindTexture(GL_TEXTURE_2D,texture->textureId());
    glUniform2f(texSizeUniform,texture->size().w(), texture->size().h());
    glUniform4f(srcRectUniform,src.x(), src.y(), src.w(), src.h());
    glUniform4f(dstRectUniform,dst.x(), dst.y(), dst.w(), dst.h());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void WOpenGL::drawColor(WRect dst, float r, float g, float b, float a)
{
    glUniform4f(colorUniform,r,g,b,a);
    glUniform1i(modeUniform,1);
    glUniform4f(dstRectUniform,dst.x(), dst.y(), dst.w(), dst.h());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void WOpenGL::setViewport(WRect rect, Int32 scale)
{
    // Set the viewport
    glViewport(0, 0, rect.w(), rect.h());

    // Set screen size
    glUniform2f(
        screenSizeUniform,
        rect.w() / scale,
        rect.h() / scale);
}

void WOpenGL::viewportToOutput()
{
    if(p_output)
        setViewport(WRect(WPoint(0,0),p_output->size), p_output->getOutputScale());
}


