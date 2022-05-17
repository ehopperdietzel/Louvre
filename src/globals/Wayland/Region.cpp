#include "Region.h"

#include <WRegion.h>
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

void Wpp::Globals::Region::resource_destroy(wl_resource *resource)
{
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->p_client->regions.remove(region);

    // TODO NOTIFY REGION DESTROY

    delete region;
}

void Wpp::Globals::Region::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Wpp::Globals::Region::add(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->addRect(WRect(x,y,width,height));
}

void Wpp::Globals::Region::subtract(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->subtractRect(WRect(x,y,width,height));
}
