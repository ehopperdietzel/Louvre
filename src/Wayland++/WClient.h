#ifndef WCLIENT_H
#define WCLIENT_H

#include <list>
#include <wayland-server.h>
#include <WRegion.h>
#include <WSurface.h>

using namespace std;

class WClient
{
public:
    WClient(wl_client *cli,wl_resource *res);
    wl_client *client;
    wl_resource *resource;

    list<WRegion*>regions;
    list<WSurface*>surfaces;
};

#endif // WCLIENT_H
