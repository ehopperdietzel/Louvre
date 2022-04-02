#include "Pointer.h"
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

void WaylandPlus::Globals::Pointer::set_cursor(wl_client *client, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    (void)client;(void)resource;(void)serial;(void)surface;(void)hotspot_x;(void)hotspot_y;
    WCompositor *compositor = (WCompositor*)wl_resource_get_user_data(resource);

    compositor->renderMutex.lock();

    if(surface == nullptr)
    {
        compositor->setCursorRequest(nullptr,0,0);
        compositor->renderMutex.unlock();
        return;
    }


    compositor->setCursorRequest((WSurface*)wl_resource_get_user_data(surface),hotspot_x,hotspot_y);
    compositor->renderMutex.unlock();
}

void WaylandPlus::Globals::Pointer::release(wl_client *client, wl_resource *resource)
{
    //printf("POINTER RELEASED\n");
    (void)client;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wClient->getCompositor()->renderMutex.lock();
    wClient->setPointer(nullptr);
    wClient->getCompositor()->renderMutex.unlock();
}
