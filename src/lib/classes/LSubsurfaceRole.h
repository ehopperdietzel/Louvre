#ifndef LSUBSURFACEROLE_H
#define LSUBSURFACEROLE_H

#include <LBaseSurfaceRole.h>

class Louvre::LSubsurfaceRole : public LBaseSurfaceRole
{
public:
    LSubsurfaceRole(wl_resource *resource,LSurface *surface);
    virtual ~LSubsurfaceRole();

    const LPoint &rolePos() const override;

    virtual void localPosChangedRequest();
    virtual void syncModeChangedRequest();
    virtual void placedAbove(LSurface *sibiling);
    virtual void placedBelow(LSurface *sibiling);

    bool isSynced() const;
    const LPoint &localPos() const;

    class LSubsurfaceRolePrivate;
    LSubsurfaceRolePrivate *imp() const;

private:
    LSubsurfaceRolePrivate *m_imp = nullptr;

};

#endif // LSUBSURFACEROLE_H
