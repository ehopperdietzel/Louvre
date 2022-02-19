#ifndef XDGPOPUP_H
#define XDGPOPUP_H

#include <wayland-server.h>
#include <WTypes.h>

void xdg_popup_destroy(wl_client *client, wl_resource *resource);
void xdg_popup_grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial);
void xdg_popup_reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 token);
#endif // XDGPOPUP_H
