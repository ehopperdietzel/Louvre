#include "Pointer.h"
#include <WClient.h>
#include <stdio.h>

// POINTER
void pointer_set_cursor (wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *_surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;(void)_surface;(void)hotspot_x;(void)hotspot_y;
    printf("CURSOR SURFACE\n");
    //struct surface *surface = wl_resource_get_user_data(_surface);
    //cursor = surface;
}

void pointer_release(wl_client *client, wl_resource *resource)
{
    printf("POINTER RELEASED\n");
    (void)client;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wClient->setPointer(nullptr);
}
