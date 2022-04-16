#include "XdgSurface.h"
#include <XdgToplevel.h>
#include <XdgPopup.h>
#include <xdg-shell.h>
#include <WSurface.h>
#include <WCompositor.h>
#include <WPositioner.h>

using namespace Wpp;

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
void Extensions::XdgShell::Surface::resource_destroy(wl_resource *resource)
{
    printf("XDG SURFACE DESTROYED\n");
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_shell = nullptr;
}

void Extensions::XdgShell::Surface::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;
    Extensions::XdgShell::Surface::resource_destroy(resource);
}
void Extensions::XdgShell::Surface::get_toplevel(wl_client *client,wl_resource *resource, UInt32 id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id); // 4

    surface->_type = SurfaceType::Toplevel;
    wl_resource_set_implementation(surface->xdg_toplevel, &xdg_toplevel_implementation, surface, &Extensions::XdgShell::Toplevel::destroy_resource);
    surface->sendConfigureEvent(0,0,SurfaceState::Activated);
    surface->typeChangeRequest();
}
void Extensions::XdgShell::Surface::get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{
    (void)parent;(void)positioner;

    WPositioner *wPositioner = (WPositioner*)wl_resource_get_user_data(positioner);

    if(wPositioner->size().area() <= 0 || wPositioner->anchorRect().area() <= 0)
    {
        printf("INVALID POSITIONER.\n");
        wl_resource_post_error(positioner,XDG_POSITIONER_ERROR_INVALID_INPUT,"Invalid Popup");
        return;
    }

    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Delete previous popup if exists
    if(surface->_positioner != nullptr)
        delete surface->_positioner;

    surface->_positioner = wPositioner;
    surface->xdg_popup = wl_resource_create(client, &xdg_popup_interface, wl_resource_get_version(resource), id); // 4
    surface->_type = SurfaceType::Popup;
    if(parent != NULL)
    {
        printf("POPUP HAS PARENT.\n");
        WSurface *parentSurface = (WSurface*)wl_resource_get_user_data(parent);
        surface->_parent = parentSurface;
        parentSurface->_children.push_back(surface);
        printf("CHILDREN %lu.\n",parentSurface->_children.size());
        surface->parentChangeRequest();
    }

    wl_resource_set_implementation(surface->xdg_popup, &xdg_popup_implementation, surface, &Extensions::XdgShell::Popup::destroy_resource);
    xdg_popup_send_configure(surface->xdg_popup,0,0,0,0);
    surface->typeChangeRequest();
    printf("NEW POPUP.\n");
}
void Extensions::XdgShell::Surface::set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->_decorationGeometry = {x, y, width, height};
    surface->geometryChangeRequest();
}
void Extensions::XdgShell::Surface::ack_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)resource;(void)serial;
}

