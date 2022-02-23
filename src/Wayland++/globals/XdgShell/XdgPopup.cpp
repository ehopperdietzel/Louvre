#include "XdgPopup.h"

using namespace WaylandPlus;

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
