#ifndef SUBSURFACE_H
#define SUBSURFACE_H


#include <wayland-server.h>
#include <WNamespaces.h>

class Wpp::Globals::Subsurface
{
public:
    static void destroy(wl_client *client, wl_resource *resource);
    static void set_position(wl_client *client, wl_resource *resource, Int32 x, Int32 y);
    static void place_above(wl_client *client, wl_resource *resource, wl_resource *sibiling);
    static void place_below(wl_client *client, wl_resource *resource, wl_resource *sibiling);
    static void set_sync(wl_client *client, wl_resource *resource);
    static void set_desync(wl_client *client, wl_resource *resource);
    static void bind(wl_client *client, void *data, UInt32 version, UInt32 id);
};

#endif // SUBSURFACE_H
