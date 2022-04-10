#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <queue>
#include <WNamespaces.h>


class WaylandPlus::WTexture
{

public:

    enum Type
    {
        SHM = 0,
        EGL = 1
    };

    WTexture(GLuint textureUnit = 0);
    void setData(int width, int height, void *data, Type textureType = SHM);
    void deleteTexture();
    int height(){return _height;}
    int width(){return _width;}
    bool isInitialized(){ return _initialized;}
    GLuint textureId(){return _textureId;}
    GLuint textureUnit(){return _textureUnit;}
    Type type(){return _type;}

private:
    friend class WaylandPlus::Globals::Surface;
    friend class WaylandPlus::WSurface;

    std::queue<Rect>damages;
    std::queue<Rect>pendingDamages;
    int _width,_height = 0;
    GLuint _textureId,_textureUnit = 0;
    bool _initialized = false;
    Type _type = SHM;
    UInt32 _format = GL_RGBA;

};

#endif
