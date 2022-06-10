#include "Surface.h"

#include <LCompositor.h>
#include <LSurface.h>
#include <LClient.h>
#include <LSeat.h>
#include <LPopupRole.h>

#include <xdg-shell.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/eventfd.h>

#include <LToplevelRole.h>
#include <LCursor.h>
#include <LSubsurfaceRole.h>
#include <LTime.h>
#include <LCursorRole.h>
#include <LPointer.h>

using namespace Louvre;

void Globals::Surface::resource_destroy(wl_resource *resource)
{
    printf("SURFACE DESTROYED.\n");

    // Get surface
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    // Clear keyboard focus
    if(surface->seat()->p_keyboardFocusSurface == surface)
        surface->seat()->p_keyboardFocusSurface = nullptr;

    // Clear pointer focus
    if(surface->seat()->pointer()->p_pointerFocusSurface == surface)
        surface->seat()->pointer()->p_pointerFocusSurface = nullptr;

    // Clear dragging surface
    if(surface->seat()->pointer()->p_draggingSurface == surface)
        surface->seat()->pointer()->p_draggingSurface = nullptr;

    // Clear active toplevel focus
    if(surface->seat()->p_activeTopLevel == surface->toplevel())
        surface->seat()->p_activeTopLevel = nullptr;

    // Clear moving toplevel
    if(surface->seat()->pointer()->p_movingTopLevel == surface->toplevel())
        surface->seat()->pointer()->p_movingTopLevel = nullptr;

    // Clear resizing toplevel
    if(surface->seat()->pointer()->p_resizingToplevel == surface->toplevel())
        surface->seat()->pointer()->p_resizingToplevel = nullptr;

    if(surface->roleType() == LSurface::Cursor)
    {
        LCursorRole *lCursor = surface->cursor();
        surface->compositor()->destroyCursorRequest(lCursor);
        delete lCursor;
    }

    // Parent
    if(surface->parent())
        surface->parent()->p_children.remove(surface);

    if(surface->toplevel())
        surface->toplevel()->p_surface = nullptr;
    else if(surface->popup())
        surface->popup()->p_surface = nullptr;


    surface->p_role = nullptr;

    // Remove surface from its client list
    surface->client()->p_surfaces.remove(surface);

    // Remove the surface from the compositor list
    surface->compositor()->p_surfaces.remove(surface);

    // Notify from client
    surface->compositor()->destroySurfaceRequest(surface);

    delete surface;
}

// SURFACE
void Globals::Surface::attach(wl_client *, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);
    lSurface->pending.buffer = buffer;

    if(lSurface->cursor())
        lSurface->cursor()->p_pendingHotspotOffset = LPoint(x,y);

}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    if(surface->p_frameCallback)
    {
        wl_callback_send_done(surface->p_frameCallback,LTime::ms());
        wl_resource_destroy(surface->p_frameCallback);
    }

    surface->p_frameCallback = wl_resource_create(client, &wl_callback_interface, version, callback);
}

void Globals::Surface::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Globals::Surface::commit(wl_client *, wl_resource *resource)
{
    /* Client tells the server that the current buffer is ready to be drawn.
     * (this means that the current buffer already contains all the damages and transformations) */

    // Get surface reference
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    apply_commit(surface);
}

