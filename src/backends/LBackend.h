#ifndef WBACKEND
#define WBACKEND

#define W_BACKEND 2

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <LNamespaces.h>
#include <list>

class Louvre::LBackend
{
public:
    static void initialize(LCompositor *compositor);
    static std::list<LOutput*>*getAvaliableOutputs(LCompositor *compositor);
    static EGLDisplay getEGLDisplay(LOutput *output);
    static void createGLContext(LOutput *output);
    static void flipPage(LOutput *output);
    static bool hasHardwareCursorSupport();

    static void setCursor(LOutput *output, LTexture *texture, const LSizeF &size);
    static void setCursorPosition(LOutput *output, const LPoint &position);
};


#endif // WBACKEND
