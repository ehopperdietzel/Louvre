#ifndef WBACKEND
#define WBACKEND

#define W_BACKEND 1

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WNamespaces.h>

class WaylandPlus::WBackend
{
public:
    static void initBackend(WCompositor *compositor);
    static void paintDRM();
    static int backendWidth();
    static int backendHeight();
    static EGLDisplay getEGLDisplay();

    static void setHWCursor();
    static void setHWCursorPos(Int32 x, Int32 y);
};


#endif // WBACKEND
