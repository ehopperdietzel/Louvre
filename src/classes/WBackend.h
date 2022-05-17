#ifndef WBACKEND
#define WBACKEND

#define W_BACKEND 2

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WNamespaces.h>
#include <list>

class Wpp::WBackend
{
public:
    static std::list<WOutput*>getAvaliableOutputs();
    static EGLDisplay getEGLDisplay(WOutput *output);
    static void createGLContext(WOutput *output);
    static void flipPage(WOutput *output);
    static bool hasHardwareCursorSupport();


    static void setCursor(WOutput *output, WTexture *texture, const WSizeF &size);
    static void setCursorPosition(WOutput *output, const WPoint &position);

};


#endif // WBACKEND
