#ifndef WOPENGL_H
#define WOPENGL_H

#include <WNamespaces.h>
#include <GL/gl.h>

class Wpp::WOpenGL
{
public:
    static char *openShader(const char *fname);
    static void checkGLError(const char *msg);
    static GLuint getMaxTextureUnits();
    static GLuint LoadShader(GLenum type, const char *shaderSrc);
    static void useWGLProgram();
};

#endif // WOPENGL_H
