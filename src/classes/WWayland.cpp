#include "WWayland.h"
#include <WCompositor.h>
#include <WBackend.h>
#include <WTexture.h>
#include <pthread.h>
#include <xdg-shell.h>
#include <WOpenGL.h>
#include <WInput.h>

#include <Compositor.h>
#include <Subcompositor.h>
#include <Surface.h>
#include <Seat.h>
#include <DataDeviceManager.h>
#include <Output.h>
#include <XdgWmBase.h>

using namespace std;
using namespace WaylandPlus;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

WCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;
wl_event_source *renderTimer;


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
    wl_global_create(display, &wl_subcompositor_interface, 1, comp, &Globals::Subcompositor::bind); // 1

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
    flushClients();
    WInput::processInput(0,0,comp);
    eventfd_read(comp->libinputFd,&comp->libinputVal);

}

void WWayland::bindEGLDisplay(EGLDisplay eglDisplay)
{
    eglBindWaylandDisplayWL(eglDisplay, display);
}

void WWayland::runLoop()
{
    wl_display_run(display);
}
