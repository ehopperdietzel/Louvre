#ifndef WREGION_H
#define WREGION_H

#include <wayland-server.h>

class WClient;

class WRegion
{
public:
    WRegion(wl_resource *res);
    wl_resource *resource;
    WClient *client = nullptr;
};

#endif // WREGION_H
