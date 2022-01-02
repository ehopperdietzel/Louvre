#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <wayland-server.h>
#include <WTypes.h>


// COMPOSITOR
void compositor_create_surface(wl_client *client, wl_resource *resource, UInt32 id);
void compositor_create_region (wl_client *client, wl_resource *resource, UInt32 id);
void compositor_client_disconect(wl_resource *resource);
void compositor_bind(wl_client *client, void *data, UInt32 version, UInt32 id);


#endif // COMPOSITOR_H
