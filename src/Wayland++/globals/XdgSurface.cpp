#include "XdgSurface.h"
#include <globals/XdgToplevel.h>
#include <globals/XdgPopup.h>
#include <protocols/xdg-shell.h>
#include <WSurface.h>

static struct xdg_toplevel_interface xdg_toplevel_implementation = {&xdg_toplevel_destroy, &xdg_toplevel_set_parent, &xdg_toplevel_set_title, &xdg_toplevel_set_app_id, &xdg_toplevel_show_window_menu, &xdg_toplevel_move, &xdg_toplevel_resize, &xdg_toplevel_set_max_size, &xdg_toplevel_set_min_size, &xdg_toplevel_set_maximized, &xdg_toplevel_unset_maximized, &xdg_toplevel_set_fullscreen, &xdg_toplevel_unset_fullscreen, &xdg_toplevel_set_minimized};
static struct xdg_popup_interface xdg_popup_implementation = {&xdg_popup_destroy,&xdg_popup_grab,&xdg_popup_reposition};

// xdg surface
void xdg_surface_destroy (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_surface_get_toplevel(wl_client *client,wl_resource *resource, UInt32 id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_toplevel = wl_resource_create(client, &xdg_toplevel_interface, 1, id);
    wl_resource_set_implementation(surface->xdg_toplevel, &xdg_toplevel_implementation, surface, NULL);
}
void xdg_surface_get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{
    (void)parent;(void)positioner;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_popup = wl_resource_create(client, &xdg_popup_interface, 1, id);
    wl_resource_set_implementation(surface->xdg_popup, &xdg_popup_implementation, surface, NULL);
}
void xdg_surface_set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;(void)resource;(void)x;(void)y;(void)width;(void)height;
}
void xdg_surface_ack_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)resource;(void)serial;
}

