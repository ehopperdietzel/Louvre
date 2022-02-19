#ifndef XDGTOPLEVEL_H
#define XDGTOPLEVEL_H

#include <wayland-server.h>
#include <WTypes.h>

// xdg toplevel
void xdg_toplevel_destroy(wl_client *client, wl_resource *resource);
void xdg_toplevel_set_parent(wl_client *client, wl_resource *resource, wl_resource *parent);
void xdg_toplevel_set_title(wl_client *client, wl_resource *resource, const char *title);
void xdg_toplevel_set_app_id(wl_client *client, wl_resource *resource, const char *app_id);
void xdg_toplevel_show_window_menu(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y);
void xdg_toplevel_move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial);
void xdg_toplevel_resize(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges);
void xdg_toplevel_set_max_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height);
void xdg_toplevel_set_min_size(wl_client *client, wl_resource *resource, Int32 width, Int32 height);
void xdg_toplevel_set_maximized(wl_client *client, wl_resource *resource);
void xdg_toplevel_unset_maximized(wl_client *client, wl_resource *resource);
void xdg_toplevel_set_fullscreen(wl_client *client, wl_resource *resource, wl_resource *output);
void xdg_toplevel_unset_fullscreen(wl_client *client, wl_resource *resource);
void xdg_toplevel_set_minimized(wl_client *client, wl_resource *resource);


#endif // XDGTOPLEVEL_H
