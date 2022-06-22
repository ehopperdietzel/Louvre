#ifndef LPOPUPROLE_H
#define LPOPUPROLE_H

#include <LBaseSurfaceRole.h>
#include <LRect.h>

class Louvre::LPopupRole : public LBaseSurfaceRole
{
public:
    LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner);
    virtual ~LPopupRole();

    const LPoint &rolePos() const override;

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
    LPositioner *positioner() const;

private:
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Popup;

    LRect m_windowGeometry;

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
    UInt32 m_repositionSerial = 0;
#endif

    LPositioner *m_positioner = nullptr;


};

#endif // LPOPUPROLE_H
