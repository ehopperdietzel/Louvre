#ifndef SURFACE_H
#define SURFACE_H

#include <wayland-server.h>
#include <WTypes.h>

// SURFACE
void surface_get_egl_func();

void delete_surface(wl_resource *resource);
void surface_attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y);
void surface_frame(wl_client *client, wl_resource *resource, UInt32 callback);
void surface_destroy(wl_client *client, wl_resource *resource);
void surface_commit(wl_client *client, wl_resource *resource);
void surface_damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);
void surface_set_opaque_region(wl_client *client, wl_resource *resource, wl_resource *region);
void surface_set_input_region(wl_client *client, wl_resource *resource, wl_resource *region);
void surface_set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform);
void surface_set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale);


#endif // SURFACE_H
