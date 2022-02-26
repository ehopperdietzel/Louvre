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

int WWayland::initWayland(WCompositor *comp)
{
    eglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWL) eglGetProcAddress ("eglBindWaylandDisplayWL");
    Globals::Surface::get_egl_func();

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
    //wl_global_create(display, &zxdg_shell_v6_interface, 1, comp, &Extensions::XdgShell::ShellV6::bind);//4
    wl_global_create(display, &xdg_wm_base_interface, 4, comp, &Extensions::XdgShell::WmBase::bind);

    eglBindWaylandDisplayWL(WBackend::getEGLDisplay(), display);

    wl_display_init_shm(display);
    //wl_data_device_manager_init(display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

    createNullKeys();

    printf("Wayland server initialized.\n");

    return wayland_fd;

}

void WWayland::terminateDisplay()
{
   // Ends
   wl_display_terminate(display);
}

void WWayland::processWayland()
{
    wl_event_loop_dispatch(event_loop,0);
    wl_display_flush_clients(display);
}