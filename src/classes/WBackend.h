#ifndef WBACKEND
#define WBACKEND

#define W_BACKEND 1

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WNamespaces.h>
#include <list>

class WaylandPlus::WBackend
{
public:
    static std::list<WOutput*>getAvaliableOutputs();
    static EGLDisplay getEGLDisplay(WOutput *output);
    static void createGLContext(WOutput *output);
    static void flipPage(WOutput *output);
    /*
    static void initBackend(WCompositor *compositor);
    static void paintDRM();
    static int backendWidth();
    static int backendHeight();
    static EGLDisplay getEGLDisplay();

    static void setHWCursor();
    static void setHWCursorPos(Int32 x, Int32 y);
    */
};


#endif // WBACKEND
