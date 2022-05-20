#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <list>
#include <WNamespaces.h>
#include <WSize.h>
#include <WPoint.h>

class Wpp::WTexture
{

public:

    enum BufferType
    {
        SHM = 0,
        EGL = 1
    };

    WTexture(GLuint textureUnit = 0);
    void setData(Int32 width, Int32 height, void *data, GLenum buffFormat, GLenum buffDepth, BufferType buffType = SHM);
    void deleteTexture();
    const WSize &size() const;
    bool initialized();
    GLuint id();
    GLuint unit();
    BufferType bufferType();

    //WPoint resizeDirection;
    std::list<WRect>damages;
    std::list<WRect>pendingDamages;

private:
    friend class Wpp::Globals::Surface;
    friend class Wpp::WSurface;


    WSize p_size;
    GLuint p_id,p_unit = 0;
    bool p_initialized = false;
    BufferType p_bufferType = SHM;

};

#endif
