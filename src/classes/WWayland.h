#ifndef WWAYLAND_H
#define WWAYLAND_H

//#include <wayland-server.h>
#include <WNamespaces.h>
#include <EGL/egl.h>


class Wpp::WWayland
{
public:
    static int initWayland(WCompositor *comp);//, int libinputFd, wl_event_loop_fd_func_t *libinputFunc);
    static void terminateDisplay();
    static void dispatchEvents();
    static void flushClients();
    static int readFd(int, unsigned int, void*);
    static void scheduleDraw(WCompositor *comp);
    static void bindEGLDisplay(EGLDisplay eglDisplay);
    static void runLoop();

    static void clientConnectionEvent(wl_listener *listener, void *data);
    static void clientDisconnectionEvent(wl_listener *listener, void *data);

    static int apply_damage_emit(void *data);
    static void clDisc(wl_listener *listener, void *data);

    static wl_event_source *addTimer(wl_event_loop_timer_func_t func, void *data);

    static EGLContext eglContext();
    static void initGLContext();
    static void setContext(EGLDisplay sharedDisplay, EGLContext sharedContext);


};



#endif // WWAYLAND_H
