#ifndef LSUBSURFACEROLE_H
#define LSUBSURFACEROLE_H

#include <LNamespaces.h>
#include <LPoint.h>

class Louvre::LSubsurfaceRole
{
public:
    LSubsurfaceRole(wl_resource *resource,LSurface *surface);
    virtual ~LSubsurfaceRole();

    virtual void localPosChangedRequest();
    virtual void syncModeChangedRequest();

    bool isSynced() const;
    const LPoint &localPos() const;
    LSurface *surface() const;
    LCompositor *compositor() const;
    LSeat *seat() const;
    wl_resource *resource() const;

private:
    friend class Globals::Surface;
    friend class Globals::Subsurface;

    bool p_parentIsCommiting = false;

    bool p_isSynced = true;
    LPoint p_localPos;
    wl_resource *p_resource = nullptr;
    LSurface *p_surface = nullptr;
    LCompositor *p_compositor = nullptr;
};

#endif // LSUBSURFACEROLE_H
