#ifndef WWAYLAND_H
#define WWAYLAND_H

#include <wayland-server.h>
#include <WNamespaces.h>
#include <EGL/egl.h>


class WaylandPlus::WWayland
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

    static wl_event_source *addTimer(wl_event_loop_timer_func_t func, void *data);


};



#endif // WWAYLAND_H
