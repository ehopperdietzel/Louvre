/*
#include "MyOutput.h"

#include <WOpenGL.h>
#include <WCursor.h>

#include <MyCompositor.h>

MyOutput::MyOutput(){}

void MyOutput::initializeGL()
{

    MyCompositor *comp = (MyCompositor*)compositor();

    // Load default cursor texture (64x64)
    comp->defaultCursorTexture = WOpenGL::loadTexture("/opt/MyCompositor/res/images/Cursor.png");

    // Load background texture
    comp->backgroundTexture = WOpenGL::loadTexture("/opt/MyCompositor/res/images/Wallpaper.png");

    comp->cursor = new WCursor(this);
    comp->cursor->setSize(WSizeF(28,28));
    comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
}

void MyOutput::paintGL()
{

    // Get the painter
    WOpenGL *GL = painter();

    MyCompositor *comp = (MyCompositor*)compositor();

    // Background
    GL->drawTexture(comp->backgroundTexture, WRect(WPoint(),comp->backgroundTexture->size()), WRect(WPoint(),size/getOutputScale()));

    // Draw surfaces
    for(MySurface *surface : comp->surfacesList)
    {
        if( surface->type() == WSurface::Undefined || surface->type() == WSurface::Cursor)
            continue;

        GL->drawTexture(surface->texture(),WRect(WPoint(),surface->texture()->size()),WRect(surface->pos,surface->size()/surface->bufferScale()));
        surface->requestNextFrame();
    }

    // Draw the cursor if hardware composition is not supported
    if(!comp->cursor->hasHardwareSupport())
        comp->cursor->paint();
}

void MyOutput::plugged()
{

}

void MyOutput::unplugged()
{

}
*/
