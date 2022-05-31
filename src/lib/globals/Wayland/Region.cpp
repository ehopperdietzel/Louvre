#include "Region.h"

#include <LRegion.h>
#include <LClient.h>
#include <stdio.h>
#include <LCompositor.h>

void Louvre::Globals::Region::resource_destroy(wl_resource *resource)
{
    LRegion *region = (LRegion*)wl_resource_get_user_data(resource);
    region->p_client->regions.remove(region);

    // TODO NOTIFY REGION DESTROY

    delete region;
}

void Louvre::Globals::Region::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Louvre::Globals::Region::add(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    LRegion *region = (LRegion*)wl_resource_get_user_data(resource);
    region->addRect(LRect(x,y,width,height));
}

void Louvre::Globals::Region::subtract(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    LRegion *region = (LRegion*)wl_resource_get_user_data(resource);
    region->subtractRect(LRect(x,y,width,height));
}
