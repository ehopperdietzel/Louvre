#include <LCompositorPrivate.h>
#include <LOutputManager.h>
#include <LNamespaces.h>
#include <LWayland.h>
#include <LOutputPrivate.h>
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
#include <LSurfacePrivate.h>
#include <dlfcn.h>

using namespace Louvre;

LCompositor::LCompositor(const char *backendPath)
{
    m_imp = new LCompositorPrivate();

    loadBackend(backendPath);

    // Store the main thread id for later use (in LCursor)
    m_imp->m_threadId = std::this_thread::get_id();

    m_imp->libinputFd = eventfd(0,EFD_SEMAPHORE);

}

LCompositor::~LCompositor()
{
    delete m_imp;
}

void LCompositor::initialize()
{
    if(outputManager()->outputs()->empty())
    {
        printf("No outputs avaliable.\n");
        exit(1);
    }

    addOutput(outputManager()->outputs()->front());
}

LOutputManager *LCompositor::createOutputManagerRequest()
{
    return new LOutputManager(this);
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

void LCompositor::destroyOutputRequest(LOutput */*output*/)
{
    printf("Default Output Destroy Event.\n");
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
    if(m_imp->_started)
        return;

    LWayland::initWayland(this);

    m_imp->m_backend->initialize(this);

    m_imp->m_outputManager = createOutputManagerRequest();

    // Ask the developer to return a LSeat
    m_imp->m_seat = createSeatRequest();
    LWayland::setSeat(m_imp->m_seat);

    initialize();

    // Init wayland
    m_imp->_started = true;
    LWayland::runLoop();
}

void LCompositor::LCompositorPrivate::riseChildren(LSurface *surface)
{
    m_surfaces.remove(surface);
    m_surfaces.push_back(surface);

    // Rise its children
    for(LSurface *children : surface->children())
        riseChildren(children);
}

void LCompositor::riseSurface(LSurface *surface)
{

    if(surface->subsurface())
    {
        riseSurface(surface->parent());
        return;
    }

    m_imp->riseChildren(surface);
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

    m_imp->m_backend = getAPI();

    return true;
}

bool LCompositor::insertSurfaceAfter(LSurface *prevSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = m_imp->m_surfaces.begin(); it != m_imp->m_surfaces.end(); it++)
    {
        if((*it) == prevSurface)
        {
            m_imp->m_surfaces.remove(surfaceToInsert);

            if(it++ == m_imp->m_surfaces.end())
                m_imp->m_surfaces.push_back(surfaceToInsert);
            else
                m_imp->m_surfaces.insert((it++),surfaceToInsert);

            return true;
        }
    }

    return false;
}

bool LCompositor::insertSurfaceBefore(LSurface *nextSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = m_imp->m_surfaces.begin(); it != m_imp->m_surfaces.end(); ++it)
    {
        if((*it) == nextSurface)
        {
            m_imp->m_surfaces.remove(surfaceToInsert);
            m_imp->m_surfaces.insert(it,surfaceToInsert);
            return true;
        }
    }

    return false;
}

LCursor *LCompositor::cursor() const
{
    return m_imp->m_cursor;
}


LSeat *LCompositor::seat() const
{
    return m_imp->m_seat;
}

LDataSource *LCompositor::dataSelection() const
{
    return m_imp->m_dataSelection;
}

LDataSource *LCompositor::dataDrag() const
{
    return m_imp->m_dataDrag;
}


void LCompositor::repaintAllOutputs()
{
    for(list<LOutput*>::iterator it = m_imp->m_outputs.begin(); it != m_imp->m_outputs.end(); ++it)
        (*it)->repaint();
}

void LCompositor::addOutput(LOutput *output)
{
    // Add the output to the compositor list
    m_imp->m_outputs.push_back(output);

    // This method inits the Output rendering thread and its OpenGL context
    output->imp()->setCompositor(this);

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
    for(list<LOutput*>::iterator it = m_imp->m_outputs.begin(); it != m_imp->m_outputs.end(); it++)
    {
        LOutput *o = *it;
        if(o == output)
        {
            output->imp()->m_initializeResult = LOutput::Pending;
            output->repaint();

            while(output->initializeResult() != LOutput::Stopped)
                usleep(1);

            destroyOutputRequest(output);
            m_imp->m_outputs.erase(it);
            return;
        }
    }
}

LOutputManager *LCompositor::outputManager() const
{
    return m_imp->m_outputManager;
}

const list<LSurface *> &LCompositor::surfaces() const
{
    return m_imp->m_surfaces;
}

const list<LOutput *> &LCompositor::outputs() const
{
    return m_imp->m_outputs;
}

const list<LClient *> &LCompositor::clients() const
{
    return m_imp->m_clients;
}

thread::id LCompositor::mainThreadId() const
{
    return m_imp->m_threadId;
}

LCompositor::LCompositorPrivate *LCompositor::imp() const
{
    return m_imp;
}


