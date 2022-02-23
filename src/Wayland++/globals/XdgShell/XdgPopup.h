#ifndef XDGPOPUP_H
#define XDGPOPUP_H

#include <wayland-server.h>
#include <WNamespaces.h>


class WaylandPlus::Extensions::XdgShell::Popup
{
public:
    static void destroy(wl_client *client, wl_resource *resource);
    static void grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial);
    static void reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 token);
};
#endif // XDGPOPUP_H
