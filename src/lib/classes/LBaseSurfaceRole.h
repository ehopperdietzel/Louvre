#ifndef LBASESURFACEROLE_H
#define LBASESURFACEROLE_H

#include <LNamespaces.h>
#include <LPoint.h>

class Louvre::LBaseSurfaceRole
{
public:
    LBaseSurfaceRole(wl_resource *resource, LSurface *surface);
    virtual ~LBaseSurfaceRole();
    virtual const LPoint &rolePos() const = 0;
    UInt32 roleId();
    LCompositor *compositor() const;
    LSurface *surface() const;
    LSeat *seat() const;
    wl_resource *resource() const;

    class LBaseSurfaceRolePrivate;
    LBaseSurfaceRolePrivate *baseImp() const;

private:
    LBaseSurfaceRolePrivate *m_baseImp = nullptr;
};

#endif // LBASESURFACEROLE_H
