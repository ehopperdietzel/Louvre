#ifndef WREGION_H
#define WREGION_H

#include <wayland-server.h>
#include <list>
#include <WNamespaces.h>

using namespace std;

class Wpp::WRegion
{
public:
    WRegion(UInt32 id, wl_resource *resource, WClient *client);

    wl_resource *getResource();
    UInt32 getId();
    WClient *getClient();

    void addRect(Int32 x, Int32 y, Int32 width, Int32 height);
    void subtractRect(Int32 x, Int32 y, Int32 width, Int32 height);
    list<WRegionRect>rects;
private:
    UInt32 _id;
    wl_resource *_resource = nullptr;
    WClient *_client = nullptr;
};

#endif // WREGION_H
