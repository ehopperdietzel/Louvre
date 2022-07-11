#ifndef LBASESURFACEROLEPRIVATE_H
#define LBASESURFACEROLEPRIVATE_H

#include <LBaseSurfaceRole.h>

class Louvre::LBaseSurfaceRole::LBaseSurfaceRolePrivate
{
public:
    LBaseSurfaceRolePrivate() = default;
    ~LBaseSurfaceRolePrivate() = default;

    LPoint rolePos;
    UInt32 roleId = 0;
    wl_resource *resource = nullptr;
    LSurface *surface = nullptr;
    LCompositor *compositor = nullptr;

};

#endif // LBASESURFACEROLEPRIVATE_H
