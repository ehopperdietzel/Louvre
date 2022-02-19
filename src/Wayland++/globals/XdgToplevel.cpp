#include "XdgToplevel.h"
#include <stdio.h>

// xdg toplevel
void xdg_toplevel_destroy (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_toplevel_set_parent (wl_client *client, wl_resource *resource, wl_resource *parent)
{
    (void)client;(void)resource;(void)parent;
}
void xdg_toplevel_set_title (wl_client *client, wl_resource *resource, const char *title)
{
    (void)client;(void)resource;(void)title;
}
void xdg_toplevel_set_app_id (wl_client *client, wl_resource *resource, const char *app_id)
{
    (void)client;(void)resource;(void)app_id;
}
void xdg_toplevel_show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)x;(void)y;
}
void xdg_toplevel_move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)resource;(void)seat;(void)serial;
    /*
    struct surface *surface = wl_resource_get_user_data (resource);
    // during the move the surface coordinates are relative to the pointer
    surface->x = surface->x - pointer_x;
    surface->y = surface->y - pointer_y;
    moving_surface = surface;
    */
}
void xdg_toplevel_resize (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges)
{
    (void)client;(void)resource;(void)seat;(void)serial;(void)edges;
}
void xdg_toplevel_set_max_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;(void)resource;(void)width;(void)height;
}
void xdg_toplevel_set_min_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height)
{
    (void)client;(void)resource;(void)width;(void)height;
}
void xdg_toplevel_set_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    printf ("surface requested maximize\n");
}
void xdg_toplevel_unset_maximized (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_toplevel_set_fullscreen(wl_client *client, wl_resource *resource, wl_resource *output)
{
    (void)client;(void)resource;(void)output;
}
void xdg_toplevel_unset_fullscreen(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_toplevel_set_minimized(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}

