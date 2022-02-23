#include "DataDeviceManager.h"
#include <WCompositor.h>

struct wl_data_device_manager_interface dataDeviceManager_implementation =
{
    &WaylandPlus::Globals::DataDeviceManager::create_data_source,
    &WaylandPlus::Globals::DataDeviceManager::get_data_device
};

void WaylandPlus::Globals::DataDeviceManager::create_data_source(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
}
void WaylandPlus::Globals::DataDeviceManager::get_data_device(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *seat)
{
    (void)client;(void)resource;(void)seat;(void)id;
}
void WaylandPlus::Globals::DataDeviceManager::resource_destroy(wl_resource *resource)
{
    (void)resource;
}
void WaylandPlus::Globals::DataDeviceManager::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    (void)data;
    //WCompositor *compositor = (WCompositor*)data;
    wl_resource *resource = wl_resource_create(client, &wl_data_device_manager_interface, version, id);
    wl_resource_set_implementation(resource, &dataDeviceManager_implementation, nullptr, &resource_destroy);
}
