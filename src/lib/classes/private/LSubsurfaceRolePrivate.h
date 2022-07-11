#ifndef LSUBSURFACEROLEPRIVATE_H
#define LSUBSURFACEROLEPRIVATE_H

#include <LSubsurfaceRole.h>

class Louvre::LSubsurfaceRole::LSubsurfaceRolePrivate
{
public:
    LSubsurfaceRolePrivate() = default;
    ~LSubsurfaceRolePrivate() = default;

    // Indicates if the commit is called from the parent
    bool parentIsCommiting = false;

    // Indicates the subsurface mode
    bool isSynced = true;

    // Local pos
    LPoint localPos,pendingLocalPos;
    bool hasPendingLocalPos = true;

    // Pending reordering
    LSurface *pendingPlaceAbove = nullptr;
    LSurface *pendingPlaceBelow = nullptr;

};

#endif // LSUBSURFACEROLEPRIVATE_H
