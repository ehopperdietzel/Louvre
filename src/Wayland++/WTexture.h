#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <queue>

struct Rect
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
};

class WTexture
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
