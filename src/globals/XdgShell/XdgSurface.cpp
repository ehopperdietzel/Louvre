#include "XdgSurface.h"
#include <XdgToplevel.h>
#include <XdgPopup.h>
#include <xdg-shell.h>
#include <WSurface.h>
#include <WCompositor.h>
#include <WPositioner.h>
#include <WToplevelRole.h>

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
    surface->p_xdg_shell = nullptr;
}

void Extensions::XdgShell::Surface::destroy (wl_client *client, wl_resource *resource)
{
    (void)client;
    Extensions::XdgShell::Surface::resource_destroy(resource);
}
void Extensions::XdgShell::Surface::get_toplevel(wl_client *client, wl_resource *resource, UInt32 id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    printf("XDG Toplevel version:%i\n",wl_resource_get_version(resource));

    // Check errors
    /*
    if(surface->pending.buffer != nullptr || surface->current.buffer != nullptr)
    {
        wl_resource_post_error(resource,XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"Given wl_surface already has a buffer attached.");
        return;
    }*/

    if (surface->type() != WSurface::Undefined)
    {
        const char msg[] = "xdg_surface already has a role object";
        printf("%s\n",msg);
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,msg);
        return;
    }

    wl_resource *toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id); // 4

    surface->p_toplevelRole = surface->compositor()->createToplevelRequest(toplevel, surface);
    surface->pending.type = WSurface::SurfaceType::Toplevel;
    wl_resource_set_implementation(toplevel, &xdg_toplevel_implementation, surface->p_toplevelRole, &Extensions::XdgShell::Toplevel::destroy_resource);

}
void Extensions::XdgShell::Surface::get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{
    (void)parent;(void)positioner;

    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if (surface->type() != WSurface::Undefined)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"xdg_surface already has a role object");
        return;
    }

    WSurface *wParent = (WSurface*)wl_resource_get_user_data(parent);

    if(!wParent)
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT,"xdg_surface.get_popup with invalid popup parent");
        return;
    }

    WPositioner *wPositioner = (WPositioner*)wl_resource_get_user_data(positioner);

    if (!wPositioner)
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_INVALID_POSITIONER,"xdg_surface.get_popup without positioner");
        return;
    }

    if(wPositioner->size().area() <= 0 || wPositioner->anchorRect().area() <= 0)
    {
        wl_resource_post_error(
                    positioner,
                    XDG_WM_BASE_ERROR_INVALID_POSITIONER,
                    "xdg_surface.get_popup with invalid positioner (size: %dx%d, anchorRect: %dx%d)",
                    wPositioner->size().w(),wPositioner->size().h(),wPositioner->anchorRect().w(),wPositioner->anchorRect().h());
        return;
    }

    // Delete previous popup if exists
    if(surface->p_positioner != nullptr)
        delete surface->p_positioner;

    surface->p_positioner = wPositioner;
    surface->p_xdg_popup = wl_resource_create(client, &xdg_popup_interface, wl_resource_get_version(resource), id); // 4
    surface->current.type = WSurface::Popup;
    surface->p_parent = wParent;
    wParent->p_children.push_back(surface);
    surface->parentChangeRequest();
    wl_resource_set_implementation(surface->p_xdg_popup, &xdg_popup_implementation, surface, &Extensions::XdgShell::Popup::destroy_resource);
    surface->typeChangeRequest();
    surface->positionerChangeRequest();
}
void Extensions::XdgShell::Surface::set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if(surface->type() == WSurface::Toplevel)
    {
        surface->toplevel()->p_windowGeometry = WRect(x, y, width, height);
        surface->toplevel()->geometryChangeRequest();
    }
}
void Extensions::XdgShell::Surface::ack_configure(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)serial;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if(surface->type() == WSurface::Toplevel)
    {
        WToplevelRole *topLevel = surface->toplevel();

        if(topLevel->p_sentConf.serial == serial)
        {
            topLevel->p_pendingConf = topLevel->p_sentConf;
            topLevel->p_pendingConf.set = true;
        }
    }

    //printf("ACK serial %i\n",serial);
    surface->ack_configure = true;
    //surface->resizingChanged();
}

