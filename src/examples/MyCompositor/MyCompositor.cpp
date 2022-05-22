#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <WBackend.h>
#include <WOutputManager.h>
#include <WOutput.h>
#include <WPositioner.h>
#include <WCursor.h>
#include <WRegion.h>

#include "MyOutput.h"
#include "MySeat.h"
#include "MyToplevel.h"

MyCompositor::MyCompositor():WCompositor()
{
    // Use the output manager to get connected displays
    WOutputManager *outputManager = new WOutputManager(this);

    // Use the first avaliable display
    WOutput *output = outputManager->getOutputsList()->front();

    // Set the output scale (2 for HiDPI displays)
    output->setOutputScale(2);

    // Add the output to the compositor (this inits the output thread, OpenGL context and render loop)
    addOutput(output);
}

/*
WOutput *MyCompositor::createOutputRequest()
{
    return new MyOutput();
}

WSurface *MyCompositor::createSurfaceRequest(wl_resource *surface, WClient *client)
{
    // Create your own surface instance
    MySurface *newSurface = new MySurface(surface,client,1);

    // Store it for later use
    surfacesList.push_back(newSurface);

    return newSurface;
}

void MyCompositor::destroySurfaceRequest(WSurface *surface)
{
    // Get the seat
    MySeat *mySeat = (MySeat*)seat();

    if(mySeat->movingSurface == surface)
        mySeat->movingSurface = nullptr;

    if(mySeat->dragginSurface == surface)
        mySeat->dragginSurface = nullptr;

    if(surface->type() == WSurface::Cursor)
        cursor->setTexture(defaultCursorTexture,WPointF());

    surfacesList.remove((MySurface*)surface);

    repaintAllOutputs();
}


WToplevelRole *MyCompositor::createToplevelRequest(wl_resource *toplevel, WSurface *surface)
{
    return new MyToplevel(toplevel, surface);
}


WSeat *MyCompositor::createSeatRequest()
{
    return new MySeat(this);
}


// Rise surface and its children
void MyCompositor::riseSurface(MySurface *surface)
{
    surfacesList.remove(surface);
    surfacesList.push_back(surface);

    for(list<WSurface*>::iterator s = surface->children().begin(); s != surface->children().end(); s++)
        riseSurface((MySurface*)(*s));

}

MySurface *MyCompositor::surfaceAt(const WPoint &point)
{
    for(list<MySurface*>::reverse_iterator s = surfacesList.rbegin(); s != surfacesList.rend(); s++)
        if((*s)->type() != WSurface::Undefined && (*s)->type() != WSurface::Cursor)
            if((*s)->inputRegionContainsPoint((*s)->pos,point))
                return *s;

    return nullptr;
}
*/



