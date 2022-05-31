#ifndef LPOPUP_H
#define LPOPUP_H

#include <LNamespaces.h>
#include <LRect.h>

class Louvre::LPopupRole
{
public:
    LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner);
    virtual ~LPopupRole();

    // Requests
    virtual void configureRequest();
    virtual void repositionRequest();

    // Notifications
    virtual void geometryChanged();

    // Events
    void configure(const LRect &rect);

    // Variables
    const LRect &windowGeometry() const;
    LPositioner *positioner() const;
    LSurface *surface() const;
    wl_resource *resource() const;
private:

    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Popup;

    LRect p_windowGeometry;
    UInt32 p_repositionSerial = 0;
    LPositioner *p_positioner = nullptr;
    LSurface *p_surface = nullptr;
    wl_resource *p_resource = nullptr;

};

#endif // LPOPUP_H