void Globals::Surface::apply_commit(LSurface *surface)
{
    // Wait for parent commit if is subsurface in sync mode
    if(surface->roleType() == LSurface::Subsurface && surface->subsurface()->isSynced())
    {
        if(!surface->subsurface()->p_parentIsCommiting)
            return;
        else
            surface->subsurface()->p_parentIsCommiting = false;
    }

    /*
     *     if(surface->roleType() == LSurface::Subsurface)
    {
        if(surface->subsurface()->isSynced())
        {
            if(!surface->subsurface()->p_parentIsCommiting)
                return;
            else
                surface->subsurface()->p_parentIsCommiting = false;
        }
        else
        {
            if(!surface->subsurface()->p_parentIsCommiting)
                return;
        }
    }
*/

    // Makes the pending buffer the current buffer
    surface->current.buffer = surface->pending.buffer;
    surface->pending.buffer = nullptr;

    // If the buffer is empty
    if(!surface->current.buffer)
    {
        if(surface->pending.type == LSurface::Toplevel)
        {
            surface->p_role->p_roleId = surface->pending.type;
            surface->pending.type = LSurface::Undefined;
            surface->toplevel()->configureRequest();
            surface->typeChangeRequest();
        }
        else if(surface->pending.type == LSurface::Popup)
        {
            surface->p_role->p_roleId = surface->pending.type;
            surface->pending.type = LSurface::Undefined;
            surface->popup()->configureRequest();
            surface->typeChangeRequest();
        }
        else if(surface->pending.type == LSurface::Subsurface)
        {

        }
        return;
    }

    if(!surface->current.buffer->client)
        return;


    // Copy pending damages to current damages
    surface->texture()->damages.clear();
    std::copy(surface->texture()->pendingDamages.begin(),
              surface->texture()->pendingDamages.end(),
              std::back_inserter(surface->texture()->damages));
    surface->texture()->pendingDamages.clear();

    surface->p_isDamaged = true;

    // Convert the buffer to OpenGL texture
    surface->applyDamages();

    // Notify that the cursor changed content
    if(surface->cursor())
    {
        surface->cursor()->p_hotspot -= surface->cursor()->p_pendingHotspotOffset/surface->bufferScale();
        surface->seat()->pointer()->setCursorRequest(surface,surface->cursor()->hotspot().x(),surface->cursor()->hotspot().y());
    }

    /************************************
     *********** SURFACE SIZE ***********
     ************************************/
    surface->pending.size = surface->texture()->size()/surface->bufferScale();
    if(surface->current.size != surface->pending.size)
    {
        surface->current.size = surface->pending.size;

        surface->texture()->damages.clear();
        surface->texture()->damages.push_back(LRect(LPoint(),surface->texture()->size()));
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
    if(surface->roleType() == LSurface::Toplevel)
    {
        LToplevelRole *lToplevel = surface->toplevel();

        if(lToplevel->p_pendingConf.set)
        {
            UChar8 prevState = lToplevel->p_stateFlags;
            lToplevel->p_stateFlags = lToplevel->p_sentConf.flags;

            if((prevState & LToplevelRole::Maximized) != (lToplevel->p_sentConf.flags & LToplevelRole::Maximized))
                lToplevel->maximizeChanged();
            if((prevState & LToplevelRole::Fullscreen) != (lToplevel->p_sentConf.flags & LToplevelRole::Fullscreen))
                lToplevel->fullscreenChanged();

            if(lToplevel->p_sentConf.flags & LToplevelRole::Activated)
            {
                if(lToplevel->seat()->activeTopLevel() && lToplevel->seat()->activeTopLevel() != lToplevel)
                    lToplevel->seat()->activeTopLevel()->configure(lToplevel->seat()->activeTopLevel()->p_currentConf.flags & ~LToplevelRole::Activated);

                lToplevel->seat()->p_activeTopLevel = lToplevel;
            }

            lToplevel->p_pendingConf.set = false;
        }
    }




    // Apply cached commit to subsurfaces
    for(LSurface *s : surface->p_children)
    {
        if(s->roleType() == LSurface::Subsurface && s->subsurface()->isSynced())
        {
            s->subsurface()->p_parentIsCommiting = true;
            apply_commit(s);
        }

        /*
         *         if(s->roleType() == LSurface::Subsurface)
        {
            if(s->subsurface()->isSynced())
            {
                s->subsurface()->p_parentIsCommiting = true;
                apply_commit(s);
            }
            else
            {
                if(!s->subsurface()->p_parentIsCommiting)
                {
                    s->subsurface()->p_parentIsCommiting = true;
                    apply_commit(s);
                }
            }
        }*/
    }


    // FALTA ENVIAR EVENTO
    surface->compositor()->repaintAllOutputs();
}


void Globals::Surface::damage(wl_client *client, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    (void)client;

    /* The client tells the server that has updated a region of the current buffer */
    //printf("DAMAGE (%i,%i,%i,%i)\n",x,y,width,height);
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
    //printf("BUFFER DAMAGE (%i,%i,%i,%i)\n",x,y,width,height);
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->texture()->pendingDamages.push_back(LRect(x, y, width, height));
}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->setBufferScale(scale);
}




void Globals::Surface::offset(wl_client *, wl_resource *resource, Int32 x, Int32 y)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(lSurface->cursor())
        lSurface->cursor()->p_pendingHotspotOffset = LPoint(x,y);
}


