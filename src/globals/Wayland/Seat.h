#ifndef SEAT_H
#define SEAT_H

#include <wayland-server.h>
#include <WNamespaces.h>


class WaylandPlus::Globals::Seat
{
public:
    static void get_pointer(wl_client *client, wl_resource *resource, UInt32 id);
    static void get_keyboard(wl_client *client, wl_resource *resource, UInt32 id);
    static void get_touch(wl_client *client, wl_resource *resource, UInt32 id);
    static void release(wl_client *client, wl_resource *resource);
    static void bind(wl_client *client, void *data, UInt32 version, UInt32 id);
};

#endif // SEAT_H
