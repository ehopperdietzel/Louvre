#include "Pointer.h"
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

void WaylandPlus::Globals::Pointer::resource_destroy(wl_resource *resource)
{
    printf("POINTER RELEASED\n");
    WClient *client = (WClient*)wl_resource_get_user_data(resource);
    client->setPointer(nullptr);
}

void WaylandPlus::Globals::Pointer::set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;(void)surface;(void)hotspot_x;(void)hotspot_y;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    if(surface == nullptr)
        return;

    WSurface *cursorSurface = (WSurface*)wl_resource_get_user_data(surface);
    cursorSurface->_type = WaylandPlus::SurfaceType::Cursor;
    cursorSurface->typeChangeRequest();

    wClient->getCompositor()->setCursorRequest((WSurface*)wl_resource_get_user_data(surface),hotspot_x,hotspot_y);
}

void WaylandPlus::Globals::Pointer::release(wl_client *client, wl_resource *resource)
{
    (void)client;
    Pointer::resource_destroy(resource);
}
