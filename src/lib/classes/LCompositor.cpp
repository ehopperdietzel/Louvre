#include "LCompositor.h"
#include <LNamespaces.h>
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
#include <LSubsurfaceRole.h>
#include <LPointer.h>
#include <LKeyboard.h>

#include <dlfcn.h>

using namespace Louvre;

LCompositor::LCompositor(const char *backendPath)
{
    //signal(SIGINT,SIG_IGN);
    //signal(SIGABRT,SIG_IGN);
    loadBackend(backendPath);

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

LPointer *LCompositor::createPointerRequest(LSeat *seat)
{
    return new LPointer(seat);
}

LKeyboard *LCompositor::createKeyboardRequest(LSeat *seat)
{
    return new LKeyboard(seat);
}



LToplevelRole *LCompositor::createToplevelRequest(wl_resource *toplevel, LSurface *surface)
{
    return new LToplevelRole(toplevel, surface);
}

LPopupRole *LCompositor::createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner)
{
    return new LPopupRole(popup,surface,positioner);
}

LSubsurfaceRole *LCompositor::createSubsurfaceRequest(wl_resource *subsurface, LSurface *surface)
{
    return new LSubsurfaceRole(subsurface,surface);
}

void LCompositor::destroyOutputRequest(LOutput *output)
{

}

void LCompositor::destroyClientRequest(LClient *client)
{

}

void LCompositor::destroySurfaceRequest(LSurface *surface)
{
    if(seat()->pointer()->cursorSurface() == surface)
    {
        cursor()->setCursor(LCursor::Arrow);
        seat()->pointer()->setCursorSurface(nullptr);
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

void LCompositor::destroyCursorRequest(LCursorRole *)
{
    cursor()->setCursor(LCursor::Arrow);
}

void LCompositor::start()
{    
    if(_started)
        return;

    // Ask the developer to return a LSeat
    p_seat = createSeatRequest();
    LWayland::setSeat(p_seat);

    initialize();

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

bool LCompositor::loadBackend(const char *backendPath)
{
    void *backendHandle = dlopen(backendPath, RTLD_LAZY);

    if(!backendHandle)
    {
        printf("ERROR: No backend found at (%s)\n",backendPath);
        return false;
    }

    LGraphicBackend *(*getAPI)() = (LGraphicBackend *(*)())dlsym(backendHandle, "getAPI");

    if(!getAPI)
    {
        printf("ERROR: Failed to load backend (%s)\n",backendPath);
        return false;
    }

    p_backend = getAPI();

    return true;
}

bool LCompositor::insertSurfaceAfter(LSurface *prevSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = p_surfaces.begin(); it != p_surfaces.end(); it++)
    {
        if((*it) == prevSurface)
        {
            p_surfaces.remove(surfaceToInsert);

            if(it++ == p_surfaces.end())
                p_surfaces.push_back(surfaceToInsert);
            else
                p_surfaces.insert((it++),surfaceToInsert);

            return true;
        }
    }

    return false;
}

bool LCompositor::insertSurfaceBefore(LSurface *nextSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = p_surfaces.begin(); it != p_surfaces.end(); ++it)
    {
        if((*it) == nextSurface)
        {
            p_surfaces.remove(surfaceToInsert);
            p_surfaces.insert(it,surfaceToInsert);
            return true;
        }
    }

    return false;
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
            usleep(1);

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

const list<LClient *> &LCompositor::clients() const
{
    return p_clients;
}


