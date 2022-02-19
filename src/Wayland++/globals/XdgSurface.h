#ifndef XDGSURFACE_H
#define XDGSURFACE_H

#include <wayland-server.h>
#include <WTypes.h>

// xdg surface
void xdg_surface_destroy(wl_client *client, wl_resource *resource);
void xdg_surface_get_toplevel(wl_client *client,wl_resource *resource, UInt32 id);
void xdg_surface_get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner);
void xdg_surface_set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);
void xdg_surface_ack_configure(wl_client *client, wl_resource *resource, UInt32 serial);

#endif // XDGSURFACE_H
