#ifndef WOPENGL_H
#define WOPENGL_H

#include <WNamespaces.h>
#include <GL/gl.h>

class Wpp::WOpenGL
{
public:
    WOpenGL();
    static char *openShader(const char *fname);
    static void checkGLError(const char *msg);
    static GLuint getMaxTextureUnits();
    static GLuint LoadShader(GLenum type, const char *shaderSrc);
    static WTexture *loadTexture(const char *fileName);

    void scaleCursor(WTexture *texture, const WRect &src, const WRect &dst);
    void drawTexture(WTexture *texture, const WRect &src, const WRect &dst);
    void drawColor(const WRect &dst, Float32 r, Float32 g, Float32 b, Float32 a);
    void setViewport(const WRect &rect);
    void clearScreen();
private:

    friend class WOutput;

    // Square (left for vertex, right for fragment)
    GLfloat square[16] =
    {  /*  VERTEX       FRAGMENT */
        -1.0f,  1.0f,   0.f, 1.f, // TL
        -1.0f, -1.0f,   0.f, 0.f, // BL
         1.0f, -1.0f,   1.f, 0.f, // BR
         1.0f,  1.0f,   1.f, 1.f  // TR
    };

    // Uniform variables
    GLuint
    texSizeUniform,             // Texture size (width,height)
    srcRectUniform,             // Src tex rect (x,y,width,height)
    activeTextureUniform,       // glActiveTexture
    modeUniform,
    colorUniform;

    WOutput *p_output = nullptr;
};

#endif // WOPENGL_H
