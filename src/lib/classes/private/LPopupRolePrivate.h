#ifndef LPOPUPROLEPRIVATE_H
#define LPOPUPROLEPRIVATE_H

#include <LPopupRole.h>

using namespace Louvre;

class LPopupRole::LPopupRolePrivate
{
public:
    LPopupRolePrivate() = default;
    ~LPopupRolePrivate() = default;

    LRect windowGeometry;

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
    UInt32 repositionSerial = 0;
#endif

    LPositioner *positioner = nullptr;

    bool dismissed = false;

};

#endif // LPOPUPROLEPRIVATE_H
