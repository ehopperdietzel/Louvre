#ifndef LPOPUPROLE_H
#define LPOPUPROLE_H

#include <LNamespaces.h>
#include <LRect.h>

class Louvre::LPopupRole
{
public:
    LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner);
    virtual ~LPopupRole();

    // Requests
    virtual void pong(UInt32 serial);
    virtual void grabSeatRequest();
    virtual void configureRequest();

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
    virtual void repositionRequest();
#endif

    // Notifications
    virtual void geometryChanged();

    // Events
    void ping(UInt32 serial);
    void configure(const LRect &rect);
    void sendPopupDoneEvent();

    // Variables
    const LRect &windowGeometry() const;
    LCompositor *compositor() const;
    LPositioner *positioner() const;
    LSurface *surface() const;
    LSeat *seat() const;
    wl_resource *resource() const;
private:
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Popup;

    LRect p_windowGeometry;

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
    UInt32 p_repositionSerial = 0;
#endif

    LCompositor *p_compositor = nullptr;
    LPositioner *p_positioner = nullptr;
    LSurface *p_surface = nullptr;
    wl_resource *p_resource = nullptr;

};

#endif // LPOPUPROLE_H
