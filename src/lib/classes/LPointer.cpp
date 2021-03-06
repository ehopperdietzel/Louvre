#include "LPointer.h"
#include <LSeat.h>
#include <LCompositor.h>
#include <LWayland.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LTime.h>
#include <LKeyboard.h>

#include <LClientPrivate.h>

using namespace Louvre;

LPointer::LPointer(LSeat *seat)
{
    m_seat = seat;
}

LSeat *LPointer::seat() const
{
    return m_seat;
}

LCompositor *LPointer::compositor() const
{
    return seat()->compositor();
}

LCursor *LPointer::cursor() const
{
    return compositor()->cursor();
}

void LPointer::setFocus(LSurface *surface, bool useRolePos)
{
    if(surface)
        setFocus(surface,cursor()->position()-surface->pos(useRolePos));
    else
        setFocus(nullptr,LPoint());
}

void LPointer::setFocus(LSurface *surface, const LPoint &pos)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(focusSurface() == surface)
            return;

        // Remove focus from focused surface
        sendLeaveEvent(focusSurface());

        // Set focus
        if(surface->client()->pointerResource())
        {
            // Send focus event
            sendEnterEvent(surface, pos);
            m_pointerFocusSurface = surface;
        }
        else
            m_pointerFocusSurface = nullptr;
    }

    // If surface is nullptr
    else
    {
        // Remove focus from focused surface
        sendLeaveEvent(focusSurface());
        m_pointerFocusSurface = nullptr;
    }
}

void LPointer::sendMoveEvent(bool useRolePos)
{
    if(focusSurface())
        sendMoveEvent(cursor()->position() - focusSurface()->pos(useRolePos));
}

