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
    surface->pending.buffer = buffer;
    surface->texture()->resizeDirection = WPoint(x,y);
}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    surface->pending.callbacks.push_back(wl_resource_create(client, &wl_callback_interface, version, callback));
    //surface->frame_callback = wl_resource_create(client, &wl_callback_interface, version, callback); // 1
}

void Globals::Surface::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}

void Globals::Surface::commit(wl_client *client, wl_resource *resource)
{
    (void)client;

    /* Client tells the server that the current buffer is ready to be drawn.
     * (this means that the current buffer already contains all the damages and transformations) */

    // Get surface reference
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    surface->current.buffer = surface->pending.buffer;

    if(surface->current.buffer == nullptr)
    {
        if(surface->xdg_shell)
        {
            xdg_surface_send_configure(surface->xdg_shell,surface->configureSerial);
            surface->configureSerial++;
        }

        if(surface->pending.type == SurfaceType::Toplevel)
        {
            surface->current.type = surface->pending.type;
            surface->pending.type = SurfaceType::Undefined;

            surface->sendConfigureToplevelEvent(0,0,SurfaceState::Activated);
            surface->dispachLastConfiguration();
            surface->typeChangeRequest();
        }
        return;
    }

    surface->texture()->damages.clear();
    for(list<WRect>::iterator r = surface->texture()->pendingDamages.begin(); r != surface->texture()->pendingDamages.end(); ++r)
    {
        surface->texture()->damages.push_back(*r);
    }
    surface->texture()->pendingDamages.clear();

    surface->current.callbacks.merge(surface->pending.callbacks);
    surface->pending.callbacks.clear();


    surface->_isDamaged = true;

    surface->applyDamages();

    // FALTA ENVIAR EVENTO
    surface->compositor()->repaintAllOutputs();
}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    /* The client tells the server that has updated a region of the current buffer */
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->texture()->pendingDamages.push_back(WRect(x, y, width, height)*surface->bufferScale());
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
    surface->texture()->pendingDamages.push_back(WRect(x, y, width, height));
}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}

void Globals::Surface::offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)x;(void)y;
}
