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
        deleteTexture();
        _width = width;
        _height = height;
        glGenTextures(1, &_textureId);
        glBindTexture (GL_TEXTURE_2D, _textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

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

        //printf("TOTAL DAMAGES:%lu\n",damages.size());
        // If texture is not EGL
        while(!damages.empty())
        {
            //WOpenGL::checkGLError("Error updating damage region.");
            Rect damage = damages.front();
            glPixelStorei(GL_UNPACK_ROW_LENGTH,_width);
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)damage.x);
            glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)damage.y);

            ////printf("%i %i\n",width,height);

            if(damage.x < 0)
                damage.x = 0;
            if(damage.y < 0)
                damage.y = 0;
            if(damage.width + damage.x > _width || damage.width < 0)
                damage.width = _width - damage.x;
            if(damage.height + damage.y > _height || damage.height < 0)
                damage.height = _height - damage.y;




            //printf("%i %i %i %i\n",damage.x,damage.y,damage.width,damage.height);

            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                (GLint)damage.x,
                (GLint)damage.y,
                (GLsizei)damage.width,
                (GLsizei)damage.height,
                GL_BGRA,
                GL_UNSIGNED_BYTE,
                data);
            WOpenGL::checkGLError("Error updating texture region.");










            damages.pop();
        }


        /*
        glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        0,
                        0,
                        width,
                        height,
                        GL_BGRA,
                        GL_UNSIGNED_BYTE,
                        data);
        */
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
    }

    //glBindTexture(GL_TEXTURE_2D,0);
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

