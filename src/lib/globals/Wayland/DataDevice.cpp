#include "DataDevice.h"


void Louvre::Globals::DataDevice::start_drag(wl_client *client, wl_resource *resource, wl_resource *source, wl_resource *origin, wl_resource *icon, UInt32 serial)
{

}

void Louvre::Globals::DataDevice::set_selection(wl_client *client, wl_resource *resource, wl_resource *source, UInt32 serial)
{

}

void Louvre::Globals::DataDevice::release(wl_client *client, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Louvre::Globals::DataDevice::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{

}
