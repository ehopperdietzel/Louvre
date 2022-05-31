#include "Pointer.h"
#include <LClient.h>
#include <stdio.h>
#include <LCompositor.h>
#include <LSeat.h>
#include <LPoint.h>

void Louvre::Globals::Pointer::resource_destroy(wl_resource *resource)
{
    printf("POINTER RELEASED\n");
    LClient *client = (LClient*)wl_resource_get_user_data(resource);
    client->p_pointerResource = nullptr;
}

void Louvre::Globals::Pointer::set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;
    LClient *wClient = (LClient*)wl_resource_get_user_data(resource);

    if(surface == nullptr)
        return;

    LSurface *cursorSurface = (LSurface*)wl_resource_get_user_data(surface);
    cursorSurface->current.type = LSurface::SurfaceType::Cursor;
    cursorSurface->p_hotspot = LPoint(hotspot_x,hotspot_y)*cursorSurface->bufferScale();
    cursorSurface->typeChangeRequest();

    wClient->compositor()->seat()->setCursorRequest((LSurface*)wl_resource_get_user_data(surface),hotspot_x*cursorSurface->bufferScale(),hotspot_y*cursorSurface->bufferScale());
}

void Louvre::Globals::Pointer::release(wl_client *client, wl_resource *resource)
{
    (void)client;
    Pointer::resource_destroy(resource);
}
