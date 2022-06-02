#ifndef LCLIENT_H
#define LCLIENT_H

#include <list>
#include <LRegion.h>
#include <LSurface.h>

using namespace std;

class Louvre::LClient
{
public:
    LClient(wl_client *client, LCompositor *compositor);
    virtual ~LClient();

    LCompositor *compositor() const;
    wl_client *client() const;
    UInt32 id() const;

    const list<LSurface*>&surfaces() const;

    wl_resource *seatResource() const;
    wl_resource *pointerResource() const;
    wl_resource *keyboardResource() const;
    wl_resource *touchResource() const;

    wl_resource *xdgWmBaseResource() const;

    // Requests
    //virtual void newRegionRequest(LRegion *region) = 0;
    //virtual void regionDestroyRequest(LRegion *region) = 0;
    //virtual void newPositionerRequest(LPositioner *positioner) = 0;

    list<LRegion*>regions;
    list<LPositioner*>positioners;



private:
    friend class LWayland;
    friend class LSurface;
    friend class LPointer;
    friend class Globals::Surface;
    friend class Globals::Seat;
    friend class Globals::Compositor;
    friend class Globals::Keyboard;
    friend class Globals::Pointer;
    friend class Extensions::XdgShell::WmBase;

    list<LSurface*>p_surfaces;

    LCompositor         *p_compositor               = nullptr;
    wl_client           *p_client                   = nullptr;

    wl_resource         *p_seatResource             = nullptr;
    wl_resource         *p_pointerResource          = nullptr;
    wl_resource         *p_keyboardResource         = nullptr;
    wl_resource         *p_touchResource            = nullptr;
    wl_resource         *p_xdgWmBaseResource        = nullptr;

    LSurface            *p_pointerFocusedSurface    = nullptr;
    LSurface            *p_keyboardFocusedSurface   = nullptr;
    LSurface            *p_touchFocusedSurface      = nullptr;
    UInt32 p_id;

};

#endif // LCLIENT_H
