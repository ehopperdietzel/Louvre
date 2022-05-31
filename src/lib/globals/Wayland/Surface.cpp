#include "Surface.h"

#include <LBackend.h>
#include <LCompositor.h>
#include <LSurface.h>
#include <LClient.h>
#include <LSeat.h>
#include <LPopup.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <LToplevel.h>

using namespace Louvre;

void Globals::Surface::resource_destroy(wl_resource *resource)
{
    // Get surface
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

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

    // Parent
    if(surface->parent() != nullptr)
        surface->parent()->p_children.remove(surface);

    LClient *client = surface->client();

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
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->pending.buffer = buffer;
}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

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
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    // Makes the pending buffer the current buffer
    surface->current.buffer = surface->pending.buffer;

    // If the buffer is empty
    if(surface->current.buffer == nullptr)
    {
        if(surface->pending.type == LSurface::SurfaceType::Toplevel)
        {
            surface->current.type = surface->pending.type;
            surface->pending.type = LSurface::SurfaceType::Undefined;
            surface->toplevel()->configureRequest();
            surface->typeChangeRequest();
        }
        else if(surface->pending.type == LSurface::SurfaceType::Popup)
        {
            surface->current.type = surface->pending.type;
            surface->pending.type = LSurface::SurfaceType::Undefined;
            surface->popup()->configureRequest();
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


    /************************************
     *********** SURFACE SIZE ***********
     ************************************/
    surface->pending.size = surface->texture()->size()/surface->bufferScale();
    if(surface->current.size != surface->pending.size)
    {
        surface->current.size = surface->pending.size;
        surface->bufferSizeChangeRequest();
    }


    // Input region
    if(surface->pending.inputRegion.p_rects.empty())
    {
        surface->current.inputRegion.clear();
        surface->current.inputRegion.addRect(LRect(LPoint(),surface->size()));
    }
    else
        surface->current.inputRegion.copy(surface->pending.inputRegion);

    
    // Toplevel configure
    if(surface->type() == LSurface::Toplevel)
    {
        LToplevelRole *topLevel = surface->toplevel();

        if(topLevel->p_pendingConf.set)
        {
            unsigned char prevState = topLevel->p_stateFlags;
            topLevel->p_stateFlags = topLevel->p_sentConf.flags;

            if((prevState & LToplevelRole::Maximized) != (topLevel->p_sentConf.flags & LToplevelRole::Maximized))
                topLevel->maximizeChanged();
            if((prevState & LToplevelRole::Fullscreen) != (topLevel->p_sentConf.flags & LToplevelRole::Fullscreen))
                topLevel->fullscreenChanged();

            topLevel->p_pendingConf.set = false;
        }
    }
            


    // Notify that the cursor changed content
    if(surface->type() == LSurface::Cursor)
        surface->compositor()->seat()->setCursorRequest(surface,surface->hotspot().x(),surface->hotspot().y());


    // FALTA ENVIAR EVENTO
    surface->compositor()->repaintAllOutputs();
}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    /* The client tells the server that has updated a region of the current buffer */
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->texture()->pendingDamages.push_back(LRect(x, y, width, height)*surface->bufferScale());
}

void Globals::Surface::set_opaque_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;(void)resource;(void)region;
}

void Globals::Surface::set_input_region(wl_client *client, wl_resource *resource, wl_resource *region)
{
    (void)client;

    LSurface *wSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(region == NULL)
        wSurface->pending.inputRegion.clear();
    else
    {
        LRegion *wRegion = (LRegion*)wl_resource_get_user_data(region);
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
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->texture()->pendingDamages.push_back(LRect(x, y, width, height));
}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}

/*
void Globals::Surface::offset(wl_client *client, wl_resource *resource, Int32 x, Int32 y)
{
    (void)client;(void)resource;(void)x;(void)y;
}
*/

