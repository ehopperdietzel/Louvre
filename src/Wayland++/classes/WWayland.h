#ifndef WWAYLAND_H
#define WWAYLAND_H

#include <wayland-server.h>
#include <WNamespaces.h>


class WaylandPlus::WWayland
{
public:
    static int initWayland(WCompositor *comp);
    static void terminateDisplay();
    static void processWayland();
};



#endif // WWAYLAND_H
