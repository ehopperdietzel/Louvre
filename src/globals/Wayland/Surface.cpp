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
#include <unistd.h>
#include <sys/eventfd.h>

using namespace Wpp;

void Globals::Surface::resource_destroy(wl_resource *resource)
{
    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    WClient *client = surface->client();

    if(client != nullptr)
    {
        // Remove surface from its client list
        surface->client()->surfaces.remove(surface);

        // Notify from client
        surface->client()->surfaceDestroyRequest(surface);
    }

    delete surface;
}

// SURFACE
void Globals::Surface::attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->pendingBuffer = buffer;
}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->frame_callback = wl_resource_create(client, &wl_callback_interface, version, callback); // 1
}

void Globals::Surface::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Globals::Surface::commit(wl_client *client, wl_resource *resource)
{
    (void)client;

    /* Client tells the server that the current buffer is ready to be drawed.
     * (this means that the current buffer already contains all the damages and transformations) */

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    surface->committedBuffer = surface->pendingBuffer;
    surface->texture()->damages.swap(surface->texture()->pendingDamages);

    while(!surface->texture()->pendingDamages.empty())
        surface->texture()->pendingDamages.pop();

    if (surface->committedBuffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }

    surface->_isDamaged = true;

    // FALTA ENVIAR EVENTO
    surface->compositor()->repaintAllOutputs();
}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    /* The client tells the server that has updated a region of the current buffer */
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    Int32 s = surface->bufferScale();

    Wpp::Rect damage = {x*s, y*s, width*s, height*s};

    surface->texture()->pendingDamages.push(damage);
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
    printf("set_buffer_transform: transform %i",transform);
}

void Globals::Surface::damage_buffer(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    surface->texture()->pendingDamages.push({x, y, width, height});
}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}
