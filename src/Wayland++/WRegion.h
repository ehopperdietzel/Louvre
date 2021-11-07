#ifndef WREGION_H
#define WREGION_H

#include <wayland-server.h>

class WRegion
{
public:
    WRegion(wl_resource *res);
    wl_resource *resource;
};

#endif // WREGION_H
