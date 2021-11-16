#include "WTexture.h"
#include <stdio.h>
#include <WOpenGL.h>

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
            checkGLError("35");
            printf("EGL\n");
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            checkGLError("Error creating texture from shared memory.");
        }
    }
    else
    {
        /*
        // If texture is not EGL
        if(surface->texture->type() == WTexture::SHM)
        {

            // Updates damaged region
            wl_shm_buffer *shm_buffer = wl_shm_buffer_get(surface->buffer);
            void *data = wl_shm_buffer_get_data(shm_buffer);


            //checkGLError("Error updating damage region.");
            glBindTexture(GL_TEXTURE_2D, surface->texture->textureId());
            glPixelStorei(GL_UNPACK_ROW_LENGTH,surface->texture->width());
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,x);
            glPixelStorei(GL_UNPACK_SKIP_ROWS,y);


            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                x,
                y,
                width,
                height,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data);

            return;
            glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
            glPixelStorei(GL_UNPACK_SKIP_ROWS,0);

        }
        */
    }

    _type = textureType;
    _initialized = true;
}

void WTexture::deleteTexture()
{
    if(_initialized)
    {
        glDeleteTextures(1, &_textureId);
        checkGLError("38");
        _initialized = false;
    }
}

