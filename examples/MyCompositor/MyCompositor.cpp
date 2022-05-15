#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <MyClient.h>
#include <WBackend.h>
#include <MyOutputManager.h>
#include <WOutput.h>
#include <WPositioner.h>
#include <WCursor.h>
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
     *  the next frame for a specific output.
     *************************************************/

    // Get the painter
    WOpenGL *GL = output->painter();

    // Background
    GL->drawTexture(backgroundTexture, WRect(WPoint(0,0),backgroundTexture->size()), WRect(0,0,output->size.w()/output->getOutputScale(),output->size.h()/output->getOutputScale()));

    // Draw surfaces
    for(list<MySurface*>::iterator s = surfacesList.begin(); s != surfacesList.end(); s++)
        drawSurfaceTree(output,*s);

    if(!cursor->hasHardwareSupport())
        cursor->paint();

}

WSeat *MyCompositor::configureSeat()
{
    return new MySeat();
}

void MyCompositor::drawSurfaceTree(WOutput *output, MySurface *surface)
{

    if( surface->type() == SurfaceType::Undefined || surface->type() == SurfaceType::Cursor)
        return;

    // Get the painter
    WOpenGL *GL = output->painter();

    // If surface has no parent
    if(surface->type() == SurfaceType::Toplevel)
    {
        GL->drawTexture(surface->texture(),
                 WRect(0,0,surface->texture()->size().w(),surface->texture()->size().h()),
                 WRect(surface->pos,surface->size()/surface->bufferScale()));
    }
    else if(surface->type() == SurfaceType::Popup && surface->positioner() != nullptr)
    {
        GL->drawTexture(surface->texture(),
                 WRect(WPoint(),surface->texture()->size()),
                 WRect(surface->pos,surface->size()/surface->bufferScale()));
    }


    surface->requestNextFrame();

    /*
    // Draw it's children
    for(list<WSurface*>::iterator s = surface->_children.begin(); s != surface->_children.end(); s++)
        drawSurfaceTree((MySurface*)*s);
    */
}

WClient *MyCompositor::newClientRequest(wl_client *client)
{
    /*******************************************************************************
     * Notify a new client connection, it's automatically added to an internal std list
     * you can access with 'compositor->clients'
     *******************************************************************************/

    printf("New client connected.\n");
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
    printf("Client disconnected.\n");
}



// Rise surface and its children
void MyCompositor::riseSurface(MySurface *surface)
{
    surfacesList.remove(surface);
    surfacesList.push_back(surface);

    for(list<WSurface*>::iterator s = surface->children().begin(); s != surface->children().end(); s++)
        riseSurface((MySurface*)(*s));

}




