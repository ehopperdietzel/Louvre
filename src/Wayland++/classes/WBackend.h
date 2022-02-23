#ifndef WBACKEND
#define WBACKEND

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
};


#endif // WBACKEND
