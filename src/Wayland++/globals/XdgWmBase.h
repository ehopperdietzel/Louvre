#ifndef XDGWMBASE_H
#define XDGWMBASE_H

#include <wayland-server.h>
#include <WTypes.h>

// xdg wm base
void xdg_wm_base_destroy(wl_client *client, wl_resource *resource);
void xdg_wm_base_create_positioner(wl_client *client, wl_resource *resource, UInt32 id);
void xdg_wm_base_get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *_surface);
void xdg_wm_base_pong(wl_client *client, wl_resource *resource, UInt32 serial);
void xdg_wm_base_bind(wl_client *client, void *data, UInt32 version, UInt32 id);


#endif // XDGWMBASE_H
