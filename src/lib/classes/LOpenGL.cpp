#include "LOpenGL.h"
#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include <LRect.h>
#include <LTexture.h>
#include <SOIL/SOIL.h>
#include <LOutput.h>

using namespace Louvre;

LOpenGL::LOpenGL()
{
    GLuint vertexShader,fragmentShader,programObject;

    // Open the vertex/fragment shaders
    GLchar vShaderStr[] =
       "precision lowp float;\
        precision lowp int;\
        attribute vec4 vertexPosition;\
        varying vec2 v_texcoord;\
        void main()\
        {\
            gl_Position = vec4(vertexPosition.xy,0.0,1.0);\
            v_texcoord = vertexPosition.zw;\
        }";

    GLchar fShaderStr[] =
       "precision lowp float;\
        precision lowp int;\
        uniform sampler2D tex;\
        uniform vec2 texSize;\
        uniform vec4 srcRect;\
        uniform int mode;\
        uniform vec4 colorRGBA;\
        varying vec2 v_texcoord;\
        void main()\
        {\
          if(mode == 0)\
          {\
            float x = (srcRect.x)/texSize.x + v_texcoord.x*srcRect.z/texSize.x;\
            float y = (srcRect.y + srcRect.w - (v_texcoord.y*srcRect.w))/texSize.y;\
            gl_FragColor = texture2D(tex,vec2(x,y));\
          }\
          else\
            gl_FragColor = colorRGBA;\
        }";

    // Load the vertex/fragment shaders
    vertexShader = LOpenGL::LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LOpenGL::LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

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
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Use the program object
    glUseProgram(programObject);

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    texSizeUniform = glGetUniformLocation(programObject, "texSize");
    srcRectUniform = glGetUniformLocation(programObject, "srcRect");
    activeTextureUniform = glGetUniformLocation(programObject, "tex");
    modeUniform = glGetUniformLocation(programObject, "mode");
    colorUniform = glGetUniformLocation(programObject, "colorRGBA");

}

char *LOpenGL::openShader(const char *fname)
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

    size_t r = fread(data,fileSize,1,fp);
    (void)r;

    data[fileSize] = '\0';

    fclose(fp);
    return data;
}

void LOpenGL::checkGLError(const char *msg)
{
    GLenum errCode;

    if((errCode = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR: %i %s\n",errCode,msg);
        exit(-1);
    }
}

GLuint LOpenGL::getMaxTextureUnits()
{
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,&maxUnits);
    return maxUnits;
}

GLuint LOpenGL::LoadShader(GLenum type, const char *shaderSrc)
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

        exit(0);

        return 0;
    }

    return shader;
}

LTexture *LOpenGL::loadTexture(const char *fileName)
{
    int w,h;
    LTexture *texture = new LTexture(1);
    unsigned char * img = SOIL_load_image(fileName, &w, &h, 0, SOIL_LOAD_RGBA);
    texture->setData(w,h,img,GL_RGBA,GL_UNSIGNED_BYTE);
    SOIL_free_image_data(img);
    return texture;
}

void LOpenGL::scaleCursor(LTexture *texture, const LRect &src, const LRect &dst)
{
    glEnable(GL_BLEND);
    glScissor(0,0,64,64);
    glViewport(0,0,64,64);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);

    glScissor(dst.x(),dst.y(),dst.w(),dst.h());
    glViewport(dst.x(),dst.y(),dst.w(),dst.h());

    glActiveTexture(GL_TEXTURE0 + texture->unit());
    glUniform1i(modeUniform,0);
    glUniform1i(activeTextureUniform,texture->unit());
    glBindTexture(GL_TEXTURE_2D,texture->id());
    glUniform2f(texSizeUniform,texture->size().w(), texture->size().h());
    glUniform4f(srcRectUniform,src.x(), src.y(), src.w(), src.h());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void LOpenGL::drawTexture(LTexture *texture, const LRect &src, const LRect &dst)
{
    setViewport(dst);
    glActiveTexture(GL_TEXTURE0 + texture->unit());
    glUniform1i(modeUniform,0);
    glUniform1i(activeTextureUniform,texture->unit());
    glBindTexture(GL_TEXTURE_2D,texture->id());
    glUniform2f(texSizeUniform,texture->size().w(), texture->size().h());
    glUniform4f(srcRectUniform,src.x(), src.y(), src.w(), src.h());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    /*
    glEnable(GL_BLEND);
    float r = float(rand() % 10000)/10000.f;
    float g = float(rand() % 10000)/10000.f;
    float b = float(rand() % 10000)/10000.f;
    drawColor(dst,r,g,b,0.5);
    */

}

void LOpenGL::drawColor(const LRect &dst, Float32 r, Float32 g, Float32 b, Float32 a)
{
    setViewport(dst);
    glUniform4f(colorUniform,r,g,b,a);
    glUniform1i(modeUniform,1);
    //glClearColor(r,g,b,a);
    //glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

}

void LOpenGL::setViewport(const LRect &rect)
{
    if(m_output)
    {
        LRect r = rect*m_output->getOutputScale();
        r.setY(m_output->rect(false).h()-r.y()-r.h());
        glScissor(r.x(),r.y(),r.w(),r.h());
        glViewport(r.x(),r.y(),r.w(),r.h());
    }
}

void LOpenGL::clearScreen()
{
    glDisable(GL_BLEND);
    glScissor(0,0,m_output->rect(false).w(),m_output->rect(false).h());
    glViewport(0,0,m_output->rect(false).w(),m_output->rect(false).h());
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
}



