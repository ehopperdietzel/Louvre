#ifndef REGION_H
#define REGION_H

#include <wayland-server.h>
#include <WTypes.h>

// REGION
void delete_region(wl_resource *resource);
void region_destroy(wl_client *client, wl_resource *resource);
void region_add(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);
void region_subtract(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height);

#endif // REGION_H
