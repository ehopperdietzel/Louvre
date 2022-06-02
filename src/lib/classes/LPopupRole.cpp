#include "LPopupRole.h"
#include <LRect.h>
#include <LSurface.h>
#include <LWayland.h>
#include <LPositioner.h>
#include <LCompositor.h>
#include <LOutput.h>
#include <LSeat.h>

using namespace Louvre;


Louvre::LPopupRole::LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner)
{
    p_resource = popup;
    p_surface = surface;
    p_positioner = positioner;
    p_compositor = p_surface->compositor();
}

LPopupRole::~LPopupRole()
{

}

void LPopupRole::pong(UInt32)
{

}

void LPopupRole::grabSeatRequest()
{
    seat()->setPointerFocus(surface());
    seat()->setKeyboardFocus(surface());
}

void LPopupRole::configureRequest()
{
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(LSurface::NoRole)));
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
    xdg_surface_send_configure(surface()->p_xdgSurfaceResource,LWayland::nextSerial());
}

void LPopupRole::sendPopupDoneEvent()
{
    xdg_popup_send_popup_done(resource());
}

const LRect &LPopupRole::windowGeometry() const
{
    return p_windowGeometry;
}

LCompositor *LPopupRole::compositor() const
{
    return p_compositor;
}

LPositioner *LPopupRole::positioner() const
{
    return p_positioner;
}

LSurface *LPopupRole::surface() const
{
    return p_surface;
}

LSeat *LPopupRole::seat() const
{
    return compositor()->seat();
}

wl_resource *LPopupRole::resource() const
{
    return p_resource;
}
