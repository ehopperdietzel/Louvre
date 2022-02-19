#ifndef SEAT_H
#define SEAT_H

#include <wayland-server.h>
#include <WTypes.h>

// SEAT
void seat_get_pointer(wl_client *client, wl_resource *resource, UInt32 id);
void seat_get_keyboard(wl_client *client, wl_resource *resource, UInt32 id);
void seat_get_touch(wl_client *client, wl_resource *resource, UInt32 id);
void seat_release(wl_client *client, wl_resource *resource);
void seat_bind(wl_client *client, void *data, UInt32 version, UInt32 id);

#endif // SEAT_H
