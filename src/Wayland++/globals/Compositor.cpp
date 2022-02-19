#include "globals/Compositor.h"
#include <WCompositor.h>

#include <globals/Region.h>
#include <globals/Surface.h>

using namespace std;

struct wl_surface_interface surface_implementation = {&surface_destroy, &surface_attach, &surface_damage, &surface_frame, &surface_set_opaque_region, &surface_set_input_region, &surface_commit, &surface_set_buffer_transform, &surface_set_buffer_scale,&surface_damage_buffer};
struct wl_region_interface region_implementation = {&region_destroy, &region_add, &region_subtract};
struct wl_compositor_interface compositor_implementation = {&compositor_create_surface, &compositor_create_region};

// COMPOSITOR
void compositor_create_surface(wl_client *client, wl_resource *resource, UInt32 id)
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

void compositor_create_region (wl_client *client, wl_resource *resource, UInt32 id)
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

void compositor_resource_destroy(wl_resource *resource)
{
    // Find client
    WClient *client = (WClient*)wl_resource_get_user_data(resource);

    // Remove client from compositor list
    client->getCompositor()->clients.remove(client);

    // Destroy surfaces ( when client crashes )
    while(!client->surfaces.empty())
        delete_surface(client->surfaces.back()->getResource());

    client->surfaces.clear();

    // Destroy regions ( when client crashes )
    while(!client->regions.empty())
        delete_region(client->regions.back()->getResource());

    client->regions.clear();

    // Notify
    client->getCompositor()->clientDisconnected(client);

    // Update screen
    client->getCompositor()->repaint();

    // Destroy client
    delete client;
}

void compositor_bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{

    WCompositor *compositor = (WCompositor*)data;

    // Check if client already exists
    for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
        if((*c)->getClient() == client)
            return;

    wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, version, id);
    WClient *wClient = new WClient(client,resource,compositor);
    wl_resource_set_implementation(resource, &compositor_implementation, wClient, &compositor_resource_destroy);
}
