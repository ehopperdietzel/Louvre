#include "WTexture.h"

static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

WTexture::WTexture()
{

}

void WTexture::setData(int width, int height, void *data, bool isEGLImage)
{
    _width = width;
    _height = height;
    glGenTextures (1, &_textureId);
    glBindTexture (GL_TEXTURE_2D, _textureId);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if(isEGLImage)
        glEGLImageTargetTexture2DOES (GL_TEXTURE_2D,*(EGLImage*)data);
    else
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

    glBindTexture (GL_TEXTURE_2D, 0);
}

void WTexture::deleteTexture()
{
    if(_textureId)
        glDeleteTextures(1, &_textureId);
}

void WTexture::draw(int x, int y)
{
    if (!_textureId) return;

    GLint vertices[] = {
        x, y,
        x+_width, y,
        x+_width, y+_height,
        x, y+_height
    };
    GLint tex_coords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    glEnable (GL_TEXTURE_2D);
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glBindTexture (GL_TEXTURE_2D, _textureId);
    glVertexPointer (2, GL_INT, 0, vertices);
    glTexCoordPointer (2, GL_INT, 0, tex_coords);

    glDrawArrays (GL_QUADS, 0, 4);

    glBindTexture (GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_RECTANGLE);
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}
