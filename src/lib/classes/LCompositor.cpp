#include "LCompositor.h"
#include <LNamespaces.h>
#include <LBackend.h>
#include <LWayland.h>
#include <LOutput.h>
#include <LSeat.h>
#include <LPopupRole.h>

#include <stdio.h>
#include <sys/poll.h>
#include <thread>
#include <unistd.h>
#include <LToplevelRole.h>
#include <LCursor.h>


using namespace Louvre;

LCompositor::LCompositor()
{
    //signal(SIGINT,SIG_IGN);
    //signal(SIGABRT,SIG_IGN);

    // Store the main thread id for later use (in LCursor)
    p_threadId = std::this_thread::get_id();

    libinputFd = eventfd(0,EFD_SEMAPHORE);
    LWayland::initWayland(this);
}

LOutput *LCompositor::createOutputRequest()
{
    return new LOutput();
}

LClient *LCompositor::createClientRequest(wl_client *client)
{
    return new LClient(client,this);
}

LSurface *LCompositor::createSurfaceRequest(wl_resource *surface, LClient *client)
{
    return new LSurface(surface,client);
}

LSeat *LCompositor::createSeatRequest()
{
    return new LSeat(this);
}

LToplevelRole *LCompositor::createToplevelRequest(wl_resource *toplevel, LSurface *surface)
{
    return new LToplevelRole(toplevel, surface);
}

LPopupRole *LCompositor::createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner)
{
    return new LPopupRole(popup,surface,positioner);
}

void LCompositor::destroyOutputRequest(LOutput *output)
{

}

void LCompositor::destroyClientRequest(LClient *client)
{

}

void LCompositor::destroySurfaceRequest(LSurface *surface)
{
    if(seat()->cursorSurface() == surface)
    {
        cursor()->setCursor("arrow");
        seat()->setCursorSurface(nullptr);
    }
}

void LCompositor::destroyToplevelRequest(LToplevelRole *toplevel)
{
    (void)toplevel;
    repaintAllOutputs();
}

void LCompositor::destroyPopupRequest(LPopupRole *popup)
{
    (void)popup;
    repaintAllOutputs();
}

void LCompositor::start()
{    
    if(_started)
        return;

    // Ask the developer to return a LSeat
    p_seat = createSeatRequest();
    LWayland::setSeat(p_seat);

    // Init wayland
    _started = true;
    LWayland::runLoop();
}

void LCompositor::riseSurface(LSurface *surface)
{
    p_surfaces.remove(surface);
    p_surfaces.push_back(surface);

    // Rise its children
    for(LSurface *children : surface->children())
        riseSurface(children);
}

LCursor *LCompositor::cursor() const
{
    return p_cursor;
}


LSeat *LCompositor::seat() const
{
    return p_seat;
}

void LCompositor::repaintAllOutputs()
{
    for(list<LOutput*>::iterator it = p_outputs.begin(); it != p_outputs.end(); ++it)
        (*it)->repaint();
}

void LCompositor::addOutput(LOutput *output)
{
    // Add the output to the compositor list
    p_outputs.push_back(output);

    // This method inits the Output rendering thread and its OpenGL context
    output->setCompositor(this);

    // If the main thread has no OpenGL context yet
    if(!LWayland::isGlContextInitialized())
    {
        // Wait for the added output to create his OpenGL context in his own thread
        while(output->initializeResult() == LOutput::Pending)
            usleep(100);

        /* The next method creates a shared OpenGL context in the main thread.
         * This context is used only to allow the library to copy the surfaces buffers
         * into OpenGL textures from the main thread and release clients buffers
         * immediatly to allow them to reuse it.
         * This fix the Qt clients decoration bug while resizing. */
        LWayland::initGLContext();

    }
}

void LCompositor::removeOutput(LOutput *output)
{

}

const list<LSurface *> &LCompositor::surfaces() const
{
    return p_surfaces;
}

const list<LOutput *> &LCompositor::outputs() const
{
    return p_outputs;
}


