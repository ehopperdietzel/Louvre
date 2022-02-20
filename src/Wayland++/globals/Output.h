#ifndef OUTPUT_H
#define OUTPUT_H

#include <wayland-server.h>
#include <WTypes.h>

// OUTPUT
void output_release(wl_client *client,wl_resource *resource);
void output_resource_destroy(wl_resource *resource);
void output_bind(wl_client *client, void *data, UInt32 version, UInt32 id);

#endif // OUTPUT_H
