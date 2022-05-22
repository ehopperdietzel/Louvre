/*
#include "MySurface.h"
#include <MyCompositor.h>
#include <WOutput.h>
#include <WPositioner.h>
#include <WCursor.h>
#include <MySeat.h>
#include <WToplevelRole.h>

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


void MySurface::typeChangeRequest()
{

    printf("Surface changed type to %i.\n",type());

    comp->repaintAllOutputs();
}

void MySurface::positionerChangeRequest()
{
    MySurface *parentSurface = (MySurface*)parent();
    WPoint parentPos = parentSurface->pos + parentSurface->toplevel()->windowGeometry().topLeft();
    pos = positioner()->calculatePopupPosition(WRect(),parentPos);
    WPoint relPos = parentPos - pos;
    pos+=toplevel()->windowGeometry().topLeft();
    sendConfigurePopupEvent(relPos.x(),relPos.y(),positioner()->size().w(),positioner()->size().h());
    comp->repaintAllOutputs();
}

void MySurface::parentChangeRequest()
{
    comp->repaintAllOutputs();
}

void MySurface::bufferScaleChangeRequest()
{
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
    //keyboard().setFocus();
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

    if(type() == SurfaceType::Toplevel || type() == SurfaceType::Popup)
        rect = WRect(pos + toplevel()->windowGeometry().topLeft(),toplevel()->windowGeometry().bottomRight());
    else
        rect = WRect(pos.x(),pos.y(),width(),height());

    return rect;
}


void MySurface::setXWithoutDecoration(Int32 x)
{
    pos.setX(x-toplevel()->windowGeometry().x());
}

void MySurface::setYWithoutDecoration(Int32 y)
{
    pos.setY(y-toplevel()->windowGeometry().y());
}
*/
