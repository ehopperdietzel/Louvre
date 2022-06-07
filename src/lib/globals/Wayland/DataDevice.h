#ifndef DATADEVICE_H
#define DATADEVICE_H

#include <LNamespaces.h>

class Louvre::Globals::DataDevice
{
public:
    static void resource_destroy(wl_resource *resource);
    static void start_drag(wl_client *client, wl_resource *resource, wl_resource *source, wl_resource *origin, wl_resource *icon, UInt32 serial);
    static void set_selection(wl_client *client, wl_resource *resource, wl_resource *source, UInt32 serial);
    static void release(wl_client *client, wl_resource *resource);
};

#endif // DATADEVICE_H
