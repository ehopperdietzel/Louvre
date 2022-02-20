#include "XdgWmBase.h"
#include <protocols/xdg-shell.h>
#include <WCompositor.h>
#include <globals/XdgSurface.h>

static struct xdg_surface_interface xdg_surface_implementation = {&xdg_surface_destroy, &xdg_surface_get_toplevel, &xdg_surface_get_popup, &xdg_surface_set_window_geometry, &xdg_surface_ack_configure};
static struct xdg_wm_base_interface xdg_wm_base_implementation = {&xdg_wm_base_destroy, &xdg_wm_base_create_positioner, &xdg_wm_base_get_xdg_surface, &xdg_wm_base_pong};

// xdg wm base
void xdg_wm_base_destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_wm_base_create_positioner(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
}
void xdg_wm_base_get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *_surface)
{
    (void)resource;
    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(_surface);
    surface->xdg_shell = wl_resource_create(client, &xdg_surface_interface, 4, id);
    wl_resource_set_implementation(surface->xdg_shell, &xdg_surface_implementation, surface, NULL);
    xdg_surface_send_configure(surface->xdg_shell, 0);
}
void xdg_wm_base_pong(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)resource;(void)serial;
}

void xdg_wm_base_bind (wl_client *client, void *data, UInt32 version, UInt32 id)
{
    printf ("NEW XDG-SHELL.\n");
    (void)client;(void)data;(void)version;
    wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, 1, id);
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, NULL, NULL);
}
