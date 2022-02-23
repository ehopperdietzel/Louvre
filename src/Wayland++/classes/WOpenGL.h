#ifndef WOPENGL_H
#define WOPENGL_H

#include <WNamespaces.h>

class WaylandPlus::WOpenGL
{
public:
    static char *openShader(const char *fname);
    static void checkGLError(const char *msg);
};

#endif // WOPENGL_H
