#include "DataDevice.h"
#include <stdio.h>

void Louvre::Globals::DataDevice::resource_destroy(wl_resource *resource)
{
    printf("DATA DEVICE DESTROYED.\n");
}

void Louvre::Globals::DataDevice::release(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Louvre::Globals::DataDevice::start_drag(wl_client *client, wl_resource *resource, wl_resource *source, wl_resource *origin, wl_resource *icon, UInt32 serial)
{

}

void Louvre::Globals::DataDevice::set_selection(wl_client *client, wl_resource *resource, wl_resource *source, UInt32 serial)
{

}


