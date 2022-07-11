#include "Surface.h"

#include <LClientPrivate.h>
#include <LCompositorPrivate.h>
#include <LSurfacePrivate.h>
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
#include <LSubsurfaceRolePrivate.h>
#include <LBaseSurfaceRolePrivate.h>
#include <LTime.h>
#include <LCursorRole.h>
#include <LPointer.h>
#include <LKeyboard.h>

using namespace Louvre;

void Globals::Surface::resource_destroy(wl_resource *resource)
{
    printf("SURFACE DESTROYED.\n");

    // Get surface
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);

    // Clear keyboard focus
    if(surface->seat()->keyboard()->focusSurface() == surface)
        surface->seat()->keyboard()->m_keyboardFocusSurface = nullptr;

    // Clear pointer focus
    if(surface->seat()->pointer()->m_pointerFocusSurface == surface)
        surface->seat()->pointer()->m_pointerFocusSurface = nullptr;

    // Clear dragging surface
    if(surface->seat()->pointer()->m_draggingSurface == surface)
        surface->seat()->pointer()->m_draggingSurface = nullptr;

    // Clear active toplevel focus
    if(surface->seat()->m_activeTopLevel == surface->toplevel())
        surface->seat()->m_activeTopLevel = nullptr;

    // Clear moving toplevel
    if(surface->seat()->pointer()->m_movingTopLevel == surface->toplevel())
        surface->seat()->pointer()->m_movingTopLevel = nullptr;

    // Clear resizing toplevel
    if(surface->seat()->pointer()->m_resizingToplevel == surface->toplevel())
        surface->seat()->pointer()->m_resizingToplevel = nullptr;

    if(surface->roleType() == LSurface::Cursor)
    {
        LCursorRole *lCursor = surface->cursor();
        surface->compositor()->destroyCursorRequest(lCursor);
        delete lCursor;
    }

    for(LSurface *child : surface->children())
    {
        child->imp()->m_parent = nullptr;
        child->parentChanged();
    }

    for(LSurface *child : surface->imp()->pendingChildren)
    {
        child->imp()->pendingParent = nullptr;
    }

    // Parent
    if(surface->parent())
        surface->parent()->imp()->m_children.remove(surface);

    if(surface->imp()->pendingParent)
        surface->imp()->pendingParent->imp()->pendingChildren.remove(surface);


    if(surface->role())
        surface->role()->baseImp()->surface = nullptr;

    // surface->imp()->m_role = nullptr;

    // Remove surface from its client list
    surface->client()->imp()->m_surfaces.remove(surface);

    // Remove the surface from the compositor list
    surface->compositor()->imp()->m_surfaces.remove(surface);

    // Notify from client
    surface->compositor()->destroySurfaceRequest(surface);

    delete surface;
}

// SURFACE
void Globals::Surface::attach(wl_client *, wl_resource *resource, wl_resource *buffer, Int32 x, Int32 y)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);
    lSurface->imp()->pending.buffer = buffer;
    lSurface->imp()->attachedBuffer = true;

    if(lSurface->cursor())
        lSurface->cursor()->m_pendingHotspotOffset = LPoint(x,y);

}

void Globals::Surface::frame(wl_client *client, wl_resource *resource, UInt32 callback)
{
    /* Client waits for this frame to be marked as done before creating next frame*/

    Int32 version = wl_resource_get_version(resource);
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);


    if(surface->imp()->m_frameCallback)
    {
        wl_resource *dumb_callback = wl_resource_create(client, &wl_callback_interface, version, callback);
        wl_callback_send_done(dumb_callback,LTime::ms());
        wl_resource_destroy(dumb_callback);
        return;
    }


    surface->imp()->m_frameCallback = wl_resource_create(client, &wl_callback_interface, version, callback);
}

