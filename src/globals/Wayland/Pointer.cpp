#include "Pointer.h"
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>
#include <WSeat.h>
#include <WPoint.h>

void Wpp::Globals::Pointer::resource_destroy(wl_resource *resource)
{
    printf("POINTER RELEASED\n");
    WClient *client = (WClient*)wl_resource_get_user_data(resource);
    client->p_pointerResource = nullptr;
}

void Wpp::Globals::Pointer::set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    if(surface == nullptr)
        return;

    WSurface *cursorSurface = (WSurface*)wl_resource_get_user_data(surface);
    cursorSurface->current.type = WSurface::SurfaceType::Cursor;
    cursorSurface->p_hotspot = WPoint(hotspot_x,hotspot_y)*cursorSurface->bufferScale();
    cursorSurface->typeChangeRequest();

    wClient->compositor()->seat()->setCursorRequest((WSurface*)wl_resource_get_user_data(surface),hotspot_x*cursorSurface->bufferScale(),hotspot_y*cursorSurface->bufferScale());
}

void Wpp::Globals::Pointer::release(wl_client *client, wl_resource *resource)
{
    (void)client;
    Pointer::resource_destroy(resource);
}
