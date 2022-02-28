#include "MySurface.h"
#include <MyCompositor.h>

MySurface::MySurface(UInt32 id, wl_resource *resource, WClient *client):WSurface::WSurface(id,resource,client){}

MySurface::~MySurface()
{

}

// Event when window is grabbed (tipically by the topbar)
void MySurface::moveStartRequest()
{
    MyCompositor *comp = (MyCompositor*)getCompositor();
    if(!comp->isLeftMouseButtonPressed)
        return;
    comp->movingSurfaceInitialPosX = getRectWithoutDecoration().x;
    comp->movingSurfaceInitialPosY = getRectWithoutDecoration().y;
    comp->movingSurfaceInitialCursorPosX = comp->getPointerX();
    comp->movingSurfaceInitialCursorPosY = comp->getPointerY();
    comp->movingSurface = this;
}

void MySurface::maxSizeChangeRequest(Int32 width, Int32 height)
{
    (void)width;(void)height;
}

void MySurface::minSizeChangeRequest(Int32 width, Int32 height)
{
    (void)width;(void)height;
}

void MySurface::resizeStartRequest(ResizeEdge edge)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();

    if(!comp->isLeftMouseButtonPressed)
        return;

    comp->resizingSurface = this;
    comp->resizeEdge = edge;

    comp->resizeInitialMousePos.x = comp->getPointerX();
    comp->resizeInitialMousePos.y = comp->getPointerY();

    Rect dRect = getRectWithoutDecoration();
    comp->resizeInitialSurfaceRect.x = dRect.x;
    comp->resizeInitialSurfaceRect.y = dRect.y;
    comp->resizeInitialSurfaceRect.width = dRect.width;
    comp->resizeInitialSurfaceRect.height = dRect.height;
}

void MySurface::geometryChangeRequest(Int32 x, Int32 y, Int32 width, Int32 height)
{
    /* Geometry of the surface without client decorations
     * x and y represent the decoration margin */

    (void)x;(void)y;

    MyCompositor *comp = (MyCompositor*)getCompositor();

    if(comp->resizingSurface == this)
    {
        if(comp->resizeEdge == ResizeEdge::Top || comp->resizeEdge == ResizeEdge::TopLeft ||comp->resizeEdge == ResizeEdge::TopRight)
            comp->resizingSurface->setYWithoutDecoration(int(comp->resizeInitialSurfaceRect.y) + int(comp->resizeInitialSurfaceRect.height - height));

        if(comp->resizeEdge == ResizeEdge::Left || comp->resizeEdge == ResizeEdge::TopLeft || comp->resizeEdge == ResizeEdge::BottomLeft)
            comp->resizingSurface->setXWithoutDecoration(int(comp->resizeInitialSurfaceRect.x) + int(comp->resizeInitialSurfaceRect.width - width));

        comp->repaint();
    }
}

void MySurface::typeChangeRequest()
{
    /* Internally updated, can be accessed with the getType() menthod.
     * You should only draw surfaces with type different to Undefined */

    printf("Surface changed type to %i.\n",getType());

    getCompositor()->repaint();
}

void MySurface::positionerChangeRequest()
{
    /* Internally updated, can be accessed with the getPositioner() menthod.
     * The positoner is only avaliable for surfaces with type Popup otherwise nullptr is returned */

    getCompositor()->repaint();
}

void MySurface::parentChangeRequest()
{
    /* Internally updated, can be accessed with the getParent() menthod.
     * If the surface has no parent, nullptr is returned */

    getCompositor()->repaint();
}