void Globals::Surface::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void insertSurfaceAfter(list<LSurface*>&surfaces,LSurface *prevSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = surfaces.begin(); it != surfaces.end(); it++)
    {
        if((*it) == prevSurface)
        {
            surfaces.remove(surfaceToInsert);

            if(it++ == surfaces.end())
                surfaces.push_back(surfaceToInsert);
            else
                surfaces.insert((it++),surfaceToInsert);

        }
    }
}

void insertSurfaceBefore(list<LSurface*>&surfaces,LSurface *nextSurface, LSurface *surfaceToInsert)
{
    for(list<LSurface*>::iterator it = surfaces.begin(); it != surfaces.end(); ++it)
    {
        if((*it) == nextSurface)
        {
            surfaces.remove(surfaceToInsert);
            surfaces.insert(it,surfaceToInsert);
            return;
        }
    }
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
    // Adds pending children
    while(!surface->imp()->pendingChildren.empty())
    {
        LSurface *child = surface->imp()->pendingChildren.front();
        surface->imp()->pendingChildren.pop_front();

        if(surface->children().empty())
            surface->compositor()->insertSurfaceAfter(surface,child);
        else
            surface->compositor()->insertSurfaceAfter(surface->children().back(),child);

        surface->imp()->m_children.push_back(child);
        child->imp()->m_parent = surface;
        child->parentChanged();
    }

    // Update children subsurfaces positions and order
    for(LSurface *child : surface->children())
    {
        if(child->subsurface())
        {
            if(child->subsurface()->imp()->hasPendingLocalPos)
            {
                child->subsurface()->imp()->hasPendingLocalPos = false;
                child->subsurface()->imp()->localPos = child->subsurface()->imp()->pendingLocalPos;
                child->subsurface()->localPosChangedRequest();
            }

            if(child->subsurface()->imp()->pendingPlaceAbove)
            {
                child->compositor()->insertSurfaceAfter(child->subsurface()->imp()->pendingPlaceAbove,child);
                insertSurfaceAfter(surface->imp()->m_children,child->subsurface()->imp()->pendingPlaceAbove,child);
                child->subsurface()->placedAbove(child->subsurface()->imp()->pendingPlaceAbove);
                child->subsurface()->imp()->pendingPlaceAbove = nullptr;
            }

            if(child->subsurface()->imp()->pendingPlaceBelow)
            {
                child->compositor()->insertSurfaceBefore(child->subsurface()->imp()->pendingPlaceBelow,child);
                insertSurfaceBefore(surface->imp()->m_children,child->subsurface()->imp()->pendingPlaceBelow,child);
                child->subsurface()->placedBelow(child->subsurface()->imp()->pendingPlaceBelow);
                child->subsurface()->imp()->pendingPlaceBelow = nullptr;
            }
        }
    }

    // If no buffer was attached
    if(!surface->imp()->attachedBuffer)
    {
        if(surface->imp()->pending.type == LSurface::Toplevel)
        {
            surface->imp()->m_role->baseImp()->roleId = surface->imp()->pending.type;
            surface->imp()->pending.type = LSurface::Undefined;
            surface->toplevel()->configureRequest();
        }
        else if(surface->imp()->pending.type == LSurface::Popup)
        {
            surface->imp()->m_role->baseImp()->roleId = surface->imp()->pending.type;
            surface->imp()->pending.type = LSurface::Undefined;
            surface->popup()->configureRequest();
        }
        return;
    }

    // Makes the pending buffer the current buffer
    surface->imp()->current.buffer = surface->imp()->pending.buffer;
    surface->imp()->pending.buffer = nullptr;

    // Mark as no buffer attached again
    surface->imp()->attachedBuffer = false;

    // If the buffer is empty
    if(!surface->imp()->current.buffer)
    {
        // If the surface was mapped
        if(surface->mapped())
        {
            surface->imp()->mapped = false;

            // Notify mapping state gange
            surface->mappingChanged();

        }
        return;
    }
    else
    {
        // If the surface was not mapped
        if(!surface->mapped())
        {
            surface->imp()->mapped = true;

            // Notify mapping state gange
            surface->mappingChanged();
        }
    }

    // Subsurface commit called from the parent commit if synced
    if(surface->subsurface() && surface->subsurface()->isSynced())
    {
        if(surface->subsurface()->imp()->parentIsCommiting)
            surface->subsurface()->imp()->parentIsCommiting = false;
        else
            return;
    }

    // Apply cached commit to subsurfaces
    for(LSurface *s : surface->children())
    {
        if(s->subsurface() && s->subsurface()->isSynced())
        {
            s->subsurface()->imp()->parentIsCommiting = true;
            apply_commit(s);
        }
    }

    surface->imp()->m_isDamaged = true;

    // Convert the buffer to OpenGL texture
    surface->imp()->m_bufferToTexture();

    // Notify that the cursor changed content
    if(surface->cursor() && surface->compositor()->cursor()->texture() == surface->texture())
    {
        surface->cursor()->m_hotspot -= surface->cursor()->m_pendingHotspotOffset/surface->bufferScale();
        surface->seat()->pointer()->setCursorRequest(surface,surface->cursor()->hotspot().x(),surface->cursor()->hotspot().y());
    }

    /************************************
     *********** SURFACE SIZE ***********
     ************************************/
    surface->imp()->pending.size = surface->texture()->size()/surface->bufferScale();
    if(surface->imp()->current.size != surface->imp()->pending.size)
    {
        surface->imp()->m_inputRegionChanged = true;
        surface->imp()->current.size = surface->imp()->pending.size;

        //surface->texture()->damages.clear();
        //surface->texture()->damages.push_back(LRect(LPoint(),surface->texture()->size()));
        surface->bufferSizeChanged();
    }

    /************************************
     *********** DAMAGES ***********
     ************************************/
    if(surface->imp()->m_damagesChanged)
    {
        if(!surface->imp()->m_textureChanged)
            surface->imp()->m_currentDamages.clear();

        for(LRect &rect : surface->imp()->pending.damages)
            surface->imp()->m_currentDamages.addRect(rect);
        for(LRect &rect : surface->imp()->pending.bufferDamages)
            surface->imp()->m_currentDamages.addRect(rect/surface->bufferScale());

        surface->imp()->pending.damages.clear();
        surface->imp()->pending.bufferDamages.clear();
        surface->imp()->m_currentDamages.clip(LRect(LPoint(),surface->size()));
        surface->imp()->m_damagesChanged = false;
    }

    /************************************
     *********** INPUT REGION ***********
     ************************************/
    if(surface->imp()->m_inputRegionChanged)
    {
        if(surface->imp()->pending.inputRegion.m_rects.empty())
        {
            surface->imp()->current.inputRegion.clear();
            surface->imp()->current.inputRegion.addRect(LRect(LPoint(),surface->size()));
        }
        else
        {
            surface->imp()->current.inputRegion.copy(surface->imp()->pending.inputRegion);
            surface->imp()->current.inputRegion.clip(LRect(LPoint(),surface->size()));
        }

        surface->inputRegionChanged();
        surface->imp()->m_inputRegionChanged = false;
    }

    /************************************
     ********** OPAQUE REGION ***********
     ************************************/
    if(surface->imp()->m_opaqueRegionChanged)
    {
        surface->imp()->current.opaqueRegion = surface->imp()->pending.opaqueRegion;
        surface->imp()->current.opaqueRegion.clip(LRect(LPoint(),surface->size()));
        surface->imp()->m_opaqueRegionChanged = false;
        surface->imp()->pending.opaqueRegion.clear();
    }

    /*****************************************
     ********** TRANSLUCENT REGION ***********
     *****************************************/
    surface->imp()->m_translucentRegion.clear();
    surface->imp()->m_translucentRegion.addRect(LRect(LPoint(),surface->size()));
    for(const LRect &r : surface->opaqueRegion().rects())
        surface->imp()->m_translucentRegion.subtractRect(r);


    if(!surface->imp()->m_roleChangeNotified)
    {
        surface->imp()->m_roleChangeNotified = true;
        surface->roleChanged();
    }

    // Toplevel configure
    if(surface->roleType() == LSurface::Toplevel)
    {
        LToplevelRole *lToplevel = surface->toplevel();

        if(lToplevel->m_pendingConf.set)
        {
            UChar8 prevState = lToplevel->m_stateFlags;
            lToplevel->m_stateFlags = lToplevel->m_sentConf.flags;

            if((prevState & LToplevelRole::Maximized) != (lToplevel->m_sentConf.flags & LToplevelRole::Maximized))
                lToplevel->maximizeChanged();
            if((prevState & LToplevelRole::Fullscreen) != (lToplevel->m_sentConf.flags & LToplevelRole::Fullscreen))
                lToplevel->fullscreenChanged();

            if(lToplevel->m_sentConf.flags & LToplevelRole::Activated)
            {
                if(lToplevel->seat()->activeTopLevel() && lToplevel->seat()->activeTopLevel() != lToplevel)
                    lToplevel->seat()->activeTopLevel()->configure(lToplevel->seat()->activeTopLevel()->m_currentConf.flags & ~LToplevelRole::Activated);

                lToplevel->seat()->m_activeTopLevel = lToplevel;
            }

            if((prevState & LToplevelRole::Activated) != (lToplevel->m_sentConf.flags & LToplevelRole::Activated))
                lToplevel->activatedChanged();

            lToplevel->m_pendingConf.set = false;
        }
    }



    // FALTA ENVIAR EVENTO
    if(!surface->imp()->m_textureChanged)
    {
        surface->imp()->m_textureChanged = true;
        surface->compositor()->repaintAllOutputs();
    }
}


