#include "LSubsurfaceRole.h"
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LSubsurfaceRole::LSubsurfaceRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource,surface)
{
    p_roleId = LSurface::Subsurface;
}

LSubsurfaceRole::~LSubsurfaceRole()
{

}

const LPoint &LSubsurfaceRole::rolePos() const
{
    p_rolePos = p_localPos + surface()->parent()->pos(true);
    return p_rolePos;
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


