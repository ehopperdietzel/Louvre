#ifndef LCURSORROLE_H
#define LCURSORROLE_H

#include <LBaseSurfaceRole.h>

class Louvre::LCursorRole : public LBaseSurfaceRole
{
public:
    LCursorRole(wl_resource *resource, LSurface *surface);
    virtual ~LCursorRole();
    const LPoint &rolePos() const override;
    const LPoint &hotspot() const;

private:
    friend class Globals::Pointer;
    friend class Globals::Surface;
    LPoint p_hotspot;
    LPoint p_pendingHotspotOffset;
};

#endif // LCURSORROLE_H
