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
    LSeat *seat() const;
    wl_client *client() const;
    UInt32 id() const;
    LDataDevice *dataDevice() const;

    const list<LSurface*>&surfaces() const;

    // Global resources
    wl_resource *seatResource() const;
    wl_resource *xdgWmBaseResource() const;

    // Interface resources
    wl_resource *pointerResource() const;
    wl_resource *keyboardResource() const;
    wl_resource *touchResource() const;

    // Serials
    UInt32 lastPointerEnterEventSerial() const;
    UInt32 lastPointerLeaveEventSerial() const;
    UInt32 lastPointerButtonEventSerial() const;

    UInt32 lastKeyboardEnterEventSerial() const;
    UInt32 lastKeyboardLeaveEventSerial() const;
    UInt32 lastKeyboardKeyEventSerial() const;
    UInt32 lastKeyboardModifiersEventSerial() const;

    class LClientPrivate;
    LClientPrivate *imp() const;
private:
    LClientPrivate *m_imp = nullptr;

};

#endif // LCLIENT_H
