#include "Pointer.h"
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

void WaylandPlus::Globals::Pointer::set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;(void)surface;(void)hotspot_x;(void)hotspot_y;
    WCompositor *compositor = (WCompositor*)wl_resource_get_user_data(resource);
    compositor->cursorSurface = (WSurface*)wl_resource_get_user_data(surface);
    compositor->cursorXOffset = hotspot_x;
    compositor->cursorYOffset = hotspot_y;

    compositor->setCursorRequest(compositor->cursorSurface,compositor->cursorXOffset,compositor->cursorYOffset);
}

void WaylandPlus::Globals::Pointer::release(wl_client *client, wl_resource *resource)
{
    //printf("POINTER RELEASED\n");
    (void)client;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wClient->setPointer(nullptr);
}
