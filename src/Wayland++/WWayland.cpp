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

using namespace std;

PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;

WCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;



// XDG SHELL

// xdg toplevel
static void xdg_toplevel_destroy (wl_client *client, wl_resource *resource) {

}
static void xdg_toplevel_set_parent (wl_client *client, wl_resource *resource, wl_resource *parent) {

}
static void xdg_toplevel_set_title (wl_client *client, wl_resource *resource, const char *title) {

}
static void xdg_toplevel_set_app_id (wl_client *client, wl_resource *resource, const char *app_id) {

}
static void xdg_toplevel_show_window_menu (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, Int32 x, Int32 y) {

}
static void xdg_toplevel_move(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    /*
    struct surface *surface = wl_resource_get_user_data (resource);
    // during the move the surface coordinates are relative to the pointer
    surface->x = surface->x - pointer_x;
    surface->y = surface->y - pointer_y;
    moving_surface = surface;
    */
}
static void xdg_toplevel_resize (wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial, UInt32 edges) {

}
static void xdg_toplevel_set_max_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height) {

}
static void xdg_toplevel_set_min_size (wl_client *client, wl_resource *resource, Int32 width, Int32 height) {

}
static void xdg_toplevel_set_maximized (wl_client *client, wl_resource *resource) {
    printf ("surface requested maximize\n");
}
static void xdg_toplevel_unset_maximized (wl_client *client, wl_resource *resource){}
static void xdg_toplevel_set_fullscreen(wl_client *client, wl_resource *resource, wl_resource *output){}
static void xdg_toplevel_unset_fullscreen(wl_client *client, wl_resource *resource){}
static void xdg_toplevel_set_minimized(wl_client *client, wl_resource *resource){}

static struct xdg_toplevel_interface xdg_toplevel_implementation = {&xdg_toplevel_destroy, &xdg_toplevel_set_parent, &xdg_toplevel_set_title, &xdg_toplevel_set_app_id, &xdg_toplevel_show_window_menu, &xdg_toplevel_move, &xdg_toplevel_resize, &xdg_toplevel_set_max_size, &xdg_toplevel_set_min_size, &xdg_toplevel_set_maximized, &xdg_toplevel_unset_maximized, &xdg_toplevel_set_fullscreen, &xdg_toplevel_unset_fullscreen, &xdg_toplevel_set_minimized};

// xdg surface
static void xdg_surface_destroy (wl_client *client, wl_resource *resource) {}

static void xdg_surface_get_toplevel(wl_client *client,wl_resource *resource, UInt32 id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_toplevel = wl_resource_create (client, &xdg_toplevel_interface, 1, id);
    wl_resource_set_implementation (surface->xdg_toplevel, &xdg_toplevel_implementation, surface, NULL);
}
static void xdg_surface_get_popup(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *parent, wl_resource *positioner){}
static void xdg_surface_set_window_geometry(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height){}
static void xdg_surface_ack_configure(wl_client *client, wl_resource *resource, UInt32 serial){}

static struct xdg_surface_interface xdg_surface_implementation = {&xdg_surface_destroy, &xdg_surface_get_toplevel, &xdg_surface_get_popup, &xdg_surface_set_window_geometry, &xdg_surface_ack_configure};

// xdg wm base
static void xdg_wm_base_destroy(wl_client *client, wl_resource *resource){}
static void xdg_wm_base_create_positioner(wl_client *client, wl_resource *resource, UInt32 id){}
static void xdg_wm_base_get_xdg_surface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *_surface)
{
    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(_surface);
    surface->xdg_shell = wl_resource_create(client, &xdg_surface_interface, 1, id);
    wl_resource_set_implementation(surface->xdg_shell, &xdg_surface_implementation, surface, NULL);
}
static void xdg_wm_base_pong(wl_client *client, wl_resource *resource, UInt32 serial){}

static struct xdg_wm_base_interface xdg_wm_base_implementation = {&xdg_wm_base_destroy, &xdg_wm_base_create_positioner, &xdg_wm_base_get_xdg_surface, &xdg_wm_base_pong};
static void xdg_wm_base_bind (wl_client *client, void *data, UInt32 version, UInt32 id)
{
    printf ("NEW XDG-SHELL.\n");

    (void)client;(void)data;(void)version;
    wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, 1, id);
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, NULL, NULL);
}



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
