#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>


class WTexture
{
public:
    WTexture();
    void setData(int width, int height, void *data, bool isEGLImage = false);
    void deleteTexture();
    void draw(int x, int y);
    int height(){return _height;}
    int width(){return _width;}
    GLuint textureId(){return _textureId;}
private:
    int _width,_height;
    GLuint _textureId = 10000;
};

#endif
