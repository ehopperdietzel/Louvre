#ifndef LBASESURFACEROLE_H
#define LBASESURFACEROLE_H

#include <LNamespaces.h>
#include <LPoint.h>

class Louvre::LBaseSurfaceRole
{
public:
    LBaseSurfaceRole(wl_resource *resource, LSurface *surface);
    virtual const LPoint &rolePos() const = 0;
    UInt32 roleId();
    LCompositor *compositor() const;
    LSurface *surface() const;
    LSeat *seat() const;
    wl_resource *resource() const;

protected:
    friend class Globals::Surface;
    mutable LPoint p_rolePos;
    UInt32 p_roleId = 0;
    wl_resource *p_resource = nullptr;
    LSurface *p_surface = nullptr;
    LCompositor *p_compositor = nullptr;
};

#endif // LBASESURFACEROLE_H
