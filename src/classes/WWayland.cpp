#include "WWayland.h"
#include <WCompositor.h>
#include <WBackend.h>
#include <WTexture.h>
#include <pthread.h>
#include <xdg-shell.h>
#include <WOpenGL.h>
#include <WInput.h>
#include <Region.h>

#include <Compositor.h>
#include <Subcompositor.h>
#include <Surface.h>
#include <Seat.h>
#include <DataDeviceManager.h>
#include <Output.h>
#include <XdgWmBase.h>

#include <sys/poll.h>

#include <WSurface.h>

using namespace std;
using namespace Wpp;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

WCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;
wl_event_source *renderTimer;
wl_listener clientConnectedListener,clientDisconnectedListener;
wl_event_source *clientDisconnectedEventSource;
wl_signal sign;
wl_listener signListen;

int WWayland::initWayland(WCompositor *comp)
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
    wl_global_create(display, &wl_compositor_interface, 4, comp, &Globals::Compositor::bind);//4

    // Create subcompositor global
    //wl_global_create(display, &wl_subcompositor_interface, 1, comp, &Globals::Subcompositor::bind); // 1

    // Create seat global
    wl_global_create(display, &wl_seat_interface, 7, comp, &Globals::Seat::bind);//7

    // Create output global
    wl_global_create(display, &wl_output_interface, 3, comp, &Globals::Output::bind);//3

    // Create data device manager global
    wl_global_create(display, &wl_data_device_manager_interface, 3, comp, &Globals::DataDeviceManager::bind);//3

    // Create xdg shell global
    wl_global_create(display, &xdg_wm_base_interface, 4, comp, &Extensions::XdgShell::WmBase::bind);

    //eglBindWaylandDisplayWL(WBackend::getEGLDisplay(), display);

    wl_display_init_shm(display);
    //wl_data_device_manager_init(display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

    wl_event_loop_add_fd(event_loop,WInput::getLibinputFd(),WL_EVENT_READABLE,&WInput::processInput,NULL);

    wl_event_loop_add_fd(event_loop,comp->libinputFd,WL_EVENT_READABLE,&WWayland::readFd,comp);

    createNullKeys();

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
    WCompositor *comp = (WCompositor*)d;
    eventfd_read(comp->libinputFd,&comp->libinputVal);
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

    fds[1].events = POLLIN;
    fds[1].fd = compositor->libinputFd;

    fds[2].events = POLLIN;
    fds[2].fd = WInput::getLibinputFd();

    while(true)
    {
        poll(fds,3,-1);

        /*
        if(fds[1].revents & WL_EVENT_READABLE)
        {
            eventfd_read(compositor->libinputFd,&compositor->libinputVal);
        }
        */

        compositor->renderMutex.lock();


        //if(fds[2].revents & POLLIN)
            //WInput::processInput(0,0,NULL);


        /*
        for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
        {
            for(list<WSurface*>::iterator s = (*c)->surfaces.begin(); s != (*c)->surfaces.end(); ++s)
            {
                WWayland::apply_damage_emit(*s);
            }
        }
        */

        flushClients();
        dispatchEvents();



        /*
        if(fds[0].revents & WL_EVENT_READABLE)
            dispatchEvents();


        if(fds[1].revents & POLLIN)
            eventfd_read(compositor->libinputFd,&compositor->libinputVal);
        */
        compositor->renderMutex.unlock();



    }
    //wl_display_run(display);
}

void WWayland::clientConnectionEvent(wl_listener *listener, void *data)
{
    (void)listener;

    wl_client *client = (wl_client*)data;

    // Let the developer create his own client implementation
    WClient *newClient = compositor->newClientRequest(client);

    // Listen for client disconnection
    wl_client_get_destroy_listener(client,&WWayland::clientDisconnectionEvent);

    // Append client to the compositor list
    compositor->clients.push_back(newClient);
}

void WWayland::clientDisconnectionEvent(wl_listener *listener, void *data)
{

    (void)listener;

    wl_client *client = (wl_client*)data;

    WClient *disconnectedClient = nullptr;

    // Remove client from the compositor list
    for(list<WClient*>::iterator c = compositor->clients.begin(); c != compositor->clients.end(); ++c)
    {
        if((*c)->getClient() == client)
        {
            disconnectedClient = *c;
            break;
        }
    }

    if(disconnectedClient == nullptr)
        return;

    // Destroy surfaces events
    for(list<WSurface*>::iterator s = disconnectedClient->surfaces.begin(); s != disconnectedClient->surfaces.end(); ++s)
    {
        disconnectedClient->surfaceDestroyRequest(*s);
        (*s)->_client = nullptr;
    }

    disconnectedClient->surfaces.clear();

    compositor->clients.remove(disconnectedClient);

    compositor->clientDisconnectRequest(disconnectedClient);

    delete disconnectedClient;
}

int WWayland::apply_damage_emit(void *data)
{
    wl_signal_emit(&sign,data);
}



wl_event_source *WWayland::addTimer(wl_event_loop_timer_func_t func, void *data)
{
    return wl_event_loop_add_timer(event_loop,func,data);
}
