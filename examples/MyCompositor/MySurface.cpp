#include "MySurface.h"
#include <MyCompositor.h>
#include <WOutput.h>
#include <WPositioner.h>
#include <WCursor.h>
#include <MySeat.h>

MySurface::MySurface(wl_resource *surfaceResource, WClient *client, GLuint textureUnit)
          :WSurface::WSurface(surfaceResource,client,textureUnit)
{
    comp = (MyCompositor*)compositor();
    pos.setX(100 + rand() % W_WIDTH / 4);
    pos.setY(20 + rand() % W_HEIGHT / 4);
}

MySurface::~MySurface()
{
    // printf("Surface destoyed from child class\n");
}

// Event when window is grabbed (tipically by the topbar)
void MySurface::moveStartRequest()
{
    MySeat *seat = (MySeat*)compositor()->seat();
    if(!seat->isLeftMouseButtonPressed)
        return;
    seat->movingSurfaceInitPos = getRectWithoutDecoration().topLeft();
    seat->movingSurfaceInitCursorPos = comp->cursor->position();
    seat->movingSurface = this;
}

void MySurface::maxSizeChangeRequest()
{
}

void MySurface::minSizeChangeRequest()
{
}

void MySurface::resizeStartRequest(ResizeEdge edge)
{
    MySeat *seat = (MySeat*)compositor()->seat();

    if(!seat->isLeftMouseButtonPressed)
        return;

    seat->resizingSurface = this;
    seat->resizeEdge = edge;
    seat->resizeInitMousePos = comp->cursor->position();
    seat->resizeInitSurfaceRect = getRectWithoutDecoration();
    seat->resizeInitSurfaceDecoration = WRect(pos,size()/bufferScale());
}

void MySurface::geometryChangeRequest()
{
    /* Geometry of the surface without client decorations
     * x and y represent the decoration margin */


}

void MySurface::typeChangeRequest()
{
    /* Internally updated, can be accessed with the type() menthod.
     * You should only draw surfaces with type different to Undefined */

    printf("Surface changed type to %i.\n",type());

   comp->repaintAllOutputs();
}

void MySurface::positionerChangeRequest()
{
    /* Internally updated, can be accessed with the positioner() menthod.
     * The positoner is only avaliable if the surface type is Popup otherwise nullptr is returned */
    MySurface *parentSurface = (MySurface*)parent();
    WPoint parentPos = parentSurface->pos + parentSurface->decorationGeometry().topLeft();
    pos = positioner()->calculatePopupPosition(WRect(),parentPos);
    WPoint relPos = parentPos - pos;
    pos+=decorationGeometry().topLeft();
    sendConfigurePopupEvent(relPos.x(),relPos.y(),positioner()->size().w(),positioner()->size().h());
    comp->repaintAllOutputs();
}

void MySurface::parentChangeRequest()
{
    /* Internally updated, can be accessed with the parent() menthod.
     * If the surface has no parent, nullptr is returned */

    comp->repaintAllOutputs();
}

void MySurface::bufferScaleChangeRequest()
{
    /* Internally updated, can be accessed with the getBufferScale() menthod.
     * If the surface has no parent, nullptr is returned */

    comp->repaintAllOutputs();
}

void MySurface::bufferSizeChangeRequest()
{
    MySeat *seat = (MySeat*)compositor()->seat();

    if(seat->resizingSurface == this && seat->isLeftMouseButtonPressed)
    {
        WRect ir = seat->resizeInitSurfaceDecoration;

        if(seat->resizeEdge == ResizeEdge::Top || seat->resizeEdge == ResizeEdge::TopLeft || seat->resizeEdge == ResizeEdge::TopRight)
            pos.setY(ir.y() + ir.h() - height()/bufferScale());

        if(seat->resizeEdge == ResizeEdge::Left || seat->resizeEdge == ResizeEdge::TopLeft || seat->resizeEdge == ResizeEdge::BottomLeft)
            pos.setX(ir.x() + ir.w() - width()/bufferScale());
    }
}

void MySurface::grabSeatRequest()
{
    MySeat *seat = (MySeat*)compositor()->seat();

    if(seat->keyboardFocusSurface && seat->keyboardFocusSurface != this)
        seat->keyboardFocusSurface->sendKeyboardLeaveEvent();

    seat->keyboardFocusSurface = this;
    this->sendKeyboardEnterEvent();

    /*
    if(comp->pointerFocusSurface && comp->pointerFocusSurface != this)
        comp->pointerFocusSurface->sendPointerLeaveEvent();

    comp->pointerFocusSurface = this;
    this->sendPointerEnterEvent(20,20);
    */

}

void MySurface::configureToplevelRequest()
{
    sendConfigureToplevelEvent(0,0,Wpp::SurfaceState::Activated);
    comp->repaintAllOutputs();
}

void MySurface::resizingChanged()
{

}

Int32 MySurface::mapXtoLocal(int xGlobal)
{
    return xGlobal - pos.x();
}

Int32 MySurface::mapYtoLocal(int yGlobal)
{
    return yGlobal - pos.y();
}

bool MySurface::containsPoint(Int32 x, Int32 y, bool withoutDecoration)
{
    if(withoutDecoration)
    {
        WRect r = getRectWithoutDecoration();

        if(r.x() > x)
            return false;
        if(r.x() + r.w()/bufferScale() < x)
            return false;
        if(r.y() > y)
            return false;
        if(r.y() + r.h()/bufferScale() < y)
            return false;

        return true;
    }
    else
    {
        if(pos.x() > x)
            return false;
        if(pos.x() + width()/bufferScale() < x)
            return false;
        if(pos.y() > y)
            return false;
        if(pos.y() + height()/bufferScale() < y)
            return false;

        return true;
    }
}

WRect MySurface::getRectWithoutDecoration()
{
    WRect rect;
    WRect decoration = decorationGeometry();

    if(type() != SurfaceType::Undefined)
        rect = WRect(pos + decoration.topLeft(),decoration.bottomRight());
    else
        rect = WRect(pos.x(),pos.y(),width(),height());

    return rect;
}


void MySurface::setXWithoutDecoration(Int32 x)
{
    pos.setX(x-decorationGeometry().x());
}

void MySurface::setYWithoutDecoration(Int32 y)
{
    pos.setY(y-decorationGeometry().y());
}

