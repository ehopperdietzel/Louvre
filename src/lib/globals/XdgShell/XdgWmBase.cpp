#include "XdgWmBase.h"
#include <xdg-shell.h>
#include <LCompositor.h>
#include <XdgSurface.h>
#include <XdgPositioner.h>
#include <LPositioner.h>

using namespace Louvre;

/* XDG_WM_BASE INTERFACES */

static struct xdg_positioner_interface xdg_positioner_implementation =
{
    &Extensions::XdgShell::Positioner::destroy,
    &Extensions::XdgShell::Positioner::set_size,
    &Extensions::XdgShell::Positioner::set_anchor_rect,
    &Extensions::XdgShell::Positioner::set_anchor,
    &Extensions::XdgShell::Positioner::set_gravity,
    &Extensions::XdgShell::Positioner::set_constraint_adjustment,
    &Extensions::XdgShell::Positioner::set_offset,
    &Extensions::XdgShell::Positioner::set_reactive,
    &Extensions::XdgShell::Positioner::set_parent_size,
    &Extensions::XdgShell::Positioner::set_parent_configure
};

static struct xdg_surface_interface xdg_surface_implementation =
{
    &Extensions::XdgShell::Surface::destroy,
    &Extensions::XdgShell::Surface::get_toplevel,
    &Extensions::XdgShell::Surface::get_popup,
    &Extensions::XdgShell::Surface::set_window_geometry,
    &Extensions::XdgShell::Surface::ack_configure
};

static struct xdg_wm_base_interface xdg_wm_base_implementation =
{
    &Extensions::XdgShell::WmBase::destroy,
    &Extensions::XdgShell::WmBase::create_positioner,
    &Extensions::XdgShell::WmBase::get_xdg_surface,
    &Extensions::XdgShell::WmBase::pong
};

/* XDG_WM_BASE INTERFACE METHODS */

void Extensions::XdgShell::WmBase::resource_destroy(wl_resource *)
{
    /* No user data to free */
}

void Extensions::XdgShell::WmBase::destroy(wl_client *, wl_resource *resource)
{
    // Call WmBase::resource_destroy()
    wl_resource_destroy(resource);
}

void Extensions::XdgShell::WmBase::create_positioner(wl_client *client, wl_resource *resource, UInt32 id)
{
    // XDG_POSITIONER version used by the client
    Int32 version = wl_resource_get_version(resource);

    // Gets the LClient from user data
    LClient *wClient = (LClient*)wl_resource_get_user_data(resource);

    // Creates a new LPositioner
    LPositioner *wPositioner = new LPositioner(wClient);

    // Creates the XDG_POSITIONER resource
    wPositioner->p_resource = wl_resource_create(client, &xdg_positioner_interface, version, id);

    // Binds resource to the XDG_POSITIONER interface and destructor
    wl_resource_set_implementation(wPositioner->p_resource, &xdg_positioner_implementation, wPositioner, &Louvre::Extensions::XdgShell::Positioner::destroy_resource);

}

void Extensions::XdgShell::WmBase::get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *surface)
{
    // Gets the LSurface from user data
    LSurface *wSurface = (LSurface*)wl_resource_get_user_data(surface);

    // The surface should't have a buffer attached
    if(wSurface->pending.buffer != nullptr || wSurface->current.buffer != nullptr)
    {
        wl_resource_post_error(resource,XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"Given wl_surface already has a buffer attached.");
        return;
    }

    // The surface should't have a previous role
    if(wSurface->type() != LSurface::Undefined)
    {
        wl_resource_post_error(resource,XDG_WM_BASE_ERROR_ROLE,"Given wl_surface has another role.");
        return;
    }

    // XDG_SURFACE version used by the client
    Int32 version = wl_resource_get_version(resource);

    // Creates the XDG_SURFACE resource
    wSurface->p_xdg_shell = wl_resource_create(client, &xdg_surface_interface, version, id);

    // Binds the resource to the XDG_SURFACE interface and destructor
    wl_resource_set_implementation(wSurface->p_xdg_shell, &xdg_surface_implementation, wSurface, &Louvre::Extensions::XdgShell::Surface::resource_destroy);
}

void Extensions::XdgShell::WmBase::pong(wl_client *client, wl_resource *resource, UInt32 serial)
{
    /* TODO: NOT NEEDED YET */
    (void)client;(void)resource;(void)serial;
}

void Extensions::XdgShell::WmBase::bind (wl_client *client, void *data, UInt32 version, UInt32 id)
{

    // Gets the LCompositor from user data
    LCompositor *wCompositor = (LCompositor*)data;

    // Search the LClient
    LClient *wClient = nullptr;

    for(LClient *c : wCompositor->clients)
    {
        if(c->client() == client)
        {
            wClient = c;
            break;
        }
    }

    // If the LClient wasn't found
    if(!wClient) return;

    // Creates the XDG_WM_BASE resource
    wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, version, id);

    // Binds the resource to the XDG_WM_BASE interface and destructor
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, wClient, &WmBase::resource_destroy);
}
