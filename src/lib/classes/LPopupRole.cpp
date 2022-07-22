#include <LPopupRolePrivate.h>
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
   m_imp = new LPopupRolePrivate();
   m_imp->positioner = positioner;
}

LPopupRole::~LPopupRole()
{
    delete m_imp;
}

const LPoint &LPopupRole::rolePos() const
{

    LPoint parentPos = surface()->parent()->pos();
    LPoint popupOrigin;
    LPoint anchorPos;
    LSize popupSize = positioner()->size();

    switch(positioner()->anchor())
    {
        case LPositioner::Anchor::NoAnchor:
        {
            anchorPos = positioner()->anchorRect().bottomRight()/2;
        }break;
        case LPositioner::Anchor::Top:
        {
            anchorPos.setX(positioner()->anchorRect().w()/2);
        }break;
        case LPositioner::Anchor::Bottom:
        {
            anchorPos.setX(positioner()->anchorRect().w()/2);
            anchorPos.setY(positioner()->anchorRect().h());
        }break;
        case LPositioner::Anchor::Left:
        {
            anchorPos.setY(positioner()->anchorRect().h()/2);
        }break;
        case LPositioner::Anchor::Right:
        {
            anchorPos.setX(positioner()->anchorRect().w());
            anchorPos.setY(positioner()->anchorRect().h()/2);
        }break;
        case LPositioner::Anchor::TopLeft:
        {
            // (0,0)
        }break;
        case LPositioner::Anchor::BottomLeft:
        {
            anchorPos.setY(positioner()->anchorRect().h());
        }break;
        case LPositioner::Anchor::TopRight:
        {
            anchorPos.setX(positioner()->anchorRect().w());
        }break;
        case LPositioner::Anchor::BottomRight:
        {
            anchorPos = positioner()->anchorRect().bottomRight();
        }break;
    }

    switch(positioner()->gravity())
    {
        case LPositioner::Gravity::NoGravity:
        {
            popupOrigin = popupSize/2;
        }break;
        case LPositioner::Gravity::Top:
        {
            popupOrigin.setX(popupSize.w()/2);
            popupOrigin.setY(popupSize.h());
        }break;
        case LPositioner::Gravity::Bottom:
        {
            popupOrigin.setX(popupSize.w()/2);
        }break;
        case LPositioner::Gravity::Left:
        {
            popupOrigin.setX(popupSize.w());
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case LPositioner::Gravity::Right:
        {
            popupOrigin.setY(popupSize.h()/2);
        }break;
        case LPositioner::Gravity::TopLeft:
        {
            popupOrigin = popupSize;
        }break;
        case LPositioner::Gravity::BottomLeft:
        {
            popupOrigin.setX(popupSize.w());
        }break;
        case LPositioner::Gravity::TopRight:
        {
            popupOrigin.setY(popupSize.h());
        }break;
        case LPositioner::Gravity::BottomRight:
        {
            // (0,0)
        }break;
    }

    //if(positioner()->constraintAdjustment() & LPositioner::LConstraintAdjustment::LSlideX)

    baseImp()->rolePos = parentPos + positioner()->anchorRect().topLeft() + anchorPos - popupOrigin + positioner()->offset() - m_imp->windowGeometry.topLeft();

    return baseImp()->rolePos;
}

void LPopupRole::pong(UInt32)
{

}

void LPopupRole::grabSeatRequest()
{
    //seat()->pointer()->setFocus(surface());
    seat()->keyboard()->setFocus(surface());
}

void LPopupRole::configureRequest()
{
    LOutput *output = surface()->compositor()->outputs().front();

    LPoint p = rolePos() - surface()->parent()->pos();
    //surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(false)));
    configure(LRect(p,positioner()->size()));
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
void LPopupRole::repositionRequest()
{
    /*
    printf("REPOSITION\n");
    LOutput *output = surface()->compositor()->outputs().front();
    surface()->setPos(positioner()->calculatePopupPosition(output->rect(),surface()->parent()->pos(LSurface::NoRole)));
    xdg_popup_send_repositioned(resource(),p_repositionSerial);
    configure(LRect(surface()->parent()->pos()-surface()->pos(),positioner()->size()));
    */
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
    if(m_imp->dismissed)
        return;

    list<LSurface*>::const_reverse_iterator s = surface()->children().rbegin();
    for (; s!= surface()->children().rend(); s++)
    {
        if((*s)->popup())
            (*s)->popup()->sendPopupDoneEvent();
    }

    xdg_popup_send_popup_done(resource());
    m_imp->dismissed = true;
}

const LRect &LPopupRole::windowGeometry() const
{
    return m_imp->windowGeometry;
}

LPositioner *LPopupRole::positioner() const
{
    return m_imp->positioner;
}

LPopupRole::LPopupRolePrivate *LPopupRole::imp() const
{
    return m_imp;
}
