#ifndef LWAYLAND_H
#define LWAYLAND_H

//#include <wayland-server.h>
#include <LNamespaces.h>
#include <EGL/egl.h>


class Louvre::WWayland
{
public:

    static void setSeat(LSeat *seat);
    static int processSeat(int, unsigned int, void*userData);


    static UInt32 nextSerial();

    static int initWayland(LCompositor *comp);//, int libinputFd, wl_event_loop_fd_func_t *libinputFunc);
    static void terminateDisplay();
    static void dispatchEvents();
    static void flushClients();
    static int readFd(int, unsigned int, void*);
    static void scheduleDraw(LCompositor *comp);
    static void bindEGLDisplay(EGLDisplay eglDisplay);
    static void runLoop();

    static void clientConnectionEvent(wl_listener *listener, void *data);
    static void clientDisconnectionEvent(wl_listener *listener, void *data);

    static int apply_damage_emit(void *data);
    static void clDisc(wl_listener *listener, void *data);

    static wl_event_source *addTimer(wl_event_loop_timer_func_t func, void *data);

    static EGLContext eglContext();
    static EGLDisplay eglDisplay();
    static void initGLContext();
    static bool isGlContextInitialized();
    static void setContext(LOutput *output, EGLDisplay sharedDisplay, EGLContext sharedContext);

    static LOutput *mainOutput();

    static void forceUpdate();


};



#endif // LWAYLAND_H
