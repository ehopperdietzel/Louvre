#include <Compositor.h>
#include <WCompositor.h>

#include <Region.h>
#include <Surface.h>

using namespace std;
using namespace WaylandPlus;

struct wl_surface_interface surface_implementation =
{
    &Globals::Surface::destroy,
    &Globals::Surface::attach,
    &Globals::Surface::damage,
    &Globals::Surface::frame,
    &Globals::Surface::set_opaque_region,
    &Globals::Surface::set_input_region,
    &Globals::Surface::commit,
    &Globals::Surface::set_buffer_transform,
    &Globals::Surface::set_buffer_scale,
    &Globals::Surface::damage_buffer
};

struct wl_region_interface region_implementation =
{
    &Globals::Region::destroy,
    &Globals::Region::add,
    &Globals::Region::subtract
};

struct wl_compositor_interface compositor_implementation =
{
    &Globals::Compositor::create_surface,
    &Globals::Compositor::create_region
};

// COMPOSITOR
void Globals::Compositor::create_surface(wl_client *client, wl_resource *resource, UInt32 id)
{
    printf("NEW SURFACE! %i\n",id);

    // New surface resource
    wl_resource *surface = wl_resource_create (client, &wl_surface_interface, 4, id);

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Create surface
    WSurface *wSurface = new WSurface(id,surface,wClient);

    // Append surface
    wClient->surfaces.push_back(wSurface);

    // Notify from compositor
    wClient->getCompositor()->newSurface(wSurface);

    // Implement surface
    wl_resource_set_implementation(surface, &surface_implementation, wSurface, NULL);
}

void Globals::Compositor::create_region (wl_client *client, wl_resource *resource, UInt32 id)
{
    // New region
    wl_resource *region = wl_resource_create(client, &wl_region_interface, 1, id);

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Create region
    WRegion *wRegion = new WRegion(id,region,wClient);

    // Append region to client
    wClient->regions.push_back(wRegion);

    // Implement region
    wl_resource_set_implementation(region, &region_implementation, wRegion, NULL);
}

void Globals::Compositor::resource_destroy(wl_resource *resource)
{
    // Find client
    WClient *client = (WClient*)wl_resource_get_user_data(resource);

    // Remove client from compositor list
    client->getCompositor()->clients.remove(client);

    // Destroy surfaces ( when client crashes )
    while(!client->surfaces.empty())
        Globals::Surface::delete_surface(client->surfaces.back()->getResource());

    client->surfaces.clear();

    // Destroy regions ( when client crashes )
    while(!client->regions.empty())
        Globals::Region::remove(client->regions.back()->getResource());

    client->regions.clear();

    // Notify
    client->getCompositor()->clientDisconnected(client);

    // Update screen
    client->getCompositor()->repaint();

    // Destroy client
    delete client;
}

void Globals::Compositor::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{

    WCompositor *compositor = (WCompositor*)data;

    // Check if client already exists
    for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
        if((*c)->getClient() == client)
            return;

    wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, version, id);
    WClient *wClient = new WClient(client,resource,compositor);
    wl_resource_set_implementation(resource, &compositor_implementation, wClient, &Compositor::resource_destroy);
}
