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
    m_unit = textureUnit;
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
}

LTexture::~LTexture()
{
    deleteTexture();
}

void LTexture::setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType)
{
    deleteTexture();

    m_bufferFormat = buffFormat;

    GLuint newTexture = 0;

    m_size.setW(width);
    m_size.setH(height);

    glGenTextures(1, &newTexture);
    glBindTexture (GL_TEXTURE_2D, newTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_id = newTexture;

    // EGL
    if(buffType == BufferType::EGL)
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D,*(EGLImage*)data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, buffFormat, m_size.w(), m_size.h(), 0, buffFormat, buffDepth, data);

    m_initialized = true;
}

void LTexture::deleteTexture()
{
    glActiveTexture(GL_TEXTURE0 + unit());
    if(initialized())
    {
        glDeleteTextures(1, &m_id);
        m_initialized = false;
    }
}

const LSize &LTexture::size() const
{
    return m_size;
}

bool LTexture::initialized()
{
    return m_initialized;
}

GLuint LTexture::id()
{
    return m_id;
}

GLuint LTexture::unit()
{
    return m_unit;
}

LTexture::BufferType LTexture::bufferroleType() const
{
    return m_bufferType;
}

GLenum LTexture::bufferFormat() const
{
    return m_bufferFormat;
}