void LPointer::sendMoveEvent(const LPoint &pos)
{
    // If no surface has focus surface
    if(!focusSurface())
        return;

    // If do not have a wl_pointer
    if(!focusSurface()->client()->pointerResource())
        return;

    // Send pointer move event to the focused surface
    wl_pointer_send_motion(focusSurface()->client()->pointerResource(),LTime::ms(),wl_fixed_from_int(pos.x()),wl_fixed_from_int(pos.y()));

#if LOUVRE_SEAT_VERSION >=5
    if(wl_resource_get_version(focusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(focusSurface()->client()->pointerResource());
#endif
}

void LPointer::sendButtonEvent(UInt32 button, UInt32 state)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_pointer
    if(!focusSurface()->client()->pointerResource())
        return;

    // Send pointer button event
    focusSurface()->client()->imp()->m_lastPointerButtonEventSerial = LWayland::nextSerial();
    wl_pointer_send_button(focusSurface()->client()->pointerResource(),focusSurface()->client()->lastPointerButtonEventSerial(),LTime::ms(),button,state);

#if LOUVRE_SEAT_VERSION >=5
    if(wl_resource_get_version(focusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(focusSurface()->client()->pointerResource());
#endif
}


void LPointer::startResizingToplevel(LToplevelRole *toplevel, LToplevelRole::Edge edge)
{
    m_resizingToplevel = toplevel;
    m_resizingToplevelEdge = edge;
    m_resizingToplevelInitSize = toplevel->surface()->size();
    m_resizingToplevelInitWindowSize = toplevel->windowGeometry().bottomRight();
    m_resizingToplevelInitCursorPos = cursor()->position();
    m_resizingToplevelInitPos = toplevel->surface()->pos();
}

bool LPointer::updateResizingToplevelSize()
{
    if(resizingToplevel())
    {
        LSize newSize = resizingToplevel()->calculateResizeRect(resizingToplevelInitCursorPos()-cursor()->position(),
                                                                m_resizingToplevelInitWindowSize,
                                                                resizingToplevelEdge());

        resizingToplevel()->configure(newSize ,LToplevelRole::Activated | LToplevelRole::Resizing);
        return true;
    }
    return false;
}

void LPointer::updateResizingToplevelPos()
{
    if(resizingToplevel())
    {
        LSize s = resizingToplevelInitSize();
        LPoint p = resizingToplevelInitPos();
        LToplevelRole::Edge edge =  resizingToplevelEdge();

        if(edge ==  LToplevelRole::Edge::Top || edge ==  LToplevelRole::TopLeft || edge ==  LToplevelRole::TopRight)
            resizingToplevel()->surface()->setY(p.y() + (s.h() - resizingToplevel()->surface()->size().h()));

        if(edge ==  LToplevelRole::Edge::Left || edge ==  LToplevelRole::Edge::TopLeft || edge ==  LToplevelRole::Edge::BottomLeft)
            resizingToplevel()->surface()->setX(p.x() + (s.w() - resizingToplevel()->surface()->size().w()));
    }
}

void LPointer::stopResizingToplevel()
{
    if(resizingToplevel())
        resizingToplevel()->configure(LToplevelRole::Activated);

    m_resizingToplevel = nullptr;
}

void LPointer::startMovingTopLevel(LToplevelRole *topLevel)
{
    m_movingTopLevelInitPos = topLevel->surface()->pos();
    m_movingTopLevelInitCursorPos = cursor()->position();
    m_movingTopLevel = topLevel;
}

bool LPointer::updateMovingTopLevelPos()
{
    if(movingTopLevel())
    {
        movingTopLevel()->surface()->setPos(movingTopLevelInitPos() - movingTopLevelInitCursorPos() + cursor()->position());
        return true;
    }
    return false;
}

void LPointer::stopMovingTopLevel()
{
    m_movingTopLevel = nullptr;

}

void LPointer::setDragginSurface(LSurface *surface)
{
    m_draggingSurface = surface;
}

void LPointer::setCursorSurface(LSurface *surface)
{
    m_cursorSurface = surface;
}

void LPointer::dismissPopups()
{    
    list<LSurface*>::const_reverse_iterator s = compositor()->surfaces().rbegin();
    for (; s!= compositor()->surfaces().rend(); s++)
    {
        if((*s)->popup())
            (*s)->popup()->sendPopupDoneEvent();
    }
}

LSurface *LPointer::draggingSurface() const
{
    return m_draggingSurface;
}

LSurface *LPointer::cursorSurface() const
{
    return m_cursorSurface;
}

LToplevelRole *LPointer::resizingToplevel() const
{
    return m_resizingToplevel;
}

LToplevelRole *LPointer::movingTopLevel() const
{
    return m_movingTopLevel;
}

const LPoint &LPointer::movingTopLevelInitPos() const
{
    return m_movingTopLevelInitPos;
}

const LPoint &LPointer::movingTopLevelInitCursorPos() const
{
    return m_movingTopLevelInitCursorPos;
}

const LPoint &LPointer::resizingToplevelInitPos() const
{
    return m_resizingToplevelInitPos;
}

const LPoint &LPointer::resizingToplevelInitCursorPos() const
{
    return m_resizingToplevelInitCursorPos;
}

const LSize &LPointer::resizingToplevelInitSize() const
{
    return m_resizingToplevelInitSize;
}

LToplevelRole::Edge LPointer::resizingToplevelEdge() const
{
    return m_resizingToplevelEdge;
}

UInt32 LPointer::lastPointerEnterEventSerial() const
{
    return m_lastPointerEnterEventSerial;
}

#if LOUVRE_SEAT_VERSION >= 5

void LPointer::sendAxisEvent(double x, double y, UInt32 source)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_pointer
    wl_resource *res = focusSurface()->client()->pointerResource();
    if(!res)
        return;

    UInt32 time = LTime::ms();

    if(wl_resource_get_version(res) >= 5)
    {
        if(source == WL_POINTER_AXIS_SOURCE_WHEEL || source == WL_POINTER_AXIS_SOURCE_WHEEL_TILT)
        {
            wl_pointer_send_axis_discrete(res,WL_POINTER_AXIS_HORIZONTAL_SCROLL,scrollWheelStep().x());
            wl_pointer_send_axis(res,time,WL_POINTER_AXIS_HORIZONTAL_SCROLL,wl_fixed_from_double(x));
            wl_pointer_send_axis_discrete(res,WL_POINTER_AXIS_VERTICAL_SCROLL,scrollWheelStep().y());
            wl_pointer_send_axis(res,time,WL_POINTER_AXIS_VERTICAL_SCROLL,wl_fixed_from_double(y));
        }
        else
        {
            if(x == 0.0)
                wl_pointer_send_axis_stop(res,time,WL_POINTER_AXIS_HORIZONTAL_SCROLL);
            else
                wl_pointer_send_axis(res,time,WL_POINTER_AXIS_HORIZONTAL_SCROLL,wl_fixed_from_double(x));

            if(y == 0.0)
                wl_pointer_send_axis_stop(res,time,WL_POINTER_AXIS_VERTICAL_SCROLL);
            else
                wl_pointer_send_axis(res,time,WL_POINTER_AXIS_VERTICAL_SCROLL,wl_fixed_from_double(y));
        }
        wl_pointer_send_axis_source(res,source);
        wl_pointer_send_frame(res);
    }
    else
    {
        wl_pointer_send_axis(res,time,WL_POINTER_AXIS_HORIZONTAL_SCROLL,wl_fixed_from_double(x));
        wl_pointer_send_axis(res,time,WL_POINTER_AXIS_VERTICAL_SCROLL,wl_fixed_from_double(y));
    }

}

const LPoint &LPointer::scrollWheelStep() const
{
    return m_axisDiscreteStep;
}

void LPointer::setScrollWheelStep(const LPoint &step)
{
    m_axisDiscreteStep = step;
}

#else
void LPointer::sendAxisEvent(double x, double y)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_pointer
    wl_resource *res = focusSurface()->client()->pointerResource();
    if(!res)
        return;

    UInt32 time = LTime::ms();
    wl_pointer_send_axis(res,time,WL_POINTER_AXIS_HORIZONTAL_SCROLL,wl_fixed_from_double(x));
    wl_pointer_send_axis(res,time,WL_POINTER_AXIS_VERTICAL_SCROLL,wl_fixed_from_double(y));
}
#endif

LSurface *LPointer::surfaceAt(const LPoint &point, bool useRolePos)
{
    for(list<LSurface*>::const_reverse_iterator s = compositor()->surfaces().rbegin(); s != compositor()->surfaces().rend(); s++)
        if((*s)->roleType() != LSurface::Undefined && (*s)->roleType() != LSurface::Cursor && !(*s)->minimized())
            if((*s)->inputRegionContainsPoint((*s)->pos(useRolePos),point))
                return *s;

    return nullptr;
}

LSurface *LPointer::focusSurface() const
{
    return m_pointerFocusSurface;
}

void LPointer::sendLeaveEvent(LSurface *surface)
{
    // If surface is nullptr
    if(!surface)
        return;

    // If do not have a wl_pointer
    if(!surface->client()->pointerResource())
        return;

    // Send the unset focus event
    surface->client()->imp()->m_lastPointerLeaveEventSerial = LWayland::nextSerial();
    wl_pointer_send_leave(surface->client()->pointerResource(),surface->client()->lastPointerLeaveEventSerial(),surface->resource());

#if LOUVRE_SEAT_VERSION >=5
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());
#endif

}

