#include "WCompositor.h"
#include <WNamespaces.h>
#include <WBackend.h>
#include <WWayland.h>
#include <WOutput.h>
#include <WSeat.h>

#include <stdio.h>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <WToplevelRole.h>
#include <WCursor.h>


using namespace Wpp;

WCompositor::WCompositor()
{
    //signal(SIGINT,SIG_IGN);
    //signal(SIGABRT,SIG_IGN);

    // Store the main thread id for later use (in WCursor)
    p_threadId = std::this_thread::get_id();

    libinputFd = eventfd(0,EFD_SEMAPHORE);
    WWayland::initWayland(this);
}

WOutput *WCompositor::createOutputRequest()
{
    return new WOutput();
}

WClient *WCompositor::createClientRequest(wl_client *client)
{
    return new WClient(client,this);
}

WSurface *WCompositor::createSurfaceRequest(wl_resource *surface, WClient *client)
{
    return new WSurface(surface,client);
}

WSeat *WCompositor::createSeatRequest()
{
    return new WSeat(this);
}

WToplevelRole *WCompositor::createToplevelRequest(wl_resource *toplevel, WSurface *surface)
{
    return new WToplevelRole(toplevel, surface);
}

void WCompositor::destroyOutputRequest(WOutput *output)
{

}

void WCompositor::destroyClientRequest(WClient *client)
{

}

void WCompositor::destroySurfaceRequest(WSurface *surface)
{
    if(seat()->cursorSurface() == surface)
    {
        cursor()->setCursor("arrow");
        seat()->setCursorSurface(nullptr);
    }
}

void WCompositor::destroyTopLevelRequest(WToplevelRole *topLevel)
{
    repaintAllOutputs();
}


void WCompositor::start()
{    
    if(_started)
        return;

    // Ask the developer to return a WSeat
    p_seat = createSeatRequest();
    WWayland::setSeat(p_seat);

    // Init wayland
    _started = true;
    WWayland::runLoop();
}

void WCompositor::riseSurface(WSurface *surface)
{
    p_surfaces.remove(surface);
    p_surfaces.push_back(surface);

    // Rise its children
    for(WSurface *children : surface->children())
        riseSurface(children);
}

WCursor *WCompositor::cursor() const
{
    return p_cursor;
}


WSeat *WCompositor::seat() const
{
    return p_seat;
}

void WCompositor::repaintAllOutputs()
{
    for(list<WOutput*>::iterator it = p_outputs.begin(); it != p_outputs.end(); ++it)
        (*it)->repaint();
}

void WCompositor::addOutput(WOutput *output)
{
    // Add the output to the compositor list
    p_outputs.push_back(output);

    // This method inits the Output rendering thread and its OpenGL context
    output->setCompositor(this);

    // If the main thread has no OpenGL context yet
    if(!WWayland::isGlContextInitialized())
    {
        // Wait for the added output to create his OpenGL context in his own thread
        while(output->initializeResult() == WOutput::Pending)
            usleep(100);

        /* The next method creates a shared OpenGL context in the main thread.
         * This context is used only to allow the library to copy the surfaces buffers
         * into OpenGL textures from the main thread and release clients buffers
         * immediatly to allow them to reuse it.
         * This fix the Qt clients decoration bug while resizing. */
        WWayland::initGLContext();

    }
}

void WCompositor::removeOutput(WOutput *output)
{

}

const list<WSurface *> &WCompositor::surfaces() const
{
    return p_surfaces;
}

const list<WOutput *> &WCompositor::outputs() const
{
    return p_outputs;
}


