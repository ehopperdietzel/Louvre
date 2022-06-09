#include "LBaseSurfaceRole.h"
#include <LPoint.h>
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LBaseSurfaceRole::LBaseSurfaceRole(wl_resource *resource, LSurface *surface)
{
    p_resource = resource;
    p_surface = surface;
    p_compositor = surface->compositor();
}

UInt32 LBaseSurfaceRole::roleId()
{
    return p_roleId;
}

LCompositor *LBaseSurfaceRole::compositor() const
{
    return p_compositor;
}

LSurface *LBaseSurfaceRole::surface() const
{
    return p_surface;
}

LSeat *LBaseSurfaceRole::seat() const
{
    return compositor()->seat();
}

wl_resource *LBaseSurfaceRole::resource() const
{
    return p_resource;
}
