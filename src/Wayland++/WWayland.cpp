#include "WWayland.h"
#include <WCompositor.h>
#include <WBackend.h>
#include <WTexture.h>
#include <pthread.h>
#include <protocols/xdg-shell.h>
#include <WOpenGL.h>
#include <WInput.h>

#include <globals/Compositor.h>
#include <globals/Surface.h>
#include <globals/Seat.h>
#include <globals/XdgWmBase.h>

using namespace std;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

WCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;



int initWayland(WCompositor *comp)
{
    eglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWL) eglGetProcAddress ("eglBindWaylandDisplayWL");
    surface_get_egl_func();

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
    wl_global_create(display, &wl_compositor_interface, 3, comp, &compositor_bind);

    // Create xdg shell global
    wl_global_create(display, &xdg_wm_base_interface, 1, NULL, &xdg_wm_base_bind);

    // Create seat global
    wl_global_create(display, &wl_seat_interface, 7, comp, &seat_bind);

    eglBindWaylandDisplayWL(getEGLDisplay(), display);

    wl_display_init_shm (display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

    createNullKeys();

    printf("Wayland server initialized.\n");

    return wayland_fd;

}

void terminateDisplay()
{
   // Ends
   wl_display_terminate(display);
}

void processWayland()
{
    wl_event_loop_dispatch(event_loop,0);
    wl_display_flush_clients(display);
}
