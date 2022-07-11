#include <LBaseSurfaceRolePrivate.h>
#include <LSubsurfaceRolePrivate.h>
#include <LSurface.h>
#include <LCompositor.h>

using namespace Louvre;

LSubsurfaceRole::LSubsurfaceRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource,surface)
{
    baseImp()->roleId = LSurface::Subsurface;
    m_imp = new LSubsurfaceRolePrivate();
}

LSubsurfaceRole::~LSubsurfaceRole()
{
    delete m_imp;
}

const LPoint &LSubsurfaceRole::rolePos() const
{
    baseImp()->rolePos = m_imp->localPos + surface()->parent()->pos(true);
    return baseImp()->rolePos;
}

void LSubsurfaceRole::localPosChangedRequest()
{

}

void LSubsurfaceRole::syncModeChangedRequest()
{

}

void LSubsurfaceRole::placedAbove(LSurface *sibiling)
{

}

void LSubsurfaceRole::placedBelow(LSurface *sibiling)
{

}

bool LSubsurfaceRole::isSynced() const
{
    if(surface()->parent() && surface()->parent()->subsurface())
        return surface()->parent()->subsurface()->isSynced() || m_imp->isSynced;

    return m_imp->isSynced;
}

const LPoint &LSubsurfaceRole::localPos() const
{
    return m_imp->localPos;
}

LSubsurfaceRole::LSubsurfaceRolePrivate *LSubsurfaceRole::imp() const
{
    return m_imp;
}


