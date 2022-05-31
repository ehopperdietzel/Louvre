#ifndef LOPENGL_H
#define LOPENGL_H

#include <LNamespaces.h>
#include <GL/gl.h>

class Louvre::LOpenGL
{
public:
    LOpenGL();
    static char *openShader(const char *fname);
    static void checkGLError(const char *msg);
    static GLuint getMaxTextureUnits();
    static GLuint LoadShader(GLenum type, const char *shaderSrc);
    static LTexture *loadTexture(const char *fileName);

    void scaleCursor(LTexture *texture, const LRect &src, const LRect &dst);
    void drawTexture(LTexture *texture, const LRect &src, const LRect &dst);
    void drawColor(const LRect &dst, Float32 r, Float32 g, Float32 b, Float32 a);
    void setViewport(const LRect &rect);
    void clearScreen();
private:

    friend class LOutput;

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

    LOutput *p_output = nullptr;
};

#endif // LOPENGL_H
