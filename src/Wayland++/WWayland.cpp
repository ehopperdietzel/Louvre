#include "WWayland.h"
#include <WCompositor.h>
#include <WBackend.h>
#include <WTexture.h>
#include <pthread.h>
#include <protocols/xdg-shell.h>
#include <WOpenGL.h>
#include <WInput.h>


using namespace std;

static PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;
static PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

WCompositor *compositor;

static struct wl_display *display;
struct wl_event_loop *event_loop;
int wayland_fd;
EGLint texture_format;


static void delete_surface(wl_resource *resource)
{
    printf("DELETE SURFACE.\n");

    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Remove surface from its client list
    surface->client->surfaces.remove(surface);

    // Remove surface
    delete surface;

    // CHECK IF ACTIVE SURFACE

    // CHECK IF IS POINTER SURFACE

    // REPAINT

}

// SURFACE
static void surface_attach(wl_client *client, wl_resource *resource, wl_resource *buffer, int32_t x, int32_t y)
{
    //printf("SURFACE ATTACH.\n");
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->buffer = buffer;
}

static void surface_frame(wl_client *client, wl_resource *resource, uint32_t callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->frame_callback = wl_resource_create (client, &wl_callback_interface, 1, callback);
}

static void surface_destroy(wl_client *client, wl_resource *resource)
{
    printf("DESTROY SURFACE.\n");
}

static void surface_commit(wl_client *client, wl_resource *resource)
{
    //printf("SURFACE COMMIT.\n");

    /* Client tells the server that the current buffer is ready to be drawed
     * (this means that the current buffer already contains all the damages and transformations) */

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);


    if (!surface->buffer)
    {
        xdg_surface_send_configure(surface->xdg_shell, 0);
        return;
    }


    if(eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        EGLint width, height;
        eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_WIDTH, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(getEGLDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, surface->buffer, &attribs);
        surface->texture->setData(width, height, &image, WTexture::Type::EGL);
        eglDestroyImage (getEGLDisplay(), image);
    }
    else
    {
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(surface->buffer);
        uint32_t width = wl_shm_buffer_get_width(shm_buffer);
        uint32_t height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        surface->texture->setData(width, height, data);
    }

    wl_buffer_send_release(surface->buffer);

    if (surface->frame_callback)
    {

        wl_callback_send_done(surface->frame_callback,compositor->getMilliseconds());
        wl_resource_destroy(surface->frame_callback);
        surface->frame_callback = nullptr;
    }

    compositor->repaint();

}


static void surface_damage (wl_client *client, wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height)
{

    /* The client tells the server that has updated a region of the current buffer */

    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    if (!surface->buffer)
    {
        xdg_surface_send_configure(surface->xdg_shell, 0);
        return;
    }

    if(x+width > surface->texture->width() || y+height > surface->texture->height())
        return;

    Rect damage = {0};
    damage.x = x;
    damage.y = y;
    damage.width = width;
    damage.height = height;
    surface->texture->damages.push(damage);
}
static void surface_set_opaque_region (wl_client *client, wl_resource *resource, wl_resource *region){}
static void surface_set_input_region (wl_client *client, wl_resource *resource, wl_resource *region){}


static void surface_set_buffer_transform (struct wl_client *client, struct wl_resource *resource, int32_t transform){}
static void surface_set_buffer_scale(wl_client *client, wl_resource *resource, int32_t scale)
{
    (void)client;
    printf("BUFFER SCALE:%i\n",scale);
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}

static struct wl_surface_interface surface_implementation = {&surface_destroy, &surface_attach, &surface_damage, &surface_frame, &surface_set_opaque_region, &surface_set_input_region, &surface_commit, &surface_set_buffer_transform, &surface_set_buffer_scale};

// REGION
static void region_destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    printf("DELETED REGION\n");
    WRegion *region = (WRegion*)wl_resource_get_user_data(resource);
    region->client->regions.remove(region);
    delete region;
}
static void region_add (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}
static void region_subtract (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}

static struct wl_region_interface region_implementation = {&region_destroy, &region_add, &region_subtract};

// COMPOSITOR
static void compositor_create_surface (wl_client *client, wl_resource *resource, uint32_t id)
{
    printf("NEW SURFACE!\n");

    (void)resource;

    // New surface resource
    wl_resource *surface = wl_resource_create (client, &wl_surface_interface, 4, id);

    // Create surface
    WSurface *wSurface = new WSurface(surface);

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Append surface
    wClient->surfaces.push_back(wSurface);

    // Add client reference to surface
    wSurface->client = wClient;

    // Implement surface
    wl_resource_set_implementation(surface, &surface_implementation, wSurface, &delete_surface);

}
static void compositor_create_region (wl_client *client, wl_resource *resource, uint32_t id)
{
    printf("NEW REGION!\n");

    (void)resource;

    // New region
    wl_resource *region = wl_resource_create (client, &wl_region_interface, 1, id);

    // Create region
    WRegion *wRegion = new WRegion(region);

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Assign client
    wRegion->client = wClient;

    // Append region to client
    wClient->regions.push_back(wRegion);

    // Implement region
    wl_resource_set_implementation (region, &region_implementation, wRegion, NULL);
}

static struct wl_compositor_interface compositor_implementation = {&compositor_create_surface, &compositor_create_region};

static void compositor_bind(wl_client *client, void *data, uint32_t version, uint32_t id)
{
    printf("NEW CLIENT!\n");

    (void)data;(void)version;

    // New client
    wl_resource *resource = wl_resource_create (client, &wl_compositor_interface, 1, id);

    // Create client
    WClient *wClient = new WClient(client,resource);

    // Append client to compositor
    compositor->clients.push_back(wClient);

    // Implement compositor resource
    wl_resource_set_implementation (resource, &compositor_implementation,wClient, NULL);
}

// XDG SHELL

// xdg toplevel
static void xdg_toplevel_destroy (struct wl_client *client, struct wl_resource *resource) {

}
static void xdg_toplevel_set_parent (struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent) {

}
static void xdg_toplevel_set_title (struct wl_client *client, struct wl_resource *resource, const char *title) {

}
static void xdg_toplevel_set_app_id (struct wl_client *client, struct wl_resource *resource, const char *app_id) {

}
static void xdg_toplevel_show_window_menu (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y) {

}
static void xdg_toplevel_move(wl_client *client, wl_resource *resource, wl_resource *seat, uint32_t serial)
{
    /*
    struct surface *surface = wl_resource_get_user_data (resource);
    // during the move the surface coordinates are relative to the pointer
    surface->x = surface->x - pointer_x;
    surface->y = surface->y - pointer_y;
    moving_surface = surface;
    */
}
static void xdg_toplevel_resize (struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial, uint32_t edges) {

}
static void xdg_toplevel_set_max_size (struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height) {

}
static void xdg_toplevel_set_min_size (struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height) {

}
static void xdg_toplevel_set_maximized (struct wl_client *client, struct wl_resource *resource) {
    printf ("surface requested maximize\n");
}
static void xdg_toplevel_unset_maximized (struct wl_client *client, struct wl_resource *resource){}
static void xdg_toplevel_set_fullscreen(wl_client *client, wl_resource *resource, wl_resource *output){}
static void xdg_toplevel_unset_fullscreen(wl_client *client, wl_resource *resource){}
static void xdg_toplevel_set_minimized(wl_client *client, wl_resource *resource){}

static struct xdg_toplevel_interface xdg_toplevel_implementation = {&xdg_toplevel_destroy, &xdg_toplevel_set_parent, &xdg_toplevel_set_title, &xdg_toplevel_set_app_id, &xdg_toplevel_show_window_menu, &xdg_toplevel_move, &xdg_toplevel_resize, &xdg_toplevel_set_max_size, &xdg_toplevel_set_min_size, &xdg_toplevel_set_maximized, &xdg_toplevel_unset_maximized, &xdg_toplevel_set_fullscreen, &xdg_toplevel_unset_fullscreen, &xdg_toplevel_set_minimized};

// xdg surface
static void xdg_surface_destroy (wl_client *client, wl_resource *resource) {}

static void xdg_surface_get_toplevel(wl_client *client,wl_resource *resource, uint32_t id)
{
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->xdg_toplevel = wl_resource_create (client, &xdg_toplevel_interface, 1, id);
    wl_resource_set_implementation (surface->xdg_toplevel, &xdg_toplevel_implementation, surface, NULL);
}
static void xdg_surface_get_popup(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *parent, wl_resource *positioner){}
static void xdg_surface_set_window_geometry(wl_client *client, wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}
static void xdg_surface_ack_configure(wl_client *client, wl_resource *resource, uint32_t serial){}

static struct xdg_surface_interface xdg_surface_implementation = {&xdg_surface_destroy, &xdg_surface_get_toplevel, &xdg_surface_get_popup, &xdg_surface_set_window_geometry, &xdg_surface_ack_configure};

// xdg wm base
static void xdg_wm_base_destroy(wl_client *client, wl_resource *resource){}
static void xdg_wm_base_create_positioner(wl_client *client, wl_resource *resource, uint32_t id){}
static void xdg_wm_base_get_xdg_surface(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *_surface)
{
    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(_surface);
    surface->xdg_shell = wl_resource_create(client, &xdg_surface_interface, 1, id);
    wl_resource_set_implementation(surface->xdg_shell, &xdg_surface_implementation, surface, NULL);
}
static void xdg_wm_base_pong(wl_client *client, wl_resource *resource, uint32_t serial){}

static struct xdg_wm_base_interface xdg_wm_base_implementation = {&xdg_wm_base_destroy, &xdg_wm_base_create_positioner, &xdg_wm_base_get_xdg_surface, &xdg_wm_base_pong};
static void xdg_wm_base_bind (wl_client *client, void *data, uint32_t version, uint32_t id)
{
    printf ("NEW XDG-SHELL.\n");

    (void)client;(void)data;(void)version;
    struct wl_resource *resource = wl_resource_create (client, &xdg_wm_base_interface, 1, id);
    wl_resource_set_implementation (resource, &xdg_wm_base_implementation, NULL, NULL);
}


// POINTER
static void pointer_set_cursor (wl_client *client, wl_resource *resource, uint32_t serial, wl_resource *_surface, int32_t hotspot_x, int32_t hotspot_y)
{
    printf("CURSOR SURFACE\n");
    //struct surface *surface = wl_resource_get_user_data(_surface);
    //cursor = surface;
}
static void pointer_release(wl_client *client, wl_resource *resource)
{
    printf("POINTER RELEASED\n");
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource_destroy(wClient->pointer);
    wClient->pointer = nullptr;
}
static struct wl_pointer_interface pointer_implementation = {&pointer_set_cursor, &pointer_release};

// KEYBOARD
static void keyboard_release(wl_client *client, wl_resource *resource)
{
    printf("KEYBOARD RELEASED\n");
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource_destroy(wClient->keyboard);
    wClient->keyboard = nullptr;
}
static struct wl_keyboard_interface keyboard_implementation = {&keyboard_release};

// SEAT
static void seat_get_pointer (wl_client *client, wl_resource *resource, uint32_t id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource *pointer = wl_resource_create (client, &wl_pointer_interface, 7, id);
    wl_resource_set_implementation (pointer, &pointer_implementation, wClient, NULL);
    wClient->pointer = pointer;
}

static void seat_get_keyboard (wl_client *client, wl_resource *resource, uint32_t id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource *keyboard = wl_resource_create(client, &wl_keyboard_interface,7, id);
    wl_resource_set_implementation(keyboard, &keyboard_implementation, NULL, NULL);
    wClient->keyboard = keyboard;
    wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,getKeymapFD(),getKeymapSize());
}
static void seat_get_touch (wl_client *client, wl_resource *resource, uint32_t id){}

static void seat_release( wl_client *client, wl_resource *resource)
{
    printf("SEAT RELEASED\n");
    wl_resource_destroy(resource);
}

static struct wl_seat_interface seat_implementation = {&seat_get_pointer, &seat_get_keyboard, &seat_get_touch,&seat_release};
static void seat_bind(wl_client *client, void *data, uint32_t version, uint32_t id)
{
    (void)data;(void)version;
    wl_resource *seat = wl_resource_create(client, &wl_seat_interface,version,id);
    WClient *wClient = *find_if(compositor->clients.begin(),compositor->clients.end(),[client](WClient *x) { return x->client == client;});
    wl_resource_set_implementation(seat, &seat_implementation, wClient, NULL);
    wl_seat_send_capabilities (seat, WL_SEAT_CAPABILITY_POINTER|WL_SEAT_CAPABILITY_KEYBOARD);

}

int initWayland(WCompositor *comp)
{

    eglBindWaylandDisplayWL = (PFNEGLBINDWAYLANDDISPLAYWL) eglGetProcAddress ("eglBindWaylandDisplayWL");
    eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");

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
    wl_global_create (display, &wl_compositor_interface, 3, NULL, &compositor_bind);

    // Create xdg shell global
    wl_global_create (display, &xdg_wm_base_interface, 1, NULL, &xdg_wm_base_bind);

    // Create seat global
    wl_global_create (display, &wl_seat_interface, 7, NULL, &seat_bind);

    eglBindWaylandDisplayWL(getEGLDisplay(), display);

    wl_display_init_shm (display);

    event_loop = wl_display_get_event_loop(display);
    wayland_fd = wl_event_loop_get_fd(event_loop);

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
