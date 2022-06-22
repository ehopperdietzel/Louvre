#include "LBaseSurfaceRole.h"
#include <LPoint.h>
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LBaseSurfaceRole::LBaseSurfaceRole(wl_resource *resource, LSurface *surface)
{
    m_resource = resource;
    m_surface = surface;
    m_compositor = surface->compositor();
}

UInt32 LBaseSurfaceRole::roleId()
{
    return m_roleId;
}

LCompositor *LBaseSurfaceRole::compositor() const
{
    return m_compositor;
}

LSurface *LBaseSurfaceRole::surface() const
{
    return m_surface;
}

LSeat *LBaseSurfaceRole::seat() const
{
    return compositor()->seat();
}

wl_resource *LBaseSurfaceRole::resource() const
{
    return m_resource;
}
