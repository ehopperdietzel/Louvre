#ifndef DATADEVICEMANAGER_H
#define DATADEVICEMANAGER_H

#include <wayland-server.h>
#include <WTypes.h>

// Data Device Manager
void dataDeviceManager_create_data_source(wl_client *client, wl_resource *resource, UInt32 id);
void dataDeviceManager_get_data_device(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *seat);
void dataDeviceManager_resource_destroy(wl_resource *resource);
void dataDeviceManager_bind(wl_client *client, void *data, UInt32 version, UInt32 id);

#endif // DATADEVICEMANAGER_H
