#include "XdgPopup.h"

void xdg_popup_destroy(wl_client *client, wl_resource *resource)
{
    (void)client;(void)resource;
}
void xdg_popup_grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)resource;(void)seat;(void)serial;
}
void xdg_popup_reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 token)
{
    (void)client;(void)resource;(void)positioner;(void)token;
}
