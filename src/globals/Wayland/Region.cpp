#include "Region.h"

#include <WRegion.h>
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

void WaylandPlus::Globals::Region::resource_destroy(wl_resource *resource)
{
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->getClient()->regions.remove(region);
    //region->getClient()->regionDestroyed(region);
    //delete region;
}

void WaylandPlus::Globals::Region::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    Region::resource_destroy(resource);
}

void WaylandPlus::Globals::Region::add(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->addRect(x,y,width,height);
}

void WaylandPlus::Globals::Region::subtract(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->subtractRect(x,y,width,height);
}
