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

    bool p_parentIsCommiting = false;
    bool p_isSynced = true;
    LPoint p_localPos;

};

#endif // LSUBSURFACEROLE_H
