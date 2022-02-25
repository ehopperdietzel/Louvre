#include "WTexture.h"
#include <stdio.h>
#include <WOpenGL.h>

using namespace WaylandPlus;

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

WTexture::WTexture()
{
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
}

void WTexture::setData(int width, int height, void *data, Type textureType)
{

    // Prevent gen a new texture if the buffer size is the same
    if(width != _width || height != _height || textureType == Type::EGL)
    {
        deleteTexture();
        _width = width;
        _height = height;
        glGenTextures(1, &_textureId);
        glBindTexture (GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if(textureType == Type::EGL)
        {

            glEGLImageTargetTexture2DOES(GL_TEXTURE_2D,*(EGLImage*)data);
            WOpenGL::checkGLError("35");
            //printf("EGL\n");
        }
        else
        {
            while(!damages.empty())
                damages.pop();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            WOpenGL::checkGLError("Error creating texture from shared memory.");
        }
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _textureId);

        // If texture is not EGL
        while(!damages.empty())
        {
            //WOpenGL::checkGLError("Error updating damage region.");
            glPixelStorei(GL_UNPACK_ROW_LENGTH,_width);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)damages.front().x);
            glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)damages.front().y);

            //printf("%i %i %i %i\n",damages.front().x,damages.front().y,damages.front().width,damages.front().height);
            //printf("%i %i\n",width,height);


            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                (GLint)damages.front().x,
                (GLint)damages.front().y,
                (GLsizei)damages.front().width,
                (GLsizei)damages.front().height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data);

            WOpenGL::checkGLError("Error updating texture region.");

            damages.pop();
        }

        glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
    }

    glBindTexture(GL_TEXTURE_2D,0);
    _type = textureType;
    _initialized = true;
}

void WTexture::deleteTexture()
{
    if(_initialized)
    {
        glDeleteTextures(1, &_textureId);
        WOpenGL::checkGLError("38");
        _initialized = false;
    }
}

