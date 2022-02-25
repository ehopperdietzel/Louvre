#include "XdgSurface.h"
#include <XdgToplevel.h>
#include <XdgPopup.h>
#include <xdg-shell.h>
#include <WSurface.h>
#include <WCompositor.h>

using namespace WaylandPlus;

static struct xdg_toplevel_interface xdg_toplevel_implementation =
{
    &Extensions::XdgShell::Toplevel::destroy,
    &Extensions::XdgShell::Toplevel::set_parent,
    &Extensions::XdgShell::Toplevel::set_title,
    &Extensions::XdgShell::Toplevel::set_app_id,
    &Extensions::XdgShell::Toplevel::show_window_menu,
    &Extensions::XdgShell::Toplevel::move,
    &Extensions::XdgShell::Toplevel::resize,
    &Extensions::XdgShell::Toplevel::set_max_size,
    &Extensions::XdgShell::Toplevel::set_min_size,
    &Extensions::XdgShell::Toplevel::set_maximized,
    &Extensions::XdgShell::Toplevel::unset_maximized,
    &Extensions::XdgShell::Toplevel::set_fullscreen,
    &Extensions::XdgShell::Toplevel::unset_fullscreen,
    &Extensions::XdgShell::Toplevel::set_minimized
};

static struct xdg_popup_interface xdg_popup_implementation =
{
    &Extensions::XdgShell::Popup::destroy,
    &Extensions::XdgShell::Popup::grab,
    &Extensions::XdgShell::Popup::reposition
};

// xdg surface
void Extensions::XdgShell::Surface::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void Extensions::XdgShell::Surface::get_toplevel(wl_client *client,wl_resource *resource, UInt32 id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id); // 4
    surface->_type = SurfaceType::Toplevel;
    wl_resource_set_implementation(surface->xdg_toplevel, &xdg_toplevel_implementation, surface, NULL);
    surface->getCompositor()->newSurface(surface);
    surface->sendConfigureEvent(0,0,SurfaceState::Activated);
}
void Extensions::XdgShell::Surface::get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{
    (void)parent;(void)positioner;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_popup = wl_resource_create(client, &xdg_popup_interface, wl_resource_get_version(resource), id); // 4
    surface->_type = SurfaceType::Popup;
    wl_resource_set_implementation(surface->xdg_popup, &xdg_popup_implementation, surface, NULL);
}
void Extensions::XdgShell::Surface::set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->_decorationGeometry.x = x;
    surface->_decorationGeometry.y = y;
    surface->_decorationGeometry.width = width;
    surface->_decorationGeometry.height = height;

    surface->getCompositor()->surfaceGeometryChangedRequest(surface,x,y,width,height);
}
void Extensions::XdgShell::Surface::ack_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)resource;(void)serial;
}

