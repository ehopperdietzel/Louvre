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
    Int32 version = wl_resource_get_version(resource);

    //printf("Surface version: %i\n",version);

    // New surface resource
    wl_resource *surface = wl_resource_create(client, &wl_surface_interface, version, id); // 4

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Create surface
    WSurface *wSurface = wClient->newSurfaceRequest(surface);

    // Append surface
    wClient->surfaces.push_back(wSurface);

    // Implement surface
    wl_resource_set_implementation(surface, &surface_implementation, wSurface, &Surface::resource_destroy);

}

void Globals::Compositor::create_region (wl_client *client, wl_resource *resource, UInt32 id)
{
    // New region
    wl_resource *region = wl_resource_create(client, &wl_region_interface, wl_resource_get_version(resource), id); // 1

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Create region
    WRegion *wRegion = new WRegion(id,region,wClient);

    // Append region to client
    wClient->regions.push_back(wRegion);

    // Implement region
    wl_resource_set_implementation(region, &region_implementation, wRegion, &Region::resource_destroy);
}

void Globals::Compositor::resource_destroy(wl_resource *resource)
{

    printf("COMPOSITOR DESTROYED.\n");

    // Find client
    WClient *client = (WClient*)wl_resource_get_user_data(resource);

    /*

    // Destroy surfaces ( when client crashes )
    while(!client->surfaces.empty())
        Globals::Surface::delete_surface(client->surfaces.back()->getResource());

    client->surfaces.clear();

    // Destroy regions ( when client crashes )
    while(!client->regions.empty())
        Globals::Region::remove(client->regions.back()->getResource());

    client->regions.clear();
    */





    // Notify
    //11client->getCompositor()->clientDisconnectRequest(client);

    // Destroy client
    //delete client;


}

void Globals::Compositor::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    printf("Compositor version: %i\n",version);

    WCompositor *compositor = (WCompositor*)data;

    WClient *wClient = nullptr;

    // Search for the client object
    for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
    {
        if((*c)->getClient() == client)
        {
            wClient = (*c);
            break;
        }
    }

    if(wClient == nullptr)
        return;

    wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, version, id);
    wl_resource_set_implementation(resource, &compositor_implementation, wClient, &Compositor::resource_destroy);
}
