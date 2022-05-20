#include "XdgPopup.h"

#include <WCompositor.h>
#include <WSurface.h>
#include <WClient.h>
#include <WPositioner.h>

using namespace Wpp;

void Extensions::XdgShell::Popup::destroy_resource(wl_resource *resource)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if(surface->parent() != nullptr)
        surface->parent()->p_children.remove(surface);

    surface->compositor()->destroySurfaceRequest(surface);

    printf("POPUP DESTROYED.\n");
}

void Extensions::XdgShell::Popup::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Popup::grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)resource;(void)seat;(void)serial;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->grabSeatRequest();
    // TODO add serial
}
void Extensions::XdgShell::Popup::reposition(wl_client *client, wl_resource *surface, wl_resource *positioner, UInt32 token)
{
    (void)client;(void)positioner;(void)token;
    WPositioner *wPositioner = (WPositioner*)wl_resource_get_user_data(positioner);
    WSurface *wSurface = (WSurface*)wl_resource_get_user_data(surface);
    delete wSurface->p_positioner;
    wSurface->p_positioner = wPositioner;
    wSurface->positionerChangeRequest();
}
