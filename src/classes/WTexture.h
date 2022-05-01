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

    enum Type
    {
        SHM = 0,
        EGL = 1
    };

    WTexture(GLuint textureUnit = 0);
    void setData(Int32 width, Int32 height, void *data, Type textureType = SHM);
    void deleteTexture();
    WSize size(){ return _size; }
    bool isInitialized(){ return _initialized;}
    GLuint textureId(){return _textureId;}
    GLuint textureUnit(){return _textureUnit;}
    Type type(){return _type;}
    UInt32 format(){return _format;}

    WPoint resizeDirection;
    std::list<WRect>damages;
    std::list<WRect>pendingDamages;

private:
    friend class Wpp::Globals::Surface;
    friend class Wpp::WSurface;


    WSize _size;
    GLuint _textureId,_textureUnit = 0;
    bool _initialized = false;
    Type _type = SHM;
    UInt32 _format = GL_RGBA;

};

#endif
