#include "Viewporter.h"
#include "viewporter.h"
#include <stdio.h>
#include <list>
#include <LCompositor.h>

using namespace Louvre;
using namespace std;

static struct wp_viewporter_interface wp_viewporter_implementation
{
    .destroy = &Extensions::Viewporter::Viewporter::destroy,
    .get_viewport = &Extensions::Viewporter::Viewporter::get_viewport
};


void Extensions::Viewporter::Viewporter::resource_destroy(wl_resource *resource)
{
    (void)resource;
}

void Extensions::Viewporter::Viewporter::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Extensions::Viewporter::Viewporter::get_viewport(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *surface)
{

}

void Extensions::Viewporter::Viewporter::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    printf("Viewporter version: %i\n",version);

    LCompositor *compositor = (LCompositor*)data;

    LClient *wClient = nullptr;

    // Check if client already exists
    for(list<LClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
        if((*c)->client() == client)
        {
            wClient = (*c);
            break;
        }

    wl_resource *resource = wl_resource_create(client, &wp_viewporter_interface, version, id);
    wl_resource_set_implementation(resource, &wp_viewporter_implementation, wClient, &Viewporter::resource_destroy);
}
