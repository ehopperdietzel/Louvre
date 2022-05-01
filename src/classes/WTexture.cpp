#include "WTexture.h"
#include <stdio.h>
#include <WOpenGL.h>
#include <WRect.h>
#include <GLES2/gl2.h>

using namespace Wpp;
using namespace std;

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

WTexture::WTexture(GLuint __textureUnit)
{
    _textureUnit = __textureUnit;
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
}

void WTexture::setData(Int32 width, Int32 height, void *data, Type textureType)
{
    glActiveTexture(GL_TEXTURE0 + textureUnit());
    //printf("Resize: W %i, H %i\n",width,height);
    // Prevent gen a new texture if the buffer size is the same
    if(true || width != _size.w() || height != _size.h() || textureType == Type::EGL)
    {
        UInt32 newTexture;

        _size = WSize(width,height);
        glGenTextures(1, &newTexture);
        glBindTexture (GL_TEXTURE_2D, newTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if(textureType == Type::EGL)
        {

            glEGLImageTargetTexture2DOES(GL_TEXTURE_2D,*(EGLImage*)data);
            WOpenGL::checkGLError("35");
        }
        else
        {
            //damages.clear();
            glTexImage2D(GL_TEXTURE_2D, 0, _format, _size.w(), _size.h(), 0, _format, GL_UNSIGNED_BYTE, data);
            /*
            else
            {

                GLuint fboId = 0;
                glGenFramebuffers(1, &fboId);
                glBindFramebuffer(GL_FRAMEBUFFER, fboId);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);

                glBindTexture(GL_TEXTURE_2D, newTexture);
                Int32 sx,sy,dx,dy = 0;
                if(resizeDirection.x() > 0)
                {
                    xx =
                }
                if(resizeDirection.y() > 0)
                    resizeDirection.setY(0);
                */
                /*
                glCopyTexSubImage2D(GL_TEXTURE_2D, 0, abs(resizeDirection.x()), abs(resizeDirection.y()),0,0,oldSize.w(), oldSize.h());
                WOpenGL::checkGLError("Error creating texture from shared memory.");
                glDeleteFramebuffers(1,&fboId);

                for(list<WRect>::iterator t = damages.begin(); t != damages.end(); t++)
                {

                    if((*t).x() < 0)
                        (*t).setX(0);
                    if((*t).y() < 0)
                        (*t).setY(0);
                    if((*t).w() + (*t).x() > _size.w() || (*t).w() < 0)
                        (*t).setW(_size.w() - (*t).x());
                    if((*t).h() + (*t).y() > _size.h() || (*t).h() < 0)
                        (*t).setH(_size.h() - (*t).y());

                    if((*t).w() <= 0 || (*t).h() <= 0 || (*t).x() + (*t).w() > _size.w() || (*t).y() + (*t).h() > _size.h())
                    {
                        t = damages.erase(t);
                        continue;
                    }


                    printf("%i %i %i %i\n",(*t).x(),(*t).y(),(*t).w(),(*t).h());
                    glPixelStorei(GL_UNPACK_ROW_LENGTH,_size.w());
                    glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)(*t).x());
                    glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)(*t).y());

                    glTexSubImage2D(GL_TEXTURE_2D,
                        0,
                        (GLint)(*t).x(),
                        (GLint)(*t).y(),
                        (GLsizei)(*t).w(),
                        (GLsizei)(*t).h(),
                        _format,
                        GL_UNSIGNED_BYTE,
                        data);

                    WOpenGL::checkGLError("Error updating texture region.");

                }


                glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
                glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
                glPixelStorei(GL_UNPACK_SKIP_ROWS,0);

            }
            */
        }

        deleteTexture();
        _textureId = newTexture;
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _textureId);


        // If texture is not EGL
        for(list<WRect>::iterator t = damages.begin(); t != damages.end(); t++)
        {

            if((*t).x() < 0)
                (*t).setX(0);
            if((*t).y() < 0)
                (*t).setY(0);
            if((*t).w() + (*t).x() > _size.w() || (*t).w() < 0)
                (*t).setW(_size.w() - (*t).x());
            if((*t).h() + (*t).y() > _size.h() || (*t).h() < 0)
                (*t).setH(_size.h() - (*t).y());

            if((*t).w() <= 0 || (*t).h() <= 0 || (*t).x() + (*t).w() > _size.w() || (*t).y() + (*t).h() > _size.h())
            {
                t = damages.erase(t);
                continue;
            }


            //printf("%i %i %i %i %i %i\n",(*t).x(),(*t).y(),(*t).w(),(*t).h(),width,height);
            glPixelStorei(GL_UNPACK_ROW_LENGTH,_size.w());
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)(*t).x());
            glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)(*t).y());

            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                (GLint)(*t).x(),
                (GLint)(*t).y(),
                (GLsizei)(*t).w(),
                (GLsizei)(*t).h(),
                _format,
                GL_UNSIGNED_BYTE,
                data);

            //WOpenGL::checkGLError("Error updating texture region.");

        }

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

        WOpenGL::checkGLError("Error updating texture region.");
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
        WOpenGL::checkGLError("38");
        _initialized = false;
    }
}

