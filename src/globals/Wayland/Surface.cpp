#include "Surface.h"

#include <WBackend.h>
#include <WCompositor.h>
#include <WSurface.h>
#include <WClient.h>
#include <WSeat.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <WToplevelRole.h>

using namespace Wpp;

void Globals::Surface::resource_destroy(wl_resource *resource)
{
    // Get surface
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    // Clear keyboard focus
    if(surface->compositor()->seat()->p_keyboardFocusSurface == surface)
        surface->compositor()->seat()->p_keyboardFocusSurface = nullptr;

    // Clear pointer focus
    if(surface->compositor()->seat()->p_pointerFocusSurface == surface)
        surface->compositor()->seat()->p_pointerFocusSurface = nullptr;

    // Clear dragging surface
    if(surface->compositor()->seat()->p_draggingSurface == surface)
        surface->compositor()->seat()->p_draggingSurface = nullptr;

    // Clear active toplevel focus
    if(surface->compositor()->seat()->p_activeTopLevel == surface->toplevel())
        surface->compositor()->seat()->p_activeTopLevel = nullptr;

    // Clear moving toplevel
    if(surface->compositor()->seat()->p_movingTopLevel == surface->toplevel())
        surface->compositor()->seat()->p_movingTopLevel = nullptr;

    // Clear resizing toplevel
    if(surface->compositor()->seat()->p_resizingToplevel == surface->toplevel())
        surface->compositor()->seat()->p_resizingToplevel = nullptr;

    WClient *client = surface->client();

    if(client != nullptr)
    {
        // Remove surface from its client list
        surface->client()->surfaces.remove(surface);

        // Remove the surface from the compositor list
        surface->compositor()->p_surfaces.remove(surface);

        // Notify from client
        surface->compositor()->destroySurfaceRequest(surface);
    }

    surface->texture()->deleteTexture();
    delete surface;
}

// SURFACE
void Globals::Surface::attach(wl_client *client, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    (void)client;(void)x;(void)y;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
    surface->pending.buffer = buffer;
}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);

    if(surface->p_frameCallback)
        wl_resource_destroy(surface->p_frameCallback);

    surface->p_frameCallback = wl_resource_create(client, &wl_callback_interface, version, callback);
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

    // Makes the pending buffer the current buffer
    surface->current.buffer = surface->pending.buffer;

    // If the buffer is empty
    if(surface->current.buffer == nullptr)
    {

        if(surface->p_xdg_shell)
            xdg_surface_send_configure(surface->p_xdg_shell,1);

        if(surface->pending.type == WSurface::SurfaceType::Toplevel)
        {
            surface->current.type = surface->pending.type;
            surface->pending.type = WSurface::SurfaceType::Undefined;

            surface->toplevel()->configureRequest();
            surface->typeChangeRequest();
        }
        return;
    }

    // Copy pending damages to current damages
    surface->texture()->damages.clear();
    std::copy(surface->texture()->pendingDamages.begin(),
              surface->texture()->pendingDamages.end(),
              std::back_inserter(surface->texture()->damages));
    surface->texture()->pendingDamages.clear();

    surface->p_isDamaged = true;

    // Convert the buffer to OpenGL texture
    surface->applyDamages();

    // Input region
    if(surface->pending.inputRegion.p_rects.empty())
    {
        surface->current.inputRegion.clear();
        surface->current.inputRegion.addRect(WRect(WPoint(),surface->texture()->size()));
        surface->current.inputRegion.multiply(1.f/double(surface->bufferScale()));
    }
    else
        surface->current.inputRegion.copy(surface->pending.inputRegion);

    
    // Toplevel configure
    if(surface->type() == WSurface::Toplevel)
    {
        WToplevelRole *topLevel = surface->toplevel();

        if(topLevel->p_pendingConf.set)
        {
            unsigned char prevState = topLevel->p_stateFlags;
            topLevel->p_stateFlags = topLevel->p_sentConf.flags;

            if(!(prevState & WToplevelRole::Maximized) && (topLevel->p_sentConf.flags & WToplevelRole::Maximized))
                topLevel->maximizeChanged();
            if((prevState & WToplevelRole::Maximized) && !(topLevel->p_sentConf.flags & WToplevelRole::Maximized))
                topLevel->maximizeChanged();

            topLevel->p_pendingConf.set = false;
        }
    }
            


    // Notify that the cursor changed content
    if(surface->type() == WSurface::Cursor)
        surface->compositor()->seat()->setCursorRequest(surface,surface->hotspot().x(),surface->hotspot().y());


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
    (void)client;

    WSurface *wSurface = (WSurface*)wl_resource_get_user_data(resource);

    if(region == NULL)
        wSurface->pending.inputRegion.clear();
    else
    {
        WRegion *wRegion = (WRegion*)wl_resource_get_user_data(region);
        wSurface->current.inputRegion.p_client = wRegion->p_client;
        wSurface->pending.inputRegion.copy(*wRegion);
    }
}

void Globals::Surface::set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform)
{
    (void)client;(void)resource;(void)transform;
    printf("set_buffer_transform: transform %i",transform);
}

void Globals::Surface::damage_buffer(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;
    WSurface *surface = (WSurface*)wl_resource_get_user_data(resource);
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
