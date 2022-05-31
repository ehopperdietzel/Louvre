#include "DataDeviceManager.h"
#include <DataDevice.h>
#include <LCompositor.h>

struct wl_data_device_interface dataDevice_implementation =
{
    &Louvre::Globals::DataDevice::start_drag,
    &Louvre::Globals::DataDevice::set_selection,
    &Louvre::Globals::DataDevice::release
};

struct wl_data_device_manager_interface dataDeviceManager_implementation =
{
    &Louvre::Globals::DataDeviceManager::create_data_source,
    &Louvre::Globals::DataDeviceManager::get_data_device
};

void Louvre::Globals::DataDeviceManager::create_data_source(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
    printf("Create data source.\n");
}
void Louvre::Globals::DataDeviceManager::get_data_device(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *seat)
{
    (void)client;(void)resource;(void)seat;(void)id;
    printf("Get data device.\n");
    //LCompositor *compositor = (LCompositor*)data;
    wl_resource *res = wl_resource_create(client, &wl_data_device_interface, wl_resource_get_version(resource), id); // 3
    wl_resource_set_implementation(res, &dataDevice_implementation, nullptr, NULL);
}
void Louvre::Globals::DataDeviceManager::resource_destroy(wl_resource *resource)
{
    (void)resource;
}
void Louvre::Globals::DataDeviceManager::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    (void)data;
    LCompositor *compositor = (LCompositor*)data;
    printf("Bind data device.\n");
    wl_resource *resource = wl_resource_create(client, &wl_data_device_manager_interface, version, id);
    wl_resource_set_implementation(resource, &dataDeviceManager_implementation, compositor, &DataDeviceManager::resource_destroy);
}
