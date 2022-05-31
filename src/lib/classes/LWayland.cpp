#include "LWayland.h"
#include <LCompositor.h>
#include <LBackend.h>
#include <LTexture.h>
#include <pthread.h>
#include <xdg-shell.h>
#include <LOpenGL.h>
#include <Region.h>

#include <Compositor.h>
#include <Subcompositor.h>
#include <Surface.h>
#include <Seat.h>
#include <DataDeviceManager.h>
#include <Output.h>
#include <XdgWmBase.h>

#include <sys/poll.h>
#include <unistd.h>

#include <LSurface.h>
#include <LOutput.h>
#include <LSeat.h>
#include <LToplevel.h>

using namespace std;
using namespace Louvre;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

LCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;
wl_event_source *renderTimer;
wl_listener clientConnectedListener,clientDisconnectedListener;
wl_event_source *clientDisconnectedEventSource;
wl_signal sign;
wl_listener signListen;
EGLContext eglCtx;
EGLDisplay eglDpy;
EGLContext sharedContext;
EGLDisplay sharedDisplay;
Int32 contextInitialized = 0;
LOutput *p_mainOutput;

bool updated = false;


void WWayland::initGLContext()
{

    if(contextInitialized == 1)
        contextInitialized = 2;
    else
    {
        return;
    }

    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };


    eglDpy = sharedDisplay;

    EGLConfig config;
    EGLint num_configs_returned;

    eglChooseConfig(eglDpy, attribs, &config, 1, &num_configs_returned);

    // EGL context and surface
    if(!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Failed to bind api EGL_OPENGL_ES_API\n");
        exit(-1);
    }


    // 3. Create a context and make it current
    eglCtx = eglCreateContext(eglDpy, config, sharedContext,context_attribs);

    if(eglCtx == NULL)
    {
        printf("CONTEXT ERROR %d\n",eglGetError());
        exit(0);
    }


    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, eglCtx);

    compositor->p_painter = new LOpenGL();
    return;
}

bool WWayland::isGlContextInitialized()
{
    return contextInitialized == 2;
}

void WWayland::setContext(LOutput *output, EGLDisplay sharedDisp, EGLContext sharedCont)
{
    p_mainOutput = output;
    sharedDisplay = sharedDisp;
    sharedContext = sharedCont;
    contextInitialized = 1;
}

LOutput *WWayland::mainOutput()
{
    return p_mainOutput;
}

void WWayland::forceUpdate()
{
    if(!updated)
        eventfd_write(compositor->libinputFd,1);
}

void WWayland::setSeat(LSeat *seat)
{
    // Create seat global
    wl_global_create(display, &wl_seat_interface, LOUVRE_SEAT_VERSION, seat->p_compositor, &Globals::Seat::bind);
    wl_event_loop_add_fd(event_loop,seat->libinputFd(),WL_EVENT_READABLE,&WWayland::processSeat,seat);
}

int WWayland::processSeat(int, unsigned int, void *userData)
{
    LSeat *seat = (LSeat*)userData;
    seat->processInput();
    return 0;
}

UInt32 WWayland::nextSerial()
{
    return wl_display_next_serial(display);
}

int WWayland::initWayland(LCompositor *comp)
{
    eglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWL) eglGetProcAddress ("eglBindWaylandDisplayWL");

    // Stores compositor reference
    compositor = comp;

    // Create a new Wayland display
    display = wl_display_create();

    if (!display)
    {
        printf("Unable to create Wayland display.\n");
        exit(EXIT_FAILURE);
    }

    // Use wayland-0 socket by default
    const char *socket = wl_display_add_socket_auto(display);

    if (!socket)
    {
        printf("Unable to add socket to Wayland display.\n");
        exit(EXIT_FAILURE);
    }


    // GLOBALS

    // Create compositor global
    wl_global_create(display, &wl_compositor_interface, 4, comp, &Globals::Compositor::bind); // Last 5

    // Create subcompositor global
    //wl_global_create(display, &wl_subcompositor_interface, 1, comp, &Globals::Subcompositor::bind); // 1

    // Create output global
    wl_global_create(display, &wl_output_interface, 3, comp, &Globals::Output::bind);//3

    // Create data device manager global
    wl_global_create(display, &wl_data_device_manager_interface, 3, comp, &Globals::DataDeviceManager::bind);//3

    // Create xdg shell global
    wl_global_create(display, &xdg_wm_base_interface, LOUVRE_XDG_SHELL_VERSION, comp, &Extensions::XdgShell::WmBase::bind); //4

    wl_display_init_shm(display);
    //wl_data_device_manager_init(display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

    wl_event_loop_add_fd(event_loop,comp->libinputFd,WL_EVENT_READABLE,&WWayland::readFd,comp);

    printf("Wayland server initialized.\n");

    comp->waylandFd = wl_event_loop_get_fd(event_loop);


    // Listen for client connections
    clientConnectedListener.notify = &clientConnectionEvent;
    wl_display_add_client_created_listener(display,&clientConnectedListener);
    return wayland_fd;

}

