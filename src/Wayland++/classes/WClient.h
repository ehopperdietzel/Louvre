#ifndef WCLIENT_H
#define WCLIENT_H

#include <list>
#include <wayland-server.h>
#include <WRegion.h>
#include <WSurface.h>

using namespace std;

class WaylandPlus::WClient
{
public:
    WClient(wl_client *client, WCompositor *compositor);
    virtual ~WClient();

    WCompositor *getCompositor();
    wl_client *getClient();
    wl_resource *getKeyboard();
    wl_resource *getPointer();

    // Requests
    virtual WSurface *newSurfaceRequest(UInt32 id, wl_resource *res) = 0;
    virtual void surfaceDestroyRequest(WSurface *surface) = 0;
    //virtual void newRegionRequest(WRegion *region) = 0;
    //virtual void regionDestroyRequest(WRegion *region) = 0;
    //virtual void newPositionerRequest(WPositioner *positioner) = 0;

    void setPointer(wl_resource *pointer);
    void setKeyboard(wl_resource *keyboard);

    list<WRegion*>regions;
    list<WSurface*>surfaces;
    list<WPositioner*>positioners;

    UInt32 getId();
private:
    friend class WSurface;
    friend class Globals::Seat;
    WCompositor *_compositor = nullptr;
    wl_client *_client = nullptr;
    wl_resource *_keyboard = nullptr;
    wl_resource *_pointer = nullptr;
    UInt32 _id;
    Int32 _wl_pointer_version = -1;
};

#endif // WCLIENT_H
