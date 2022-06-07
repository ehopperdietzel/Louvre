#include "XdgSurface.h"
#include <XdgToplevel.h>
#include <XdgPopup.h>
#include <xdg-shell.h>
#include <LSurface.h>
#include <LCompositor.h>
#include <LPositioner.h>
#include <LToplevelRole.h>
#include <LPopupRole.h>

using namespace Louvre;

static struct xdg_toplevel_interface xdg_toplevel_implementation =
{
    .destroy = &Extensions::XdgShell::Toplevel::destroy,
    .set_parent = &Extensions::XdgShell::Toplevel::set_parent,
    .set_title = &Extensions::XdgShell::Toplevel::set_title,
    .set_app_id = &Extensions::XdgShell::Toplevel::set_app_id,
    .show_window_menu = &Extensions::XdgShell::Toplevel::show_window_menu,
    .move = &Extensions::XdgShell::Toplevel::move,
    .resize = &Extensions::XdgShell::Toplevel::resize,
    .set_max_size = &Extensions::XdgShell::Toplevel::set_max_size,
    .set_min_size = &Extensions::XdgShell::Toplevel::set_min_size,
    .set_maximized = &Extensions::XdgShell::Toplevel::set_maximized,
    .unset_maximized = &Extensions::XdgShell::Toplevel::unset_maximized,
    .set_fullscreen = &Extensions::XdgShell::Toplevel::set_fullscreen,
    .unset_fullscreen = &Extensions::XdgShell::Toplevel::unset_fullscreen,
    .set_minimized = &Extensions::XdgShell::Toplevel::set_minimized,
#if LOUVRE_XDG_WM_BASE_VERSION >= 4
    .configure_bounds = &Extensions::XdgShell::Toplevel::configure_bounds
#endif
};

static struct xdg_popup_interface xdg_popup_implementation =
{
    .destroy = &Extensions::XdgShell::Popup::destroy,
    .grab = &Extensions::XdgShell::Popup::grab,

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
    .reposition = &Extensions::XdgShell::Popup::reposition
#endif

};

// xdg surface
void Extensions::XdgShell::Surface::resource_destroy(wl_resource *resource)
{
    printf("XDG_SURFACE DESTROYED.\n");
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);
    lSurface->p_xdgSurfaceResource = nullptr;
}

void Extensions::XdgShell::Surface::destroy (wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Extensions::XdgShell::Surface::get_toplevel(wl_client *client, wl_resource *resource, UInt32 id)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(lSurface->pending.buffer != nullptr || lSurface->current.buffer != nullptr)
    {
        wl_resource_post_error(resource,XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"Given wl_surface already has a buffer attached.");
        return;
    }

    if (lSurface->type() != LSurface::Undefined)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"xdg_surface already has a role object.");
        return;
    }

    wl_resource *toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id); // 4

    lSurface->p_role = lSurface->compositor()->createToplevelRequest(toplevel, lSurface);
    lSurface->pending.type = LSurface::SurfaceType::Toplevel;
    wl_resource_set_implementation(toplevel, &xdg_toplevel_implementation, lSurface->p_role, &Extensions::XdgShell::Toplevel::destroy_resource);

}
void Extensions::XdgShell::Surface::get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{

    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(lSurface->pending.buffer != nullptr || lSurface->current.buffer != nullptr)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "Given wl_surface already has a buffer attached.");
        return;
    }

    if (lSurface->type() != LSurface::Undefined)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "xdg_surface already has a role object.");
        return;
    }

    if(!parent)
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_INVALID_POPUP_PARENT, "xdg_surface.get_popup with invalid popup parent.");
        return;
    }

    LSurface *lParent = (LSurface*)wl_resource_get_user_data(parent);
    LPositioner *lPositioner = (LPositioner*)wl_resource_get_user_data(positioner);

    if (!lPositioner)
    {
        wl_resource_post_error(resource, XDG_WM_BASE_ERROR_INVALID_POSITIONER, "xdg_surface.get_popup without positioner.");
        return;
    }

    if(lPositioner->size().area() <= 0 || lPositioner->anchorRect().area() <= 0)
    {
        wl_resource_post_error(
                    positioner,
                    XDG_WM_BASE_ERROR_INVALID_POSITIONER,
                    "xdg_surface.get_popup with invalid positioner (size: %dx%d, anchorRect: %dx%d)",
                    lPositioner->size().w(),lPositioner->size().h(),lPositioner->anchorRect().w(),lPositioner->anchorRect().h());
        return;
    }


    wl_resource *popup = wl_resource_create(client, &xdg_popup_interface, wl_resource_get_version(resource), id);
    LPopupRole *lPopup = lSurface->compositor()->createPopupRequest(popup,lSurface,lPositioner);
    wl_resource_set_implementation(popup, &xdg_popup_implementation, lPopup, &Extensions::XdgShell::Popup::destroy_resource);

    lSurface->pending.type = LSurface::Popup;
    lSurface->p_role = lPopup;
    lSurface->p_parent = lParent;
    lParent->p_children.push_back(lSurface);
    lSurface->parentChangeRequest();

}
void Extensions::XdgShell::Surface::set_window_geometry(wl_client *, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    if(surface->type() == LSurface::Toplevel)
    {
        surface->toplevel()->p_windowGeometry = LRect(x, y, width, height);
        surface->toplevel()->geometryChangeRequest();
    }
    else if(surface->type() == LSurface::Popup)
    {
        surface->popup()->p_windowGeometry = LRect(x, y, width, height);
        surface->popup()->geometryChanged();
    }
    else
    {
        wl_resource_post_error(
                    surface->xdgSurfaceResource(),
                    XDG_SURFACE_ERROR_NOT_CONSTRUCTED,
                    "wl_surface does not have a role yet.");
    }
}
void Extensions::XdgShell::Surface::ack_configure(wl_client *, wl_resource *resource, UInt32 serial)
{
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    if(surface->type() == LSurface::Toplevel)
    {
        LToplevelRole *topLevel = surface->toplevel();

        if(topLevel->p_sentConf.serial == serial)
        {
            topLevel->p_pendingConf = topLevel->p_sentConf;
            topLevel->p_pendingConf.set = true;
        }
    }
    else if(surface->type() == LSurface::Popup)
    {

    }
    else
    {
        wl_resource_post_error(
                    surface->xdgSurfaceResource(),
                    XDG_SURFACE_ERROR_NOT_CONSTRUCTED,
                    "wl_surface does not have a role yet.");
    }

}

