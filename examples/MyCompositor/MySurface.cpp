#include "MySurface.h"
#include <MyCompositor.h>
#include <WOutput.h>
#include <WPositioner.h>

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
    if(!comp->isLeftMouseButtonPressed)
        return;
    comp->movingSurfaceInitPos = getRectWithoutDecoration().topLeft();
    comp->movingSurfaceInitCursorPos = comp->pointer;
    comp->movingSurface = this;
}

void MySurface::maxSizeChangeRequest()
{
}

void MySurface::minSizeChangeRequest()
{
}

void MySurface::resizeStartRequest(ResizeEdge edge)
{
    if(!comp->isLeftMouseButtonPressed)
        return;

    comp->resizingSurface = this;
    comp->resizeEdge = edge;
    comp->resizeInitMousePos = WPoint(comp->pointer);
    comp->resizeInitSurfaceRect = getRectWithoutDecoration();
    comp->resizeInitSurfaceDecoration = WRect(pos,size()/bufferScale());
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
    if(comp->resizingSurface == this && comp->isLeftMouseButtonPressed)
    {
        WRect ir = comp->resizeInitSurfaceDecoration;

        if(comp->resizeEdge == ResizeEdge::Top || comp->resizeEdge == ResizeEdge::TopLeft || comp->resizeEdge == ResizeEdge::TopRight)
            pos.setY(ir.y() + ir.h() - height()/bufferScale());

        if(comp->resizeEdge == ResizeEdge::Left || comp->resizeEdge == ResizeEdge::TopLeft || comp->resizeEdge == ResizeEdge::BottomLeft)
            pos.setX(ir.x() + ir.w() - width()/bufferScale());
    }
}

void MySurface::grabSeatRequest()
{
    if(comp->_keyboardFocusSurface && comp->_keyboardFocusSurface != this)
        comp->_keyboardFocusSurface->sendKeyboardLeaveEvent();

    comp->_keyboardFocusSurface = this;
    this->sendKeyboardEnterEvent();

    /*
    if(comp->_pointerFocusSurface && comp->_pointerFocusSurface != this)
        comp->_pointerFocusSurface->sendPointerLeaveEvent();

    comp->_pointerFocusSurface = this;
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
        if(r.x() + r.w() < x)
            return false;
        if(r.y() > y)
            return false;
        if(r.y() + r.h() < y)
            return false;

        return true;
    }
    else
    {
        if(pos.x() > x)
            return false;
        if(pos.x() + width() < x)
            return false;
        if(pos.y() > y)
            return false;
        if(pos.y() + height() < y)
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

