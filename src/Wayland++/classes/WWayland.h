#ifndef WWAYLAND_H
#define WWAYLAND_H

#include <wayland-server.h>
#include <WNamespaces.h>


class WaylandPlus::WWayland
{
public:
    static int initWayland(WCompositor *comp);//, int libinputFd, wl_event_loop_fd_func_t *libinputFunc);
    static void terminateDisplay();
    static void dispatchEvents();
    static void flushClients();

};



#endif // WWAYLAND_H
