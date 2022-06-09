#include "LSubsurfaceRole.h"
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LSubsurfaceRole::LSubsurfaceRole(wl_resource *resource, LSurface *surface)
{
    p_resource = resource;
    p_surface = surface;
    p_compositor = surface->compositor();
}

LSubsurfaceRole::~LSubsurfaceRole()
{

}

void LSubsurfaceRole::localPosChangedRequest()
{

}

void LSubsurfaceRole::syncModeChangedRequest()
{

}

bool LSubsurfaceRole::isSynced() const
{
    return p_isSynced;
}

const LPoint &LSubsurfaceRole::localPos() const
{
    return p_localPos;
}

LSurface *LSubsurfaceRole::surface() const
{
    return p_surface;
}

LCompositor *LSubsurfaceRole::compositor() const
{
    return p_compositor;
}

LSeat *LSubsurfaceRole::seat() const
{
    return compositor()->seat();
}

wl_resource *LSubsurfaceRole::resource() const
{
    return p_resource;
}
