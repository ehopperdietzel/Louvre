#include "LTexture.h"
#include <stdio.h>
#include <LOpenGL.h>
#include <LRect.h>
#include <GLES2/gl2.h>


using namespace Louvre;
using namespace std;

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

LTexture::LTexture(GLuint textureUnit)
{
    p_unit = textureUnit;
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
}

void LTexture::setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType)
{
    //GLenum depth = GL_
    glActiveTexture(GL_TEXTURE0 + unit());

    // Prevent gen a new texture if the buffer size is the same
    if(width != p_size.w() || height != p_size.h() || buffType == BufferType::EGL)
    {
        UInt32 newTexture;

        p_size.setW(width);
        p_size.setH(height);

        glGenTextures(1, &newTexture);
        glBindTexture (GL_TEXTURE_2D, newTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        if(buffType == BufferType::EGL)
        {
            glEGLImageTargetTexture2DOES(GL_TEXTURE_2D,*(EGLImage*)data);
        }
        else
        {
            damages.clear();
            glTexImage2D(GL_TEXTURE_2D, 0, buffFormat, width, height, 0, buffFormat, buffDepth, data);
        }

        deleteTexture();
        p_id = newTexture;
    }
    else
    {
        //glBindTexture(GL_TEXTURE_2D, _textureId);

        //printf("DAMAGES: %lu\n",damages.size());


        for(list<LRect>::iterator t = damages.begin(); t != damages.end(); t++)
        {

            if((*t).x() < 0)
                (*t).setX(0);
            if((*t).y() < 0)
                (*t).setY(0);
            if((*t).w() + (*t).x() > p_size.w() || (*t).w() < 0)
                (*t).setW(p_size.w() - (*t).x());
            if((*t).h() + (*t).y() > p_size.h() || (*t).h() < 0)
                (*t).setH(p_size.h() - (*t).y());

            if((*t).w() <= 0 || (*t).h() <= 0 || (*t).x() + (*t).w() > p_size.w() || (*t).y() + (*t).h() > p_size.h())
            {
                t = damages.erase(t);
                continue;
            }

            /*
            printf("%i %i %i %i %i %i\n",(*t).x(),(*t).y(),(*t).w(),(*t).h(),width,height);
            glPixelStorei(GL_UNPACK_ROW_LENGTH,_size.w());
            glPixelStorei(GL_UNPACK_SKIP_PIXELS,(GLint)(*t).x());
            glPixelStorei(GL_UNPACK_SKIP_ROWS,(GLint)(*t).y());

            glTexSubImage2D(GL_TEXTURE_2D,
                0,
                0,//(GLint)(*t).x(),
                0,//(GLint)(*t).y(),
                width,//(GLsizei)(*t).w(),
                height,//(GLsizei)(*t).h(),
                _format,
                GL_UNSIGNED_BYTE,
                data);
            */
            //LOpenGL::checkGLError("Error updating texture region.");

        }

        /*
        glPixelStorei(GL_UNPACK_ROW_LENGTH,0);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS,0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS,0);
        */
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

        LOpenGL::checkGLError("Error updating texture region.");
        */

        UInt32 newTexture;

        p_size.setW(width);
        p_size.setH(height);

        glGenTextures(1, &newTexture);
        glBindTexture (GL_TEXTURE_2D, newTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, buffFormat, p_size.w(), p_size.h(), 0, buffFormat, buffDepth, data);
        deleteTexture();
        p_id = newTexture;
    }

    p_initialized = true;
}

void LTexture::deleteTexture()
{
    if(initialized())
    {
        glDeleteTextures(1, &p_id);
        p_initialized = false;
    }
}

const LSize &LTexture::size() const
{
    return p_size;
}

bool LTexture::initialized()
{
    return p_initialized;
}

GLuint LTexture::id()
{
    return p_id;
}

GLuint LTexture::unit()
{
    return p_unit;
}

LTexture::BufferType LTexture::bufferType()
{
    return p_bufferType;
}

