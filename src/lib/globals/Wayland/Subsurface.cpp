#include "Subsurface.h"
#include <LSubsurfaceRole.h>
#include <LSurfacePrivate.h>
#include <LCompositor.h>

void Louvre::Globals::Subsurface::resource_destroy(wl_resource *resource)
{
    printf("SUBSURFACE DESTROYED.\n");

    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);

    if(lSubsurface->surface())
    {
        // Parent
        if(lSubsurface->surface()->parent())
        {
            lSubsurface->surface()->parent()->imp()->m_children.remove(lSubsurface->surface());
            lSubsurface->surface()->imp()->m_parent = nullptr;
            lSubsurface->surface()->parentChangeRequest();
        }

        lSubsurface->surface()->imp()->m_role = nullptr;
        lSubsurface->surface()->imp()->current.type = LSurface::Undefined;
        lSubsurface->surface()->typeChangeRequest();
    }

    delete lSubsurface;
}

void Louvre::Globals::Subsurface::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Louvre::Globals::Subsurface::set_position(wl_client *, wl_resource *resource, Int32 x, Int32 y)
{
    printf("SET SUBSURFACE POSITION.\n");
    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);
    lSubsurface->m_localPos = LPoint(x,y);
    lSubsurface->localPosChangedRequest();
}

void Louvre::Globals::Subsurface::place_above(wl_client *, wl_resource *resource, wl_resource *sibiling)
{
    printf("PLACE SUBSURFACE ABOVE.\n");
    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);
    LSurface *prevSurface = (LSurface*)wl_resource_get_user_data(sibiling);
    lSubsurface->compositor()->insertSurfaceAfter(prevSurface,lSubsurface->surface());
}

void Louvre::Globals::Subsurface::place_below(wl_client *, wl_resource *resource, wl_resource *sibiling)
{
    printf("PLACE SUBSURFACE BELOW.\n");
    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);
    LSurface *nextSurface = (LSurface*)wl_resource_get_user_data(sibiling);
    lSubsurface->compositor()->insertSurfaceBefore(nextSurface,lSubsurface->surface());
}

void Louvre::Globals::Subsurface::set_sync(wl_client *, wl_resource *resource)
{
    printf("SET SUBSURFACE SYNC.\n");
    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);
    lSubsurface->m_isSynced = true;
    sync_all_children_surfaces(lSubsurface->surface());
    lSubsurface->syncModeChangedRequest();
}

void Louvre::Globals::Subsurface::set_desync(wl_client *, wl_resource *resource)
{
    printf("SET SUBSURFACE DESYNC.\n");
    LSubsurfaceRole *lSubsurface = (LSubsurfaceRole*)wl_resource_get_user_data(resource);
    lSubsurface->m_isSynced = false;
    lSubsurface->syncModeChangedRequest();
}

void Louvre::Globals::Subsurface::sync_all_children_surfaces(LSurface *surface)
{
    for(LSurface *s : surface->children())
    {
        if(s->roleType() == LSurface::Subsurface)
        {
            s->subsurface()->m_isSynced = true;
            s->subsurface()->syncModeChangedRequest();
            sync_all_children_surfaces(s);
        }
    }
}

