#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>

class WClient;

class WSurface
{
public:
    WSurface(wl_resource *res);

    WClient *client;

    wl_resource *resource;
    wl_resource *frame_callback;
    wl_resource *buffer;
};

#endif // WSURFACE_H
