#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <WClient.h>
#include <MyCompositor.h>

using namespace Wpp;

class MyClient : public WClient
{
public:
    MyClient(wl_client *client, WCompositor *compositor);
    ~MyClient();
    WSurface *newSurfaceRequest(wl_resource *surfaceResource) override;
    void surfaceDestroyRequest(WSurface *surface) override;

    MyCompositor *comp;
    //void newRegionRequest(WRegion *region) = 0;
    //void regionDestroyRequest(WRegion *region) = 0;
    //void newPositionerRequest(WPositioner *positioner) = 0;
};

#endif // MYCLIENT_H
