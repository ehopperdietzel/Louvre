#ifndef WOPENGL_H
#define WOPENGL_H

#include <WNamespaces.h>
#include <GL/gl.h>

class WaylandPlus::WOpenGL
{
public:
    static char *openShader(const char *fname);
    static void checkGLError(const char *msg);
    static GLuint getMaxTextureUnits();
};

#endif // WOPENGL_H
