#ifndef LSUBSURFACEROLE_H
#define LSUBSURFACEROLE_H

#include <LBaseSurfaceRole.h>
#include <LPoint.h>

class Louvre::LSubsurfaceRole : public LBaseSurfaceRole
{
public:
    LSubsurfaceRole(wl_resource *resource,LSurface *surface);
    virtual ~LSubsurfaceRole();

    const LPoint &rolePos() const override;

    virtual void localPosChangedRequest();
    virtual void syncModeChangedRequest();

    bool isSynced() const;
    const LPoint &localPos() const;

private:
    friend class Globals::Surface;
    friend class Globals::Subsurface;

    bool m_parentIsCommiting = false;
    bool m_isSynced = true;
    LPoint m_localPos;

};

#endif // LSUBSURFACEROLE_H
