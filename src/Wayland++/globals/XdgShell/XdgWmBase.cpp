#include "XdgWmBase.h"
#include <xdg-shell.h>
#include <WCompositor.h>
#include <XdgSurface.h>
#include <XdgPositioner.h>
#include <WPositioner.h>

using namespace WaylandPlus;

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
    printf("Xdg shell resource destroyed.\n");
}

void Extensions::XdgShell::WmBase::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
    printf("Xdg shell destroyed.\n");
}

void Extensions::XdgShell::WmBase::create_positioner(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;

    Int32 version = wl_resource_get_version(resource);
    printf("Xdg Positioner version: %i\n",version);

    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    WPositioner *positioner = new WPositioner(wClient);
    positioner->_resource = wl_resource_create(client, &xdg_positioner_interface, version, id);
    wl_resource_set_implementation(positioner->_resource, &xdg_positioner_implementation, positioner, &WaylandPlus::Extensions::XdgShell::Positioner::destroy_resource);
}
void Extensions::XdgShell::WmBase::get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *_surface)
{
    (void)resource;

    Int32 version = wl_resource_get_version(resource);
    printf("Xdg Surface version: %i\n",version);

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(_surface);
    surface->xdg_shell = wl_resource_create(client, &xdg_surface_interface, version, id);// 4
    wl_resource_set_implementation(surface->xdg_shell, &xdg_surface_implementation, surface, NULL);
    xdg_surface_send_configure(surface->xdg_shell, 0);
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
        if((*c)->getClient() == client)
        {
            wClient = (*c);
            break;
        }

    if(wClient == nullptr)
        wClient = new WClient(client,compositor);

    wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, version, id);
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, wClient, &WmBase::resource_destroy);
}
