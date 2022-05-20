/*
#include "MyToplevel.h"

#include <WCursor.h>

MyToplevel::MyToplevel(wl_resource *toplevel, WSurface *surface):WToplevelRole(toplevel,surface)
{
    mySeat = (MySeat*)seat();
    mySurface = (MySurface*)surface;
    myCompositor = (MyCompositor*)compositor();
}

void MyToplevel::startMoveRequest()
{

    if(!mySeat->isLeftMouseButtonPressed)
        return;

    mySeat->movingSurfaceInitPos = mySurface->getRectWithoutDecoration().topLeft();
    mySeat->movingSurfaceInitCursorPos = myCompositor->cursor->position();
    mySeat->movingSurface = mySurface;
}

void MyToplevel::maxSizeChanged()
{

}

void MyToplevel::minSizeChanged()
{

}

void MyToplevel::startResizeRequest(ResizeEdge edge)
{

    if(!mySeat->isLeftMouseButtonPressed)
        return;

    mySeat->resizingSurface = mySurface;
    mySeat->resizeEdge = edge;
    mySeat->resizeInitMousePos = myCompositor->cursor->position();
    mySeat->resizeInitSurfaceRect = mySurface->getRectWithoutDecoration();
    mySeat->resizeInitSurfaceDecoration = WRect(mySurface->pos,mySurface->size()/mySurface->bufferScale());
}

void MyToplevel::configureRequest()
{
    configure(0,0,Wpp::SurfaceState::Activated);
}
*/
