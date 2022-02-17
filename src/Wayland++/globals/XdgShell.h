#ifndef XDGSHELL_H
#define XDGSHELL_H

#include <wayland-server.h>
#include <WTypes.h>

// XDG SHELL

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

// xdg surface
void xdg_surface_destroy(wl_client *client, wl_resource *resource);
void xdg_surface_get_toplevel(wl_client *client,wl_resource *resource, UInt32 id);
void xdg_surface_get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner);
void xdg_surface_set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);
void xdg_surface_ack_configure(wl_client *client, wl_resource *resource, UInt32 serial);

// xdg wm base
void xdg_wm_base_destroy(wl_client *client, wl_resource *resource);
void xdg_wm_base_create_positioner(wl_client *client, wl_resource *resource, UInt32 id);
void xdg_wm_base_get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *_surface);
void xdg_wm_base_pong(wl_client *client, wl_resource *resource, UInt32 serial);
void xdg_wm_base_bind(wl_client *client, void *data, UInt32 version, UInt32 id);


#endif // XDGSHELL_H
