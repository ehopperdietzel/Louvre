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

    LTexture(GLuint textureUnit = 0);
    void setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType = SHM);
    void deleteTexture();
    const LSize &size() const;
    bool initialized();
    GLuint id();
    GLuint unit();
    BufferType bufferType() const;
    GLenum bufferFormat() const;

    //LPoint resizeDirection;
    std::list<LRect>damages;
    std::list<LRect>pendingDamages;

private:
    friend class Louvre::Globals::Surface;
    friend class Louvre::LSurface;


    LSize p_size;
    GLuint p_id,p_unit = 0;
    bool p_initialized = false;
    BufferType p_bufferType = SHM;
    GLenum p_bufferFormat;

};

#endif
