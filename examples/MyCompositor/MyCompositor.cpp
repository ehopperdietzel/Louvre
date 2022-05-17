#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <MyClient.h>
#include <WBackend.h>
#include <MyOutputManager.h>
#include <WOutput.h>
#include <WPositioner.h>
#include <WCursor.h>
#include <WRegion.h>
#include "MySeat.h"

MyCompositor::MyCompositor():WCompositor()
{
    // Use the output manager to get connected displays
    MyOutputManager *outputManager = new MyOutputManager(this);

    // Use the first avaliable display
    WOutput *output = outputManager->getOutputsList()->front();

    // Set the output scale (2 for HiDPI displays)
    output->setOutputScale(2);

    // Add the output to the compositor (this inits the output thread, OpenGL context and render loop)
    addOutput(output);
}


void MyCompositor::initializeGL(WOutput *output)
{
    /*************************************************
     * Here you initialize your OpenGL ES 2 context
     *************************************************/

    output->setPainter(new WOpenGL());

    // Load default cursor texture (64x64)
    defaultCursorTexture = WOpenGL::loadTexture("/opt/MyCompositor/res/images/Cursor.png");

    // Load background texture
    backgroundTexture = WOpenGL::loadTexture("/opt/MyCompositor/res/images/Wallpaper.png");

    cursor = new WCursor(output);
    cursor->setSize(WSizeF(28,28));
    cursor->setTexture(defaultCursorTexture,WPointF(0,0));
}

void MyCompositor::paintGL(WOutput *output)
{
    /*************************************************
     *  Here you do your OpenGL drawing.
     *  Each output has its own OpenGL context.
     *  Never invoke this method directly,
     *  use WOutput::repaint() instead to schedule
     *  the next frame for a specific output,
     *  or WCompositor::repaintAllOutputs() to
     *  repaint all outputs added to the compositor.
     *************************************************/

    // Get the painter
    WOpenGL *GL = output->painter();

    // Background
    GL->drawTexture(backgroundTexture, WRect(WPoint(),backgroundTexture->size()), WRect(WPoint(),output->size/output->getOutputScale()));

    // Draw surfaces
    for(MySurface *surface : surfacesList)
    {
        if( surface->type() != SurfaceType::Undefined && surface->type() != SurfaceType::Cursor)
            GL->drawTexture(surface->texture(),WRect(WPoint(),surface->texture()->size()),WRect(surface->pos,surface->size()/surface->bufferScale()));

        surface->requestNextFrame();
    }

    // Draw the cursor if hardware composition is not supported
    if(!cursor->hasHardwareSupport())
        cursor->paint();
}

WSeat *MyCompositor::configureSeat()
{
    return new MySeat(this);
}


WClient *MyCompositor::newClientRequest(wl_client *client)
{
    /*******************************************************************************
     * Notify a new client connection, it's automatically added to an internal std list
     * you can access with 'compositor->clients'
     *******************************************************************************/

    return new MyClient(client,this);
}

void MyCompositor::clientDisconnectRequest(WClient *client)
{
    /*******************************************************************************
     * Notify a client disconnection, it's automatically removed from the internal
     * 'compositor->clients' list.
     * All destroy events from resources related to the client (like surfaces, regions, etc)
     * are prevously notified.
     *******************************************************************************/
    (void)client;
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
        if((*s)->type() != SurfaceType::Undefined || (*s)->type() != SurfaceType::Cursor)
            if((*s)->inputRegionContainsPoint((*s)->pos,point))
                return *s;

    return nullptr;
}




