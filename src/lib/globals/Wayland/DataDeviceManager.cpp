#include "DataDeviceManager.h"
#include <DataSource.h>
#include <DataDevice.h>
#include <LCompositor.h>

struct wl_data_source_interface dataSource_implementation =
{
    .offer = &Louvre::Globals::DataSource::offer,
    .destroy = &Louvre::Globals::DataSource::destroy,
    .set_actions = &Louvre::Globals::DataSource::set_actions,
};

struct wl_data_device_interface dataDevice_implementation =
{
    .start_drag = &Louvre::Globals::DataDevice::start_drag,
    .set_selection = &Louvre::Globals::DataDevice::set_selection,
    .release = &Louvre::Globals::DataDevice::release
};

struct wl_data_device_manager_interface dataDeviceManager_implementation =
{
    .create_data_source = &Louvre::Globals::DataDeviceManager::create_data_source,
    .get_data_device = &Louvre::Globals::DataDeviceManager::get_data_device
};

void Louvre::Globals::DataDeviceManager::resource_destroy(wl_resource *)
{
    printf("DATA DEVICE MANAGER DESTROYED.\n");
}

void Louvre::Globals::DataDeviceManager::create_data_source(wl_client *client, wl_resource *resource, UInt32 id)
{
    LCompositor *lCompositor = (LCompositor*)wl_resource_get_user_data(resource);
    UInt32 version = wl_resource_get_version(resource);
    wl_resource *dataSource = wl_resource_create(client, &wl_data_source_interface, version, id);
    wl_resource_set_implementation(dataSource, &dataDevice_implementation, lCompositor, &DataDevice::resource_destroy);
}
void Louvre::Globals::DataDeviceManager::get_data_device(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *seat)
{
    LCompositor *lCompositor = (LCompositor*)wl_resource_get_user_data(resource);
    UInt32 version = wl_resource_get_version(resource);
    wl_resource *dataDevice = wl_resource_create(client, &wl_data_device_interface, version, id);
    wl_resource_set_implementation(dataDevice, &dataDevice_implementation, lCompositor, &DataDevice::resource_destroy);
}

void Louvre::Globals::DataDeviceManager::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    LCompositor *lCompositor = (LCompositor*)data;
    wl_resource *resource = wl_resource_create(client, &wl_data_device_manager_interface, version, id);
    wl_resource_set_implementation(resource, &dataDeviceManager_implementation, lCompositor, &DataDeviceManager::resource_destroy);
}
