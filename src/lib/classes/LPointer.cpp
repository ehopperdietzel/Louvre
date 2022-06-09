#include "LPointer.h"
#include <LSeat.h>
#include <LCompositor.h>
#include <LWayland.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LTime.h>

using namespace Louvre;

LPointer::LPointer(LSeat *seat)
{
    p_seat = seat;
}

LSeat *LPointer::seat() const
{
    return p_seat;
}

LCompositor *LPointer::compositor() const
{
    return seat()->compositor();
}

LCursor *LPointer::cursor() const
{
    return compositor()->cursor();
}

void LPointer::setFocus(LSurface *surface)
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
            sendEnterEvent(surface,cursor()->position()-surface->pos());
            p_pointerFocusSurface = surface;
        }
        else
            p_pointerFocusSurface = nullptr;
    }

    // If surface is nullptr
    else
    {
        // Remove focus from focused surface
        sendLeaveEvent(focusSurface());
        p_pointerFocusSurface = nullptr;
    }
}

void LPointer::sendMoveEvent()
{
    // If no surface has focus surface
    if(!focusSurface())
        return;

    // If do not have a wl_pointer
    if(!focusSurface()->client()->pointerResource())
        return;

    // Calculate local cursor position
    LPoint pos = cursor()->position() - focusSurface()->pos(true);

    // Send pointer move event to the focused surface
    wl_pointer_send_motion(focusSurface()->client()->pointerResource(),LTime::ms(),wl_fixed_from_int(pos.x()),wl_fixed_from_int(pos.y()));

    // Version 5+
    if(wl_resource_get_version(focusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(focusSurface()->client()->pointerResource());
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
    wl_pointer_send_button(focusSurface()->client()->pointerResource(),LWayland::nextSerial(),LTime::ms(),button,state);

    // Version 5+
    if(wl_resource_get_version(focusSurface()->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(focusSurface()->client()->pointerResource());
}

void LPointer::startResizingToplevel(LToplevelRole *toplevel, LToplevelRole::Edge edge)
{
    p_resizingToplevel = toplevel;
    p_resizingToplevelEdge = edge;
    p_resizingToplevelInitSize = toplevel->surface()->size();
    p_resizingToplevelInitWindowSize = toplevel->windowGeometry().bottomRight();
    p_resizingToplevelInitCursorPos = cursor()->position();
    p_resizingToplevelInitPos = toplevel->surface()->pos();
}

bool LPointer::updateResizingToplevelSize()
{
    if(resizingToplevel())
    {
        LSize newSize = resizingToplevel()->calculateResizeRect(resizingToplevelInitCursorPos()-cursor()->position(),
                                                                p_resizingToplevelInitWindowSize,
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

    p_resizingToplevel = nullptr;
}

void LPointer::startMovingTopLevel(LToplevelRole *topLevel)
{
    p_movingTopLevelInitPos = topLevel->surface()->pos();
    p_movingTopLevelInitCursorPos = cursor()->position();
    p_movingTopLevel = topLevel;
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
    p_movingTopLevel = nullptr;

}

void LPointer::setDragginSurface(LSurface *surface)
{
    p_draggingSurface = surface;
}

void LPointer::setCursorSurface(LSurface *surface)
{
    p_cursorSurface = surface;
}

void LPointer::dismissPopups()
{
    for(LSurface *surface : compositor()->surfaces())
    {
        if(surface->roleType() == LSurface::Popup)
            surface->popup()->sendPopupDoneEvent();
    }
}

LSurface *LPointer::draggingSurface() const
{
    return p_draggingSurface;
}

LSurface *LPointer::cursorSurface() const
{
    return p_cursorSurface;
}

LToplevelRole *LPointer::resizingToplevel() const
{
    return p_resizingToplevel;
}

LToplevelRole *LPointer::movingTopLevel() const
{
    return p_movingTopLevel;
}

const LPoint &LPointer::movingTopLevelInitPos() const
{
    return p_movingTopLevelInitPos;
}

const LPoint &LPointer::movingTopLevelInitCursorPos() const
{
    return p_movingTopLevelInitCursorPos;
}

const LPoint &LPointer::resizingToplevelInitPos() const
{
    return p_resizingToplevelInitPos;
}

const LPoint &LPointer::resizingToplevelInitCursorPos() const
{
    return p_resizingToplevelInitCursorPos;
}

const LSize &LPointer::resizingToplevelInitSize() const
{
    return p_resizingToplevelInitSize;
}

LToplevelRole::Edge LPointer::resizingToplevelEdge() const
{
    return p_resizingToplevelEdge;
}

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
    return p_pointerFocusSurface;
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
    wl_pointer_send_leave(surface->client()->pointerResource(),LWayland::nextSerial(),surface->resource());

    // Version 5+
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());

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
    surface->client()->p_lastPointerEnterEventSerial = LWayland::nextSerial();
    wl_pointer_send_enter(surface->client()->pointerResource(),
                          surface->client()->p_lastPointerEnterEventSerial,
                          surface->resource(),
                          wl_fixed_from_double(point.x()),
                          wl_fixed_from_double(point.y()));

    // Version 5+
    if(wl_resource_get_version(surface->client()->pointerResource()) >= 5)
        wl_pointer_send_frame(surface->client()->pointerResource());
}

/* VIRTUAL */

void LPointer::pointerMoved(float dx, float dy)
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
        sendMoveEvent();
        return;
    }


    // Find the surface at cursor positon
    LSurface *surface = surfaceAt(cursor()->position(),true);

    // If no surface was found
    if(!surface)
    {
        setFocus(nullptr);
        cursor()->setCursor(LCursor::Arrow);
    }
    else
    {
        if(focusSurface() == surface)
            sendMoveEvent();
        else
        {
            setFocus(surface);
        }
    }

}

void LPointer::pointerButtonChanged(UInt32 button, UInt32 state)
{
    if(!focusSurface())
    {
        LSurface *surface = surfaceAt(cursor()->position());

        if(surface)
        {
            seat()->setKeyboardFocus(surface);
            setFocus(surface);
            sendButtonEvent(button,state);

            if(surface->roleType() != LSurface::Popup)
                dismissPopups();
        }
        else
        {
            seat()->setKeyboardFocus(nullptr);
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
        seat()->setKeyboardFocus(focusSurface());

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
        }

    }
}

void LPointer::setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    if(cursorSurface)
        cursor()->setTexture(cursorSurface->texture(),LPointF(hotspotX,hotspotY));
    else
        cursor()->setCursor(LCursor::Arrow);

    setCursorSurface(cursorSurface);
}



