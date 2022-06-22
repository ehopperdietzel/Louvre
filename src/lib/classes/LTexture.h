#ifndef LTEXTURE_H
#define LTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <list>
#include <LNamespaces.h>
#include <LSize.h>
#include <LPoint.h>

class Louvre::LTexture
{

public:

    enum BufferType
    {
        SHM = 0,
        EGL = 1
    };

    LTexture(GLuint textureUnit = 1);
    ~LTexture();
    void setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType = SHM);
    void deleteTexture();
    const LSize &size() const;
    bool initialized();
    GLuint id();
    GLuint unit();
    BufferType bufferroleType() const;
    GLenum bufferFormat() const;

private:
    friend class Louvre::Globals::Surface;
    friend class Louvre::LSurface;

    LSize m_size = LSize();
    GLuint m_id = 0;
    GLuint m_unit = 0;
    bool m_initialized = false;
    BufferType m_bufferType = SHM;
    GLenum m_bufferFormat = GL_BGRA;

};

#endif
