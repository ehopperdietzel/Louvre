#ifndef LPOSITIONERPRIVATE_H
#define LPOSITIONERPRIVATE_H

#include <LPositioner.h>

using namespace Louvre;

class LPositioner::LPositionerPrivate
{

public:
    LPositionerPrivate() = default;
    ~LPositionerPrivate() = default;

    wl_resource *resource = nullptr;
    LClient *client = nullptr;

    LSize size;
    LRect anchorRect;
    LPoint offset;

    UInt32 anchor = Anchor::NoAnchor;
    UInt32 gravity = Gravity::NoGravity;
    UInt32 constraintAdjustment = ConstraintAdjustment::NoAdjustment;

    LSurface *linkedSurface = nullptr;

#if LOUVRE_XDG_WM_BASE_VERSION >=3
    bool isReactive = false;
    LSize parentSize;
    UInt32 parentConfigureSerial;
#endif

};

#endif // LPOSITIONERPRIVATE_H
