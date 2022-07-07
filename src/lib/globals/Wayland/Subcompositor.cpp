#include "Subcompositor.h"
#include <Subsurface.h>
#include <stdio.h>

#include <LCompositor.h>
#include <LSubsurfaceRole.h>
#include <LSurfacePrivate.h>

using namespace Louvre;

struct wl_subsurface_interface subsurface_implementation =
{
    .destroy = &Globals::Subsurface::destroy,
    .set_position = &Globals::Subsurface::set_position,
    .place_above = &Globals::Subsurface::place_above,
    .place_below = &Globals::Subsurface::place_below,
    .set_sync = &Globals::Subsurface::set_sync,
    .set_desync = &Globals::Subsurface::set_desync
};

struct wl_subcompositor_interface subcompositor_implementation =
{
    .destroy = &Globals::Subcompositor::destroy,
    .get_subsurface = &Globals::Subcompositor::get_subsurface
};

void Globals::Subcompositor::resource_destroy(wl_resource *)
{
    printf("SUBCOMPOSITOR DESTROYED.\n");
}

void Globals::Subcompositor::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Globals::Subcompositor::get_subsurface(wl_client *client, wl_resource *resource, UInt32 id, wl_resource *surface, wl_resource *parent)
{
    LSurface *lSurface = (LSurface*)wl_resource_get_user_data(surface);

    if(lSurface->roleType() != LSurface::Undefined)
    {
        wl_resource_post_error(resource,WL_SUBCOMPOSITOR_ERROR_BAD_SURFACE,"Given wl_surface already has another role.");
        return;
    }

    Int32 version = wl_resource_get_version(resource);
    LCompositor *lCompositor = (LCompositor*)wl_resource_get_user_data(resource);


    wl_resource *subsurface = wl_resource_create(client, &wl_subsurface_interface, version, id);
    LSubsurfaceRole *lSubsurface = lCompositor->createSubsurfaceRequest(subsurface, lSurface);
    lSurface->imp()->m_role = lSubsurface;

    LSurface *lParent = (LSurface*)wl_resource_get_user_data(parent);
    lCompositor->insertSurfaceAfter(lParent,lSurface);
    lParent->imp()->m_children.push_back(lSurface);
    lSurface->imp()->m_parent = lParent;

    wl_resource_set_implementation(subsurface, &subsurface_implementation, lSubsurface, &Subsurface::resource_destroy);

    lSurface->imp()->current.type = LSurface::Subsurface;
    lSurface->imp()->pending.type = LSurface::Undefined;
    lSurface->roleChanged();
    lSurface->parentChanged();

    printf("SUBSURFACE CREATED.\n");
}

void Globals::Subcompositor::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    LCompositor *lCompositor = (LCompositor*)data;
    wl_resource *subcompositor = wl_resource_create(client, &wl_subcompositor_interface, version, id);
    wl_resource_set_implementation(subcompositor, &subcompositor_implementation, lCompositor, &Subcompositor::resource_destroy);
}
