#include "XdgWmBase.h"
#include <xdg-shell.h>
#include <WCompositor.h>
#include <XdgSurface.h>
#include <XdgPositioner.h>
#include <WPositioner.h>

using namespace Wpp;

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

void Extensions::XdgShell::WmBase::resource_destroy(wl_resource *resource)
{
    (void)resource;
   printf("XDG WM DESTROYED.\n");
}

void Extensions::XdgShell::WmBase::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    Extensions::XdgShell::WmBase::resource_destroy(resource);
}

void Extensions::XdgShell::WmBase::create_positioner(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
    printf("NEW POSITIONER.\n");

    Int32 version = wl_resource_get_version(resource);
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    WPositioner *positioner = new WPositioner(wClient);
    positioner->p_resource = wl_resource_create(client, &xdg_positioner_interface, version, id);
    wl_resource_set_implementation(positioner->p_resource, &xdg_positioner_implementation, positioner, &Wpp::Extensions::XdgShell::Positioner::destroy_resource);
}
void Extensions::XdgShell::WmBase::get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *surface)
{
    (void)resource;

    // Get surface reference
    WSurface *wSurface = (WSurface*)wl_resource_get_user_data(surface);

    // Check errors
    /*
    if(surface->pending.buffer != nullptr || surface->current.buffer != nullptr)
    {
        wl_resource_post_error(resource,XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED,"Given wl_surface already has a buffer attached.");
        return;
    }
    */

    if(wSurface->type() != WSurface::Undefined)
    {
        wl_resource_post_error(resource,XDG_WM_BASE_ERROR_ROLE,"Given wl_surface has another role.");
        return;
    }

    Int32 version = wl_resource_get_version(resource);
    printf("Xdg Surface version: %i\n",version);
    wSurface->p_xdg_shell = wl_resource_create(client, &xdg_surface_interface, version, id);// 4
    wl_resource_set_implementation(wSurface->p_xdg_shell, &xdg_surface_implementation, wSurface, &Wpp::Extensions::XdgShell::Surface::resource_destroy);
}
void Extensions::XdgShell::WmBase::pong(wl_client *client, wl_resource *resource, UInt32 serial)
{
    (void)client;(void)resource;(void)serial;
}

void Extensions::XdgShell::WmBase::bind (wl_client *client, void *data, UInt32 version, UInt32 id)
{

    printf("Xdg Shell version: %i\n",version);

    WCompositor *compositor = (WCompositor*)data;

    WClient *wClient = nullptr;

    // Check if client already exists
    for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
        if((*c)->client() == client)
        {
            wClient = (*c);
            break;
        }

    wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, version, id);
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, wClient, &WmBase::resource_destroy);
}
