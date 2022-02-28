#include "Surface.h"
#include <xdg-shell.h>
#include <WBackend.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <WSurface.h>
#include <WClient.h>
#include <stdio.h>
#include <WCompositor.h>

using namespace WaylandPlus;

PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;

EGLint texture_format;

void Globals::Surface::get_egl_func()
{
     eglQueryWaylandBufferWL = (PFNEGLQUERYWAYLANDBUFFERWL) eglGetProcAddress ("eglQueryWaylandBufferWL");
}

void Globals::Surface::delete_surface(wl_resource *resource)
{
    //printf("DESTROY SURFACE.\n");

    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Remove surface from its client list
    surface->getClient()->surfaces.remove(surface);

    // Notify from client
    surface->getClient()->surfaceDestroyRequest(surface);

    if(surface->getCompositor()->getPointerFocusSurface() == surface)
        surface->getCompositor()->clearPointerFocus();

    if(surface->getCompositor()->getKeyboardFocusSurface() == surface)
        surface->getCompositor()->clearKeyboardFocus();

    surface->texture->deleteTexture();
}

// SURFACE
void Globals::Surface::attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    //printf("SURFACE ATTACH: X %i, Y %i\n", x, y);
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->buffer = buffer;
}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);

    //printf("Frame version: %i\n",version);

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->frame_callback = wl_resource_create(client, &wl_callback_interface, version, callback); // 1
}

void Globals::Surface::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;

    delete_surface(resource);
}

void Globals::Surface::commit(wl_client *client, wl_resource *resource)
{
    (void)client;

    /* Client tells the server that the current buffer is ready to be drawed
     * (this means that the current buffer already contains all the damages and transformations) */

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }

    //printf("Commit\n");

    if(eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), surface->buffer, EGL_TEXTURE_FORMAT, &texture_format))
    {
        //printf("EGL buffer\n");
        EGLint width, height;
        eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), surface->buffer, EGL_WIDTH, &width);
        eglQueryWaylandBufferWL(WBackend::getEGLDisplay(), surface->buffer, EGL_HEIGHT, &height);
        EGLAttrib attribs = EGL_NONE;
        EGLImage image = eglCreateImage(WBackend::getEGLDisplay(), EGL_NO_CONTEXT, EGL_WAYLAND_BUFFER_WL, surface->buffer, &attribs);
        surface->texture->setData(width, height, &image, WTexture::Type::EGL);
        eglDestroyImage (WBackend::getEGLDisplay(), image);
    }
    else
    {
        wl_shm_buffer *shm_buffer = wl_shm_buffer_get(surface->buffer);
        //printf("Format:%i\n",wl_shm_buffer_get_format(shm_buffer));
        UInt32 width = wl_shm_buffer_get_width(shm_buffer);
        UInt32 height = wl_shm_buffer_get_height(shm_buffer);
        //printf("Texture height: %i\n",height);
        void *data = wl_shm_buffer_get_data(shm_buffer);
        surface->texture->setData(width, height, data);
    }

    wl_buffer_send_release(surface->buffer);

    if (surface->frame_callback != nullptr)
    {

        wl_callback_send_done(surface->frame_callback,surface->getCompositor()->getMilliseconds());
        wl_resource_destroy(surface->frame_callback);
        surface->frame_callback = nullptr;
    }

    surface->getCompositor()->repaint();

}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{

    (void)client;

    /* The client tells the server that has updated a region of the current buffer */

    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    /*
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
    */

    //printf("Damage (%i,%i,%i,%i)\n",x,y,width,height);

    if(x+width > surface->texture->width() || y+height > surface->texture->height())
        return;



    Rect damage = {x, y, width, height};
    surface->texture->damages.push(damage);
}

void Globals::Surface::set_opaque_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;(void)resource;(void)region;
}

void Globals::Surface::set_input_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;(void)resource;(void)region;
}

void Globals::Surface::set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform)
{
    (void)client;(void)resource;(void)transform;
    //printf("set_buffer_transform: transform %i",transform);
}

void Globals::Surface::damage_buffer(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);

    //printf("damage_buffer:X%i,Y%i,W%i,H%i",x,y,w,h);

    /*
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
    */

    if(x+width > surface->texture->width() || y+height > surface->texture->height())
        return;

    Rect damage = {x, y, width, height};
    surface->texture->damages.push(damage);

}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    printf("BUFFER SCALE:%i\n",scale);
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
}
