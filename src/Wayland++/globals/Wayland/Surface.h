#ifndef SURFACE_H
#define SURFACE_H

#include <wayland-server.h>
#include <WNamespaces.h>

class WaylandPlus::Globals::Surface
{
public:
    static void get_egl_func();
    static void delete_surface(wl_resource *resource);
    static void attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y);
    static void frame(wl_client *client, wl_resource *resource, UInt32 callback);
    static void destroy(wl_client *client, wl_resource *resource);
    static void commit(wl_client *client, wl_resource *resource);
    static void damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);
    static void set_opaque_region(wl_client *client, wl_resource *resource, wl_resource *region);
    static void set_input_region(wl_client *client, wl_resource *resource, wl_resource *region);
    static void set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform);
    static void damage_buffer(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 w, Int32 h);
    static void set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale);
};

#endif // SURFACE_H
