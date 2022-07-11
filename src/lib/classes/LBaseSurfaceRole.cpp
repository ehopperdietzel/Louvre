#include <LBaseSurfaceRolePrivate.h>
#include <LPoint.h>
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LBaseSurfaceRole::LBaseSurfaceRole(wl_resource *resource, LSurface *surface)
{
    m_baseImp = new LBaseSurfaceRolePrivate();
    m_baseImp->resource = resource;
    m_baseImp->surface = surface;
    m_baseImp->compositor = surface->compositor();
}

LBaseSurfaceRole::~LBaseSurfaceRole()
{
    delete m_baseImp;
}

UInt32 LBaseSurfaceRole::roleId()
{
    return m_baseImp->roleId;
}

LCompositor *LBaseSurfaceRole::compositor() const
{
    return m_baseImp->compositor;
}

LSurface *LBaseSurfaceRole::surface() const
{
    return m_baseImp->surface;
}

LSeat *LBaseSurfaceRole::seat() const
{
    return compositor()->seat();
}

wl_resource *LBaseSurfaceRole::resource() const
{
    return m_baseImp->resource;
}

LBaseSurfaceRole::LBaseSurfaceRolePrivate *LBaseSurfaceRole::baseImp() const
{
    return m_baseImp;
}
