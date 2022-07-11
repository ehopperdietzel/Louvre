#include "XdgSurface.h"
#include <XdgToplevel.h>
#include <XdgPopup.h>
#include <xdg-shell.h>
#include <LSurfacePrivate.h>
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
    lSurface->imp()->m_xdgSurfaceResource = nullptr;
}

void Extensions::XdgShell::Surface::destroy (wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Extensions::XdgShell::Surface::get_toplevel(wl_client *client, wl_resource *resource, UInt32 id)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    printf("CREATE TOPLEVEL\n");

    if(lSurface->imp()->pending.buffer != nullptr || lSurface->imp()->current.buffer != nullptr)
    {
        wl_resource_post_error(resource,XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"Given wl_surface already has a buffer attached.");
        return;
    }

    if (lSurface->roleType() != LSurface::Undefined)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"xdg_surface already has a role object.");
        return;
    }

    wl_resource *toplevel = wl_resource_create(client, &xdg_toplevel_interface, wl_resource_get_version(resource), id); // 4

    lSurface->imp()->m_role = lSurface->compositor()->createToplevelRequest(toplevel, lSurface);
    lSurface->imp()->pending.type = LSurface::Toplevel;
    wl_resource_set_implementation(toplevel, &xdg_toplevel_implementation, lSurface->imp()->m_role, &Extensions::XdgShell::Toplevel::destroy_resource);

}
void Extensions::XdgShell::Surface::get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner)
{

    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(lSurface->imp()->pending.buffer != nullptr || lSurface->imp()->current.buffer != nullptr)
    {
        wl_resource_post_error(resource, XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "Given wl_surface already has a buffer attached.");
        return;
    }

    if (lSurface->roleType() != LSurface::Undefined)
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

    lSurface->imp()->pending.type = LSurface::Popup;
    lSurface->imp()->m_role = lPopup;
    lSurface->imp()->m_parent = lParent;
    lParent->imp()->m_children.push_back(lSurface);
    lSurface->parentChanged();

}
void Extensions::XdgShell::Surface::set_window_geometry(wl_client *, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    if(surface->roleType() == LSurface::Toplevel)
    {
        surface->toplevel()->m_windowGeometry = LRect(x, y, width, height);
        surface->toplevel()->geometryChanged();
    }
    else if(surface->roleType() == LSurface::Popup)
    {
        surface->popup()->m_windowGeometry = LRect(x, y, width, height);
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

    if(surface->roleType() == LSurface::Toplevel)
    {
        LToplevelRole *topLevel = surface->toplevel();

        if(topLevel->m_sentConf.serial == serial)
        {
            topLevel->m_pendingConf = topLevel->m_sentConf;
            topLevel->m_pendingConf.set = true;
        }
    }
    else if(surface->roleType() == LSurface::Popup)
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