void WWayland::terminateDisplay()
{
   // Ends
    wl_display_terminate(display);
}

void WWayland::dispatchEvents()
{
    wl_event_loop_dispatch(event_loop,0);
}

void WWayland::flushClients()
{
    wl_display_flush_clients(display);
}

int WWayland::readFd(int, unsigned int, void *d)
{
    LCompositor *comp = (LCompositor*)d;
    updated = false;
    eventfd_read(comp->libinputFd,&comp->libinputVal);
    return 0;
}

void WWayland::bindEGLDisplay(EGLDisplay eglDisplay)
{
    eglBindWaylandDisplayWL(eglDisplay, display);
}

pollfd fds[3];

void WWayland::runLoop()
{
    fds[0].events = WL_EVENT_READABLE | WL_EVENT_WRITABLE;
    fds[0].fd = wayland_fd;

    /*
    fds[1].events = POLLIN;
    fds[1].fd = compositor->libinputFd;


    fds[2].events = POLLIN;
    fds[2].fd = WInput::getLibinputFd();
    */

    while(true)
    {
        poll(fds,1,-1);

        compositor->renderMutex.lock();

        dispatchEvents();

        for(LClient *client : compositor->clients)
        {
            for(LSurface *surface : client->surfaces)
            {
                if(surface->toplevel())
                    surface->toplevel()->dispachLastConfiguration();
            }
        }

        flushClients();



        compositor->renderMutex.unlock();



    }
    //wl_display_run(display);
}

void WWayland::clientConnectionEvent(wl_listener *listener, void *data)
{
    (void)listener;

    wl_client *client = (wl_client*)data;

    // Let the developer create his own client implementation
    LClient *newClient = compositor->createClientRequest(client);

    // Listen for client disconnection
    wl_client_get_destroy_listener(client,&WWayland::clientDisconnectionEvent);

    // Append client to the compositor list
    compositor->clients.push_back(newClient);
}

void WWayland::clientDisconnectionEvent(wl_listener *listener, void *data)
{

    (void)listener;

    wl_client *client = (wl_client*)data;

    LClient *disconnectedClient = nullptr;

    // Remove client from the compositor list
    for(LClient *wClient: compositor->clients)
    {
        if(wClient->client() == client)
        {
            disconnectedClient = wClient;
            break;
        }
    }

    if(disconnectedClient == nullptr)
        return;

    /*
    // Destroy regions
    for(LRegion *wRegion : disconnectedClient->regions)
        wl_resource_destroy(wRegion->resource());
    disconnectedClient->regions.clear();
    */

    // Destroy surfaces events
    for(LSurface *wSurface : disconnectedClient->surfaces)
    {
        disconnectedClient->compositor()->destroySurfaceRequest(wSurface);
        wSurface->p_client = nullptr;
    }
    disconnectedClient->surfaces.clear();

    // Remove the client from the list
    compositor->clients.remove(disconnectedClient);

    // Notify the client desconection
    compositor->destroyClientRequest(disconnectedClient);

    delete disconnectedClient;
}

int WWayland::apply_damage_emit(void *data)
{
    wl_signal_emit(&sign,data);
    return 0;
}



wl_event_source *WWayland::addTimer(wl_event_loop_timer_func_t func, void *data)
{
    return wl_event_loop_add_timer(event_loop,func,data);
}

EGLContext WWayland::eglContext()
{
    return sharedContext;
}

EGLDisplay WWayland::eglDisplay()
{
    return sharedDisplay;
}
