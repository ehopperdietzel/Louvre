#include "Surface.h"
#include <protocols/xdg-shell.h>
#include <WBackend.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WSurface.h>
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

EGLint texture_format;

void surface_get_egl_func()
{
     eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
}

void delete_surface(wl_resource *resource)
{
    printf("DESTROY SURFACE.\n");

    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Remove surface from its client list
    surface->getClient()->surfaces.remove(surface);

    // Notify from client
    surface->getClient()->surfaceDestroyed(surface);

    // Notify from compositor
    surface->getCompositor()->surfaceDestroyed(surface);

    if(surface->getCompositor()->getPointerFocusSurface() == surface)
        surface->getCompositor()->clearPointerFocus();

    if(surface->getCompositor()->getKeyboardFocusSurface() == surface)
        surface->getCompositor()->clearKeyboardFocus();

    // Remove surface
    delete surface;
}

// SURFACE
void surface_attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    //printf("SURFACE ATTACH.\n");
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->buffer = buffer;
}

void surface_frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->frame_callback = wl_resource_create(client, &wl_callback_interface, 1, callback);
}

void surface_destroy(wl_client *client, wl_resource *resource)
{
    (void)client;

    delete_surface(resource);
}

void surface_commit(wl_client *client, wl_resource *resource)
{
    (void)client;

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
        UInt32 width = wl_shm_buffer_get_width(shm_buffer);
        UInt32 height = wl_shm_buffer_get_height(shm_buffer);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        surface->texture->setData(width, height, data);
    }

    wl_buffer_send_release(surface->buffer);

    if (surface->frame_callback)
    {

        wl_callback_send_done(surface->frame_callback,surface->getCompositor()->getMilliseconds());
        wl_resource_destroy(surface->frame_callback);
        surface->frame_callback = nullptr;
    }

    surface->getCompositor()->repaint();

}


void surface_damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{

    (void)client;

    /* The client tells the server that has updated a region of the current buffer */

    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    if (!surface->buffer)
    {
        xdg_surface_send_configure(surface->xdg_shell, 0);
        return;
    }

    if(x+width > surface->texture->width() || y+height > surface->texture->height())
        return;

    Rect damage = {x, y, width, height};
    surface->texture->damages.push(damage);
}

void surface_set_opaque_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;(void)resource;(void)region;
}

void surface_set_input_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;(void)resource;(void)region;
}

void surface_set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform)
{
    (void)client;(void)resource;(void)transform;
}

void surface_damage_buffer(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 w, Int32 h)
{
    (void)client;(void)resource;(void)x;(void)y;(void)w;(void)h;
}

void surface_set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    printf("BUFFER SCALE:%i\n",scale);
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}
