#ifndef POINTER_H
#define POINTER_H

#include <wayland-server.h>
#include <WTypes.h>

// POINTER
void pointer_set_cursor (wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *_surface, Int32 hotspot_x, Int32 hotspot_y);
void pointer_release(wl_client *client, wl_resource *resource);

#endif // POINTER_H
