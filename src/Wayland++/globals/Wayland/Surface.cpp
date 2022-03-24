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

using namespace WaylandPlus;


void Globals::Surface::delete_surface(wl_resource *resource)
{
    //printf("DESTROY SURFACE.\n");

    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Remove surface from its client list
    surface->getClient()->surfaces.remove(surface);

    // Notify from client
    surface->getClient()->surfaceDestroyRequest(surface);

    surface->_texture->deleteTexture();
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

    surface->_isDamaged = true;

    // FALTA ENVIAR EVENTO
    surface->getCompositor()->repaintAllOutputs();

}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    /* The client tells the server that has updated a region of the current buffer */
    WSurface *surface = (WSurface*)wl_resource_get_user_data (resource);
    //surface->getCompositor()->val++;
    //eventfd_write(surface->getCompositor()->compositorFd,surface->getCompositor()->val);



    /*
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
    */

    Int32 s = surface->getBufferScale();

    //if((x+width)*s > surface->_texture->width() || (y+height)*s > surface->_texture->height())
        //return;


    WaylandPlus::Rect damage = {x*s, y*s, width*s, height*s};

    //if(damage.width > 10000000 || damage.height > 1000000)
        //return;

    //printf("Damage (%i,%i,%i,%i)\n",damage.x,damage.y,damage.width,damage.height);
    surface->_texture->damages.push(damage);

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
    //eventfd_write(surface->getCompositor()->compositorFd,surface->getCompositor()->val);
    //surface->getCompositor()->val++;

    //printf("damage_buffer:X%i,Y%i,W%i,H%i",x,y,width,height);

    /*
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
    */

    //if(x+width > surface->_texture->width() || y+height > surface->_texture->height())
        //return;

    surface->_texture->damages.push({x, y, width, height});

}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    //printf("BUFFER SCALE:%i\n",scale);
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    //eventfd_write(surface->getCompositor()->compositorFd,surface->getCompositor()->val);
    //surface->getCompositor()->val++;
    surface->setBufferScale(scale);

    /*
    if (surface->buffer == nullptr)
    {
        surface->sendConfigureEvent(0,0,SurfaceState::Activated);
        return;
    }
    */
}
