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

    WTexture();
    void setData(int width, int height, void *data, Type textureType = SHM);
    void deleteTexture();
    int height(){return _height;}
    int width(){return _width;}
    bool isInitialized(){ return _initialized;}
    GLuint textureId(){return _textureId;}
    Type type(){return _type;}

    std::queue<Rect>damages;

private:
    int _width,_height = 0;
    GLuint _textureId = 0;
    bool _initialized = false;
    Type _type = SHM;
};

#endif
