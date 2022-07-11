#include "LPopupRole.h"
#include <LRect.h>
#include <LSurfacePrivate.h>
#include <LBaseSurfaceRolePrivate.h>
#include <LWayland.h>
#include <LPositioner.h>
#include <LCompositor.h>
#include <LOutput.h>
#include <LSeat.h>
#include <xdg-shell.h>

#include <LPointer.h>
#include <LKeyboard.h>

using namespace Louvre;


Louvre::LPopupRole::LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner) : LBaseSurfaceRole(popup,surface)
{
    m_positioner = positioner;
}

LPopupRole::~LPopupRole()
{

}

const LPoint &LPopupRole::rolePos() const
{
    baseImp()->rolePos = surface()->pos() - m_windowGeometry.topLeft();
    return baseImp()->rolePos;
}

void LPopupRole::pong(UInt32)
{

}

void LPopupRole::grabSeatRequest()
{
    seat()->pointer()->setFocus(surface());
    seat()->keyboard()->setFocus(surface());
}

void LPopupRole::configureRequest()
{
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(false)));
    configure(LRect(surface()->parent()->pos()-surface()->pos(),positioner()->size()));
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
void LPopupRole::repositionRequest()
{
    printf("REPOSITION\n");
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(LSurface::NoRole)));
    xdg_popup_send_repositioned(resource(),p_repositionSerial);
    configure(LRect(surface()->parent()->pos()-surface()->pos(),positioner()->size()));
}
#endif

void LPopupRole::geometryChanged()
{

}

void LPopupRole::ping(UInt32 serial)
{
    xdg_wm_base_send_ping(surface()->client()->xdgWmBaseResource(),serial);
}

void LPopupRole::configure(const LRect &rect)
{
    xdg_popup_send_configure(resource(),rect.x(),rect.y(),rect.w(),rect.h());
    xdg_surface_send_configure(surface()->imp()->m_xdgSurfaceResource,LWayland::nextSerial());
}

void LPopupRole::sendPopupDoneEvent()
{
    xdg_popup_send_popup_done(resource());
}

const LRect &LPopupRole::windowGeometry() const
{
    return m_windowGeometry;
}

LPositioner *LPopupRole::positioner() const
{
    return m_positioner;
}
