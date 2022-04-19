#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <queue>
#include <WNamespaces.h>
#include <WSize.h>

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

private:
    friend class Wpp::Globals::Surface;
    friend class Wpp::WSurface;

    std::queue<Rect>damages;
    std::queue<Rect>pendingDamages;
    WSize _size;
    GLuint _textureId,_textureUnit = 0;
    bool _initialized = false;
    Type _type = SHM;
    UInt32 _format = GL_RGBA;

};

#endif
