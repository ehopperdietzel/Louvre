#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <wayland-server.h>
#include <WNamespaces.h>

class WaylandPlus::Globals::Keyboard
{
public:
    static void resource_destroy(wl_resource *resource);
    static void release(wl_client *client, wl_resource *resource);
};

#endif // KEYBOARD_H