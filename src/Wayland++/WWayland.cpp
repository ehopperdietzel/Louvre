#include "WWayland.h"
#include <WCompositor.h>
#include <WBackendDRM.h>
#include <WTexture.h>


using namespace std;

WCompositor *compositor;

static struct wl_display *display;

struct client {
    struct wl_client *client;
    struct wl_resource *pointer;
    struct wl_resource *keyboard;
    struct wl_list link;
};
static struct wl_list clients;


static struct client *get_client (struct wl_client *_client) {
    struct client *client;
    wl_list_for_each (client, &clients, link) {
        if (client->client == _client) return client;
    }
    client = calloc (1, sizeof(struct client));
    client->client = _client;
    wl_list_insert (&clients, &client->link);
    return client;
}

struct surface {
    struct wl_resource *surface;
    struct wl_resource *xdg_surface;
    struct wl_resource *xdg_toplevel;
    struct wl_resource *buffer;
    struct wl_resource *frame_callback;
    int x, y;
    //struct texture texture;
    struct client *client;
    struct wl_list link;
};
static struct wl_list surfaces;
static struct surface *cursor = NULL;
static struct surface *moving_surface = NULL;
static struct surface *active_surface = NULL;
static struct surface *pointer_surface = NULL; // surface under the pointer

/*
static void deactivate_surface (struct surface *surface) {
    if (surface->client->keyboard) wl_keyboard_send_leave (surface->client->keyboard, 0, surface->surface);
    struct wl_array state_array;
    wl_array_init (&state_array);
    xdg_toplevel_send_configure (surface->xdg_toplevel, 0, 0, &state_array);
}
static void activate_surface (struct surface *surface)
{
    wl_list_remove (&surface->link);
    wl_list_insert (&surfaces, &surface->link);
    struct wl_array array;
    wl_array_init (&array);
    if (surface->client->keyboard) {
        wl_keyboard_send_enter (surface->client->keyboard, 0, surface->surface, &array);
        wl_keyboard_send_modifiers (surface->client->keyboard, 0, modifier_state.depressed, modifier_state.latched, modifier_state.locked, modifier_state.group);
    }
    int32_t *states = wl_array_add (&array, sizeof(int32_t));
    states[0] = XDG_TOPLEVEL_STATE_ACTIVATED;
    xdg_toplevel_send_configure (surface->xdg_toplevel, 0, 0, &array);
}
*/

static void delete_surface(struct wl_resource *resource)
{
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
static void surface_attach(struct wl_client *client, struct wl_resource *resource, struct wl_resource *buffer, int32_t x, int32_t y)
{
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->buffer = buffer;
}

static void surface_frame(wl_client *client, wl_resource *resource, uint32_t callback)
{
    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->frame_callback = wl_resource_create (client, &wl_callback_interface, 1, callback);
}

static void surface_destroy (struct wl_client *client, struct wl_resource *resource) {}
static void surface_damage (wl_client *client, wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}
static void surface_set_opaque_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region){}
static void surface_set_input_region (struct wl_client *client, struct wl_resource *resource, struct wl_resource *region){}

static void surface_commit (struct wl_client *client, struct wl_resource *resource)
{
    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    /*
    if (!surface->buffer)
    {
        xdg_surface_send_configure(surface->xdg_surface, 0);
        return;
    }
    */

    EGLint texture_format;
    if(eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        EGLint width, height;
        eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(getEGLDisplay(), surface->buffer, EGL_WIDTH, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(getEGLDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, surface->buffer, &attribs);
        texture_delete(&surface->texture);
        texture_create_from_egl_image (&surface->texture, width, height, image);
        eglDestroyImage (getEGLDisplay(), image);
    }
    else {
        struct wl_shm_buffer *shm_buffer = wl_shm_buffer_get (surface->buffer);
        uint32_t width = wl_shm_buffer_get_width (shm_buffer);
        uint32_t height = wl_shm_buffer_get_height (shm_buffer);
        void *data = wl_shm_buffer_get_data (shm_buffer);
        texture_delete (&surface->texture);
        texture_create (&surface->texture, width, height, data);
    }
    wl_buffer_send_release (surface->buffer);
    redraw_needed = 1;
}

static void surface_set_buffer_transform (struct wl_client *client, struct wl_resource *resource, int32_t transform){}
static void surface_set_buffer_scale (struct wl_client *client, struct wl_resource *resource, int32_t scale){}

static struct wl_surface_interface surface_implementation = {&surface_destroy, &surface_attach, &surface_damage, &surface_frame, &surface_set_opaque_region, &surface_set_input_region, &surface_commit, &surface_set_buffer_transform, &surface_set_buffer_scale};

// REGION
static void region_destroy (struct wl_client *client, struct wl_resource *resource){}
static void region_add (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}
static void region_subtract (struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y, int32_t width, int32_t height){}

static struct wl_region_interface region_implementation = {&region_destroy, &region_add, &region_subtract};

// COMPOSITOR
static void compositor_create_surface (wl_client *client, wl_resource *resource, uint32_t id)
{
    (void)resource;

    // New surface resource
    wl_resource *surface = wl_resource_create (client, &wl_surface_interface, 3, id);

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
    (void)resource;

    // New region
    wl_resource *region = wl_resource_create (client, &wl_region_interface, 1, id);

    // Create region
    WRegion *wRegion = new WRegion(region);

    // Find client
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    // Append region to client
    wClient->regions.push_back(wRegion);

    // Implement region
    wl_resource_set_implementation (region, &region_implementation, wRegion, NULL);
}

static struct wl_compositor_interface compositor_implementation = {&compositor_create_surface, &compositor_create_region};

static void compositor_bind(wl_client *client, void *data, uint32_t version, uint32_t id)
{
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

void initWayland(WCompositor *comp)
{
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

    // Start the wayland event loop
    wl_display_run(display);

    // to stop
    //wl_display_terminate(display);
}
