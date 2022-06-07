#include "LSubsurfaceRole.h"

using namespace Louvre;

LSubsurfaceRole::LSubsurfaceRole(wl_resource *resource, LSurface *surface)
{
    p_resource = resource;
    p_surface = surface;
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

wl_resource *LSubsurfaceRole::resource() const
{
    return p_resource;
}