void LPointer::sendEnterEvent(LSurface *surface, const LPoint &point)
{
    // If surface is nullptr
    if(!surface)
        return;

    // If do not have a wl_pointer
    if(!surface->client()->pointerResource())
        return;

    // Send focus event
    m_lastPointerEnterEventSerial = LWayland::nextSerial();
    surface->client()->imp()->m_lastPointerEnterEventSerial = m_lastPointerEnterEventSerial;
    wl_pointer_send_enter(surface->client()->pointerResource(),
                          m_lastPointerEnterEventSerial,
                          surface->resource(),
                          wl_fixed_from_double(point.x()),
                          wl_fixed_from_double(point.y()));

#if LOUVRE_SEAT_VERSION >=5
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());
#endif
}


/* VIRTUAL */

void LPointer::pointerMoveEvent(float /*dx*/, float /*dy*/)
{
    // Resizing
    if(updateResizingToplevelSize())
        return;


    // Moving surface
    if(updateMovingTopLevelPos())
    {
        // Limits the top position
        Int32 topbarHeight = (LOUVRE_TB_H+2)/compositor()->outputs().front()->getOutputScale();

        if(movingTopLevel()->surface()->pos().y() < topbarHeight)
            movingTopLevel()->surface()->setY(topbarHeight);

        compositor()->repaintAllOutputs();
        return;
    }


    // Draggin surface
    if(draggingSurface())
    {
        sendMoveEvent(true);
        return;
    }


    // Find the surface at cursor positon
    LSurface *surface = surfaceAt(cursor()->position(),true);

    // If no surface was found
    if(!surface)
    {
        setFocus(nullptr);
        cursor()->setCursor(LCursor::Arrow);
        cursor()->setVisible(true);
    }
    else
    {
        if(focusSurface() == surface)
            sendMoveEvent(true);
        else
        {
            setFocus(surface);
            cursor()->setCursor(LCursor::Arrow);
            cursor()->setVisible(true);
        }
    }

}

