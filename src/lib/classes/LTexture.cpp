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

LTexture::~LTexture()
{
    deleteTexture();
}

void LTexture::setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType)
{
    //GLenum depth = GL_
    glActiveTexture(GL_TEXTURE0 + unit());

    p_bufferFormat = buffFormat;

    // Prevent gen a new texture if the buffer size is the same
    if(buffType == BufferType::EGL)
    {
        GLuint newTexture = 0;

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

        }

        GLuint newTexture = 0;

        p_size.setW(width);
        p_size.setH(height);

        glGenTextures(1, &newTexture);
        glBindTexture (GL_TEXTURE_2D, newTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //LOpenGL::checkGLError("SIP");
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
        glActiveTexture(GL_TEXTURE0 + unit());
        glBindTexture(GL_TEXTURE_2D, p_id);
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

LTexture::BufferType LTexture::bufferroleType() const
{
    return p_bufferType;
}

GLenum LTexture::bufferFormat() const
{
    return p_bufferFormat;
}

