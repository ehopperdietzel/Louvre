#include "LSubsurfaceRole.h"
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LSubsurfaceRole::LSubsurfaceRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource,surface)
{
    m_roleId = LSurface::Subsurface;
}

LSubsurfaceRole::~LSubsurfaceRole()
{

}

const LPoint &LSubsurfaceRole::rolePos() const
{
    m_rolePos = m_localPos + surface()->parent()->pos(true);
    return m_rolePos;
}

void LSubsurfaceRole::localPosChangedRequest()
{

}

void LSubsurfaceRole::syncModeChangedRequest()
{

}

bool LSubsurfaceRole::isSynced() const
{
    return m_isSynced;
}

const LPoint &LSubsurfaceRole::localPos() const
{
    return m_localPos;
}


