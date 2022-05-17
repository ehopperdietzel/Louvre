#ifndef WCLIENT_H
#define WCLIENT_H

#include <list>
#include <WRegion.h>
#include <WSurface.h>

using namespace std;

class Wpp::WClient
{
public:
    WClient(wl_client *client, WCompositor *compositor);
    virtual ~WClient();

    WCompositor *compositor() const;
    wl_client *client() const;
    UInt32 id() const;

    wl_resource *seatResource() const;
    wl_resource *pointerResource() const;
    wl_resource *keyboardResource() const;
    wl_resource *touchResource() const;

    // Requests
    virtual WSurface *newSurfaceRequest(wl_resource *surface) = 0;
    virtual void surfaceDestroyRequest(WSurface *surface) = 0;
    //virtual void newRegionRequest(WRegion *region) = 0;
    //virtual void regionDestroyRequest(WRegion *region) = 0;
    //virtual void newPositionerRequest(WPositioner *positioner) = 0;

    list<WRegion*>regions;
    list<WSurface*>surfaces;
    list<WPositioner*>positioners;



private:
    friend class WSurface;
    friend class WPointer;
    friend class Globals::Seat;
    friend class Globals::Compositor;
    friend class Globals::Keyboard;
    friend class Globals::Pointer;

    WCompositor         *p_compositor               = nullptr;
    wl_client           *p_client                   = nullptr;

    wl_resource         *p_seatResource             = nullptr;
    wl_resource         *p_pointerResource          = nullptr;
    wl_resource         *p_keyboardResource         = nullptr;
    wl_resource         *p_touchResource            = nullptr;

    WSurface            *p_pointerFocusedSurface    = nullptr;
    WSurface            *p_keyboardFocusedSurface   = nullptr;
    WSurface            *p_touchFocusedSurface      = nullptr;
    UInt32 p_id;

};

#endif // WCLIENT_H
