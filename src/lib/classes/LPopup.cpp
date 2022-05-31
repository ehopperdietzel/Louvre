#include "LPopup.h"
#include <LRect.h>
#include <LSurface.h>
#include <LWayland.h>
#include <LPositioner.h>
#include <LCompositor.h>
#include <LOutput.h>

using namespace Louvre;


Louvre::LPopupRole::LPopupRole(wl_resource *popup, LSurface *surface, LPositioner *positioner)
{
    p_resource = popup;
    p_surface = surface;
    p_positioner = positioner;
}

LPopupRole::~LPopupRole()
{

}

void LPopupRole::configureRequest()
{
    LRect rect = positioner()->anchorRect();
    printf("POPUP CONFIGURE(%i,%i,%i,%i)\n",rect.x(),rect.y(),rect.w(),rect.h());
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(LSurface::NoRole)));
    configure(LRect(surface()->parent()->pos()-surface()->pos(),positioner()->size()));
}

void LPopupRole::repositionRequest()
{
    printf("REPOSITION\n");
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(LSurface::NoRole)));
    xdg_popup_send_repositioned(resource(),p_repositionSerial);
    configure(LRect(surface()->parent()->pos()-surface()->pos(),positioner()->size()));
}

void LPopupRole::geometryChanged()
{

}

void LPopupRole::configure(const LRect &rect)
{
    xdg_popup_send_configure(resource(),rect.x(),rect.y(),rect.w(),rect.h());
    xdg_surface_send_configure(surface()->p_xdg_shell,WWayland::nextSerial());
}

const LRect &LPopupRole::windowGeometry() const
{
    return p_windowGeometry;
}

LPositioner *LPopupRole::positioner() const
{
    return p_positioner;
}

LSurface *LPopupRole::surface() const
{
    return p_surface;
}

wl_resource *LPopupRole::resource() const
{
    return p_resource;
}
