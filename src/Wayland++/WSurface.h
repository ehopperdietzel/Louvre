#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>

class WClient;

class WSurface
{
public:
    WSurface(wl_resource *res);

    WTexture *texture = new WTexture();

    WClient *client = nullptr;

    wl_resource *resource = nullptr;
    wl_resource *frame_callback = nullptr;
    wl_resource *buffer = nullptr;

    wl_resource *xdg_shell = nullptr;
    wl_resource *xdg_toplevel = nullptr;
    wl_resource *xdg_popup = nullptr;
};

#endif // WSURFACE_H
