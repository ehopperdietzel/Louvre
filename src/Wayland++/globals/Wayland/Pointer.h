#ifndef POINTER_H
#define POINTER_H

#include <wayland-server.h>
#include <WNamespaces.h>

class WaylandPlus::Globals::Pointer
{
public:
    static void set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *_surface, Int32 hotspot_x, Int32 hotspot_y);
    static void release(wl_client *client, wl_resource *resource);
};
#endif // POINTER_H