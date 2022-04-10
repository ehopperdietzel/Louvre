#include "WTexture.h"
#include <stdio.h>
#include <WOpenGL.h>

using namespace WaylandPlus;

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

WTexture::WTexture(GLuint __textureUnit)
{
    _textureUnit = __textureUnit;
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
}

void WTexture::setData(int width, int height, void *data, Type textureType)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit());

    // Prevent gen a new texture if the buffer size is the same
    if(width != _width || height != _height || textureType == Type::EGL)
    {

        //printf("Resize: W %i, H %i\n",width,height);
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
            glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, _format, GL_UNSIGNED_BYTE, data);
            WOpenGL::checkGLError("Error creating texture from shared memory.");
        }
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _textureId);

        // If texture is not EGL
        while(!damages.empty())
        {

            Rect damage = damages.front();

            printf("%i %i\n",width,height);
            printf("Damage X:%i,Y:%i,W:%i,H:%i\n",damage.x,damage.y,damage.width,damage.height);


            if(damage.x < 0)
                damage.x = 0;
            if(damage.y < 0)
                damage.y = 0;
            if(damage.width + damage.x > _width || damage.width < 0)
                damage.width = _width - damage.x;
            if(damage.height + damage.y > _height || damage.height < 0)
                damage.height = _height - damage.y;

            if(damage.x + damage.width > _width || damage.y + damage.height > _height)
            {
                damages.pop();
                continue;
            }


            glPixelStorei(GL_UNPACK_ROW_LENGTH,_width);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)damage.x);
            glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)damage.y);

            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                (GLint)damage.x,
                (GLint)damage.y,
                (GLsizei)damage.width,
                (GLsizei)damage.height,
                _format,
                GL_UNSIGNED_BYTE,
                data);

            WOpenGL::checkGLError("Error updating texture region.");

            damages.pop();
        }

        /*

        while(!damages.empty())
            damages.pop();
        */

        glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,0);

        /*
        glTexSubImage2D(GL_TEXTURE_2D,
            0,
            0,
            0,
            width,
            height,
            _format,
            GL_UNSIGNED_BYTE,
            data);
        */
        WOpenGL::checkGLError("Error updating texture region.");
    }

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
