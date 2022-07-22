#include "LWayland.h"
#include <LCompositorPrivate.h>
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
#include <LToplevelRolePrivate.h>

using namespace std;
using namespace Louvre;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

LCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;
wl_event_source *renderTimer;
wl_listener clientConnectedListener;
wl_event_source *clientDisconnectedEventSource;
wl_signal sign;
wl_listener signListen;
EGLContext eglCtx;
EGLDisplay eglDpy;
EGLContext sharedContext;
EGLDisplay sharedDisplay;
Int32 contextInitialized = 0;
LOutput *m_mainOutput;

bool updated = false;


void LWayland::initGLContext()
{

    if(contextInitialized == 1)
        contextInitialized = 2;
    else
    {
        return;
    }

    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 0,
        EGL_GREEN_SIZE, 0,
        EGL_BLUE_SIZE, 0,
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

    compositor->imp()->m_painter = new LOpenGL();
    return;
}

bool LWayland::isGlContextInitialized()
{
    return contextInitialized == 2;
}

void LWayland::setContext(LOutput *output, EGLDisplay sharedDisp, EGLContext sharedCont)
{
    m_mainOutput = output;
    sharedDisplay = sharedDisp;
    sharedContext = sharedCont;
    contextInitialized = 1;
}

LOutput *LWayland::mainOutput()
{
    return m_mainOutput;
}

void LWayland::forceUpdate()
{
    //if(!updated)
        eventfd_write(compositor->imp()->libinputFd,1);
}

LCompositor *LWayland::bindedCompositor()
{
    return compositor;
}


void LWayland::addFdListener(int fd, void *userData, int (*callback)(int, unsigned int, void *))
{
    wl_event_loop_add_fd(event_loop,fd,WL_EVENT_READABLE,callback,userData);
}

void LWayland::setSeat(LSeat *seat)
{
    // Create seat global
    wl_global_create(display, &wl_seat_interface, LOUVRE_SEAT_VERSION, seat->m_compositor, &Globals::Seat::bind);
    wl_event_loop_add_fd(event_loop,seat->libinputFd(),WL_EVENT_READABLE,&LWayland::processSeat,seat);
}

int LWayland::processSeat(int, unsigned int, void *userData)
{
    LSeat *seat = (LSeat*)userData;
    seat->processInput();
    return 0;
}

UInt32 LWayland::nextSerial()
{
    return wl_display_next_serial(display);
}

int LWayland::initWayland(LCompositor *comp)
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
    wl_global_create(display, &wl_compositor_interface, LOUVRE_COMPOSITOR_VERSION, comp, &Globals::Compositor::bind);

    // Create subcompositor global
    wl_global_create(display, &wl_subcompositor_interface, LOUVRE_SUBCOMPOSITOR_VERSION, comp, &Globals::Subcompositor::bind);

    // Create output global
    wl_global_create(display, &wl_output_interface, 3, comp, &Globals::Output::bind);//3

    // Create data device manager global
    wl_global_create(display, &wl_data_device_manager_interface, 3, comp, &Globals::DataDeviceManager::bind);//3

    // Create xdg shell global
    wl_global_create(display, &xdg_wm_base_interface, LOUVRE_XDG_WM_BASE_VERSION, comp, &Extensions::XdgShell::WmBase::bind);

    wl_display_init_shm(display);
    //wl_data_device_manager_init(display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

    wl_event_loop_add_fd(event_loop,comp->imp()->libinputFd,WL_EVENT_READABLE,&LWayland::readFd,comp);

    comp->imp()->waylandFd = wl_event_loop_get_fd(event_loop);

    // Listen for client connections
    clientConnectedListener.notify = &clientConnectionEvent;
    wl_display_add_client_created_listener(display,&clientConnectedListener);

    printf("Wayland server initialized.\n");
    return wayland_fd;

}

void LWayland::terminateDisplay()
{
   // Ends
    wl_display_terminate(display);
}

void LWayland::dispatchEvents()
{
    wl_event_loop_dispatch(event_loop,0);
}

void LWayland::flushClients()
{
    wl_display_flush_clients(display);
}

int LWayland::readFd(int, unsigned int, void *d)
{
    LCompositor *comp = (LCompositor*)d;
    comp->seat()->processInput();
    updated = false;
    eventfd_read(comp->imp()->libinputFd,&comp->imp()->libinputVal);
    return 0;
}

void LWayland::bindEGLDisplay(EGLDisplay eglDisplay)
{
    eglBindWaylandDisplayWL(eglDisplay, display);
}

pollfd fds[0];

void LWayland::runLoop()
{
    fds[0].events = WL_EVENT_READABLE | WL_EVENT_WRITABLE;
    fds[0].fd = wayland_fd;
    //fds[1].events = POLLIN;
    //fds[1].fd = compositor->imp()->libinputFd;

    while(true)
    {
        poll(fds,1,-1);

        compositor->imp()->m_renderMutex.lock();

        dispatchEvents();

        for(LSurface *surface : compositor->surfaces())
        {
            if(surface != nullptr && surface->roleType() == LSurface::Toplevel)
                surface->toplevel()->imp()->dispachLastConfiguration();
        }

        flushClients();

        compositor->imp()->m_renderMutex.unlock();

    }
}

void LWayland::clientConnectionEvent(wl_listener *listener, void *data)
{
    (void)listener;

    wl_client *client = (wl_client*)data;

    // Let the developer create his own client implementation
    LClient *newClient = compositor->createClientRequest(client);

    // Listen for client disconnection
    //wl_listener *clientDisconnectedListener = new wl_listener;
    //clientDisconnectedListener->notify = &clientDisconnectionEvent;
    //wl_client_add_destroy_listener(client,clientDisconnectedListener);
    wl_client_get_destroy_listener(client,&LWayland::clientDisconnectionEvent);

    // Append client to the compositor list
    compositor->imp()->m_clients.push_back(newClient);
}

wl_iterator_result iter(wl_resource *r,void*)
{
    wl_resource_destroy(r);
    return WL_ITERATOR_CONTINUE;
}

void LWayland::clientDisconnectionEvent(wl_listener *listener, void *data)
{
    //delete listener;

    wl_client *client = (wl_client*)data;

    wl_client_for_each_resource(client,iter,NULL);

    LClient *disconnectedClient = nullptr;

    // Remove client from the compositor list
    for(LClient *wClient: compositor->clients())
    {
        if(wClient->client() == client)
        {
            disconnectedClient = wClient;
            break;
        }
    }

    if(disconnectedClient == nullptr)
        return;

    compositor->destroyClientRequest(disconnectedClient);
    compositor->imp()->m_clients.remove(disconnectedClient);

    delete disconnectedClient;
}


int LWayland::apply_damage_emit(void *data)
{
    wl_signal_emit(&sign,data);
    return 0;
}

wl_event_source *LWayland::addTimer(wl_event_loop_timer_func_t func, void *data)
{
    return wl_event_loop_add_timer(event_loop,func,data);
}

EGLContext LWayland::eglContext()
{
    return sharedContext;
}

EGLDisplay LWayland::eglDisplay()
{
    return sharedDisplay;
}
