#include "MySurface.h"
#include <MyCompositor.h>
#include <WOutput.h>
MySurface::MySurface(UInt32 id, wl_resource *resource, WClient *client, GLuint textureUnit):WSurface::WSurface(id,resource,client,textureUnit)
{
    comp = (MyCompositor*)getCompositor();
    setX(rand() % comp->pointerOutput->getCurrentMode().hdisplay / 4);
    setY(rand() % comp->pointerOutput->getCurrentMode().vdisplay / 4);
}

MySurface::~MySurface()
{

}

// Event when window is grabbed (tipically by the topbar)
void MySurface::moveStartRequest()
{
    if(!comp->isLeftMouseButtonPressed)
        return;
    comp->movingSurfaceInitialPosX = getRectWithoutDecoration().x;
    comp->movingSurfaceInitialPosY = getRectWithoutDecoration().y;
    comp->movingSurfaceInitialCursorPosX = comp->pointer.x;
    comp->movingSurfaceInitialCursorPosY = comp->pointer.y;
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

    comp->resizeInitialMousePos = { comp->pointer.x, comp->pointer.y };

    Rect dr = getRectWithoutDecoration();
    comp->resizeInitialSurfaceRect = {(double)dr.x,(double)dr.y,(double)dr.width,(double)dr.height};
    comp->resizeInitialSurfaceDecoration = {getX(),getY(),getWidth(),getHeight()};
}

void MySurface::geometryChangeRequest()
{
    /* Geometry of the surface without client decorations
     * x and y represent the decoration margin */
}

void MySurface::typeChangeRequest()
{
    /* Internally updated, can be accessed with the getType() menthod.
     * You should only draw surfaces with type different to Undefined */

    printf("Surface changed type to %i.\n",getType());

   comp->repaintAllOutputs();
}

void MySurface::positionerChangeRequest()
{
    /* Internally updated, can be accessed with the getPositioner() menthod.
     * The positoner is only avaliable for surfaces with type Popup otherwise nullptr is returned */

    comp->repaintAllOutputs();
}

void MySurface::parentChangeRequest()
{
    /* Internally updated, can be accessed with the getParent() menthod.
     * If the surface has no parent, nullptr is returned */

    comp->repaintAllOutputs();
}

void MySurface::bufferScaleChangeRequest()
{
    /* Internally updated, can be accessed with the getBufferScale() menthod.
     * If the surface has no parent, nullptr is returned */

    comp->repaintAllOutputs();
}

Int32 MySurface::mapXtoLocal(int xGlobal)
{
    return xGlobal - getX();
}

Int32 MySurface::mapYtoLocal(int yGlobal)
{
    return yGlobal - getY();
}

bool MySurface::containsPoint(Int32 x, Int32 y, bool withoutDecoration)
{
    if(withoutDecoration)
    {
        Rect r = getRectWithoutDecoration();

        if(r.x > x)
            return false;
        if(r.x + r.width < x)
            return false;
        if(r.y> y)
            return false;
        if(r.y + r.height < y)
            return false;

        return true;
    }
    else
    {
        if(_posX > x)
            return false;
        if(_posX + getWidth() < x)
            return false;
        if(_posY > y)
            return false;
        if(_posY + getHeight() < y)
            return false;

        return true;
    }
}

Rect MySurface::getRectWithoutDecoration()
{
    Rect rect;
    Rect decorationGeometry = getDecorationGeometry();
    if(getType() != SurfaceType::Undefined)
    {
        rect.x = getX() + decorationGeometry.x;
        rect.y = getY() + decorationGeometry.y;
        rect.width = decorationGeometry.width;
        rect.height = decorationGeometry.height;
    }
    else
    {
        rect.x = getX();
        rect.y = getY();
        rect.width = getWidth();
        rect.height = getHeight();
    }
    return rect;
}


void MySurface::setPos(int x, int y)
{
    _posX = x;
    _posY = y;
}

void MySurface::setX(int x)
{
    _posX = x;
}

void MySurface::setY(int y)
{
    _posY = y;
}

void MySurface::setXWithoutDecoration(Int32 x)
{
    setX(x-getDecorationGeometry().x);
}

void MySurface::setYWithoutDecoration(Int32 y)
{
    setY(y-getDecorationGeometry().y);
}

int MySurface::getX()
{
    return _posX;
}

int MySurface::getY()
{
    return _posY;
}
