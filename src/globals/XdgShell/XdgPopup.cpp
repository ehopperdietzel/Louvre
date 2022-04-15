#include "XdgPopup.h"
#include <WSurface.h>
#include <WClient.h>

using namespace Wpp;

void Extensions::XdgShell::Popup::destroy_resource(wl_resource *resource)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if(surface->parent() != nullptr)
        surface->parent()->_children.remove(surface);

    surface->client()->surfaceDestroyRequest(surface);

}

void Extensions::XdgShell::Popup::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void Extensions::XdgShell::Popup::grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)resource;(void)seat;(void)serial;
}
void Extensions::XdgShell::Popup::reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 token)
{
    (void)client;(void)resource;(void)positioner;(void)token;
}