void Globals::Surface::damage(wl_client *, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);
    lSurface->imp()->pending.damages.push_back(LRect(x, y, width, height));
    //surface->texture()->pendingDamages.push_back(LRect(x, y, width, height)*surface->bufferScale());
    lSurface->imp()->m_damagesChanged = true;
}

void Globals::Surface::damage_buffer(wl_client *, wl_resource *resource, Int32 x, Int32 y, Int32 width, Int32 height)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);
    lSurface->imp()->pending.bufferDamages.push_back(LRect(x, y, width, height));
    //surface->texture()->pendingDamages.push_back(LRect(x, y, width, height));
    lSurface->imp()->m_damagesChanged = true;
}

void Globals::Surface::set_opaque_region(wl_client *, wl_resource *resource, wl_resource *region)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(region)
    {
        LRegion *lRegion = (LRegion*)wl_resource_get_user_data(region);
        lSurface->imp()->pending.opaqueRegion.copy(*lRegion);
    }
    else
        lSurface->imp()->pending.opaqueRegion.clear();

    lSurface->imp()->m_opaqueRegionChanged = true;
}

void Globals::Surface::set_input_region(wl_client *, wl_resource *resource, wl_resource *region)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(region == NULL)
        lSurface->imp()->pending.inputRegion.clear();
    else
    {
        LRegion *lRegion = (LRegion*)wl_resource_get_user_data(region);
        lSurface->imp()->pending.inputRegion.copy(*lRegion);
    }

    lSurface->imp()->m_inputRegionChanged = true;
}

void Globals::Surface::set_buffer_transform(wl_client *client, wl_resource *resource, Int32 transform)
{
    (void)client;(void)resource;(void)transform;
    printf("set_buffer_transform: transform %i\n",transform);
}

void Globals::Surface::set_buffer_scale(wl_client *client, wl_resource *resource, Int32 scale)
{
    (void)client;
    LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    surface->imp()->m_bufferScale = scale;
}

void Globals::Surface::offset(wl_client *, wl_resource *resource, Int32 x, Int32 y)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(resource);

    if(lSurface->cursor())
        lSurface->cursor()->m_pendingHotspotOffset = LPoint(x,y);
}