void LPointer::pointerButtonEvent(UInt32 button, UInt32 state)
{
    if(!focusSurface())
    {
        LSurface *surface = surfaceAt(cursor()->position());

        if(surface)
        {
            seat()->keyboard()->setFocus(surface);
            setFocus(surface);
            sendButtonEvent(button,state);

            if(surface->roleType() != LSurface::Popup)
                dismissPopups();
        }
        else
        {
            seat()->keyboard()->setFocus(nullptr);
            dismissPopups();
        }

        return;
    }

    sendButtonEvent(button,state);

    if(button != LEFT_BUTTON)
        return;

    if(state == LIBINPUT_BUTTON_STATE_PRESSED)
    {
        if(focusSurface()->roleType() != LSurface::Popup)
            dismissPopups();

        setDragginSurface(focusSurface());
        seat()->keyboard()->setFocus(focusSurface());

        if(focusSurface()->roleType() == LSurface::Toplevel)
            focusSurface()->toplevel()->configure(focusSurface()->toplevel()->state() | LToplevelRole::Activated);

        // Raise view
        if(focusSurface()->parent())
            compositor()->riseSurface(focusSurface()->topParent());
        else
            compositor()->riseSurface(focusSurface());

    }
    // Release
    else
    {

        // Stop Resizing TopLevel
        stopResizingToplevel();

        // Stop Moving TopLevel
        stopMovingTopLevel();

        // Set dragging surface to nullptr
        setDragginSurface(nullptr);

        // Send a mouse leave event if the surface was being dragged
        if(!focusSurface()->inputRegionContainsPoint(focusSurface()->pos(),cursor()->position()))
        {
            setFocus(nullptr);
            cursor()->setCursor(LCursor::Arrow);
            cursor()->setVisible(true);
        }

    }
}


#if LOUVRE_SEAT_VERSION >= 5
void LPointer::pointerAxisEvent(double x, double y, UInt32 source)
{
    sendAxisEvent(-x,-y,source);
}
#else
void LPointer::pointerAxisEvent(double x, double y)
{
    sendAxisEvent(-x,-y);
}
#endif

void LPointer::setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    if(cursorSurface)
    {
        cursor()->setTexture(cursorSurface->texture(),LPointF(hotspotX,hotspotY));
        cursor()->setVisible(true);
    }
    else
        cursor()->setVisible(false);

    setCursorSurface(cursorSurface);
}



