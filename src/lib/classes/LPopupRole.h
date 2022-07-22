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

    class LPopupRolePrivate;

    LPopupRolePrivate *imp() const;

private:
    LPopupRolePrivate *m_imp = nullptr;




};

#endif // LPOPUPROLE_H
