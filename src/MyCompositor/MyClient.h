#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <WClient.h>

using namespace WaylandPlus;

class MyClient : public WClient
{
public:
    MyClient(wl_client *client, WCompositor *compositor);

    WSurface *newSurfaceRequest(UInt32 id, wl_resource *resource) override;
    void surfaceDestroyRequest(WSurface *surface) override;
    //void newRegionRequest(WRegion *region) = 0;
    //void regionDestroyRequest(WRegion *region) = 0;
    //void newPositionerRequest(WPositioner *positioner) = 0;
};

#endif // MYCLIENT_H
