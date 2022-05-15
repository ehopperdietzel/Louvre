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

    void drawTexture(WTexture *tex, WRect src, WRect dst);
    void drawColor(WRect dst, float r, float g, float b, float a);
    void setViewport(WRect rect,Int32 scale);
    void viewportToOutput();
private:

    friend class WOutput;

    // Square
    GLfloat square[16] =
    {
         0.0f, 0.0f,  0.f, 1.f,
         0.0f, 1.0f,  0.f, 0.f,
         1.0f, 1.0f,  1.f, 0.f,
         1.0f, 0.0f,  1.f, 1.f
    };

    // Uniform variables
    GLuint
    screenSizeUniform,          // Screen size (width,height)
    texSizeUniform,             // Texture size (width,height)
    dstRectUniform,             // Dest quad position and size (x,y,width,height)
    srcRectUniform,             // Src tex rect (x,y,width,height)
    activeTextureUniform,       // glActiveTexture
    modeUniform,
    colorUniform;

    WOutput *p_output = nullptr;
};

#endif // WOPENGL_H
