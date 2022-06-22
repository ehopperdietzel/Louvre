#include <LCompositorPrivate.h>
#include <LClientPrivate.h>

// Globals
#include <Compositor.h>
#include <Region.h>
#include <Surface.h>

using namespace std;
using namespace Louvre;

struct wl_surface_interface surface_implementation =
{
    .destroy                = &Globals::Surface::destroy,
    .attach                 = &Globals::Surface::attach,
    .damage                 = &Globals::Surface::damage,
    .frame                  = &Globals::Surface::frame,
    .set_opaque_region      = &Globals::Surface::set_opaque_region,
    .set_input_region       = &Globals::Surface::set_input_region,
    .commit                 = &Globals::Surface::commit,
    .set_buffer_transform   = &Globals::Surface::set_buffer_transform,
    .set_buffer_scale       = &Globals::Surface::set_buffer_scale,
    .damage_buffer          = &Globals::Surface::damage_buffer,
    .offset                 = &Globals::Surface::offset
};

struct wl_region_interface region_implementation =
{
    .destroy = &Globals::Region::destroy,
    .add = &Globals::Region::add,
    .subtract = &Globals::Region::subtract
};

struct wl_compositor_interface compositor_implementation =
{
    .create_surface = &Globals::Compositor::create_surface,
    .create_region = &Globals::Compositor::create_region
};

// COMPOSITOR
void Globals::Compositor::create_surface(wl_client *client, wl_resource *resource, UInt32 id)
{
    Int32 version = wl_resource_get_version(resource);

    // New surface resource
    wl_resource *surface = wl_resource_create(client, &wl_surface_interface, version, id); // 4

    // Find client
    LClient *wClient = (LClient*)wl_resource_get_user_data(resource);

    // Create surface
    LSurface *wSurface = wClient->compositor()->createSurfaceRequest(surface,wClient);

    // Append surface
    wClient->imp()->m_surfaces.push_back(wSurface);
    wClient->compositor()->imp()->m_surfaces.push_back(wSurface);

    // Implement surface
    wl_resource_set_implementation(surface, &surface_implementation, wSurface, &Surface::resource_destroy);

}

void Globals::Compositor::create_region (wl_client *client, wl_resource *resource, UInt32 id)
{
    // New region
    wl_resource *region = wl_resource_create(client, &wl_region_interface, wl_resource_get_version(resource), id);

    // Create region
    LRegion *lRegion = new LRegion();

    // Implement region
    wl_resource_set_implementation(region, &region_implementation, lRegion, &Region::resource_destroy);
}

void Globals::Compositor::resource_destroy(wl_resource *resource)
{
    printf("COMPOSITOR DESTROYED.\n");
}

void Globals::Compositor::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    LCompositor *lCompositor = (LCompositor*)data;

    LClient *lClient = nullptr;

    // Search for the client object
    for(LClient *c : lCompositor->clients())
    {
        if(c->client() == client)
        {
            lClient = c;
            break;
        }
    }

    if(!lClient)
        return;

    wl_resource *resource = wl_resource_create(client, &wl_compositor_interface, version, id);
    wl_resource_set_implementation(resource, &compositor_implementation, lClient, &Compositor::resource_destroy);
}
