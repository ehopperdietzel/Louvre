#include "Region.h"

#include <WRegion.h>
#include <WClient.h>
#include <stdio.h>

// REGION
void delete_region(wl_resource *resource)
{
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->getClient()->regions.remove(region);
    region->getClient()->regionDestroyed(region);
    delete region;
}

void region_destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    delete_region(resource);
}

void region_add(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->addRect(x,y,width,height);
}

void region_subtract(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->subtractRect(x,y,width,height);
}