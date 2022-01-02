#ifndef SEAT_H
#define SEAT_H

#include <wayland-server.h>
#include <WTypes.h>

// POINTER
void pointer_set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *_surface, Int32 hotspot_x, Int32 hotspot_y);
void pointer_release(wl_client *client, wl_resource *resource);

// KEYBOARD
void keyboard_release(wl_client *client, wl_resource *resource);

// SEAT
void seat_get_pointer(wl_client *client, wl_resource *resource, UInt32 id);
void seat_get_keyboard(wl_client *client, wl_resource *resource, UInt32 id);
void seat_get_touch(wl_client *client, wl_resource *resource, UInt32 id);
void seat_release(wl_client *client, wl_resource *resource);
void seat_bind(wl_client *client, void *data, UInt32 version, UInt32 id);

#endif // SEAT_H
