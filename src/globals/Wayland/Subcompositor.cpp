#include "Subcompositor.h"
#include <Subsurface.h>
#include <stdio.h>

using namespace WaylandPlus;

struct wl_subsurface_interface subsurface_implementation =
{
    &Globals::Subsurface::destroy,
    &Globals::Subsurface::set_position,
    &Globals::Subsurface::place_above,
    &Globals::Subsurface::place_below,
    &Globals::Subsurface::set_sync,
    &Globals::Subsurface::set_desync
};

struct wl_subcompositor_interface subcompositor_implementation =
{
    &Globals::Subcompositor::destroy,
    &Globals::Subcompositor::get_subsurface
};

void Globals::Subcompositor::destroy(wl_client *client, wl_resource *resource)
{

}

void Globals::Subcompositor::get_subsurface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *subSurface, wl_resource *parent)
{
    //printf("New subsurface.\n");
}

void Globals::Subcompositor::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    //printf("New Subcompositor.\n");
    WCompositor *compositor = (WCompositor*)data;
    wl_resource *resource = wl_resource_create(client, &wl_subcompositor_interface, version, id);
    wl_resource_set_implementation(resource, &subcompositor_implementation, compositor, NULL);
}
