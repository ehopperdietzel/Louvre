#ifndef LPOSITIONER_H
#define LPOSITIONER_H

#include <LNamespaces.h>
#include <LRect.h>

class Louvre::LPositioner
{
public:
    LPositioner(LClient *client);
    ~LPositioner();

    struct Anchor
    {
        enum : UInt32
        {
            NoAnchor    = 0,
            Top         = 1,
            Bottom      = 2,
            Left        = 3,
            Right       = 4,
            TopLeft     = 5,
            BottomLeft  = 6,
            TopRight    = 7,
            BottomRight = 8
        };
    };

    struct Gravity
    {
        enum : UInt32
        {
            NoGravity   = 0,
            Top         = 1,
            Bottom      = 2,
            Left        = 3,
            Right       = 4,
            TopLeft     = 5,
            BottomLeft  = 6,
            TopRight    = 7,
            BottomRight = 8
        };
    };

    struct ConstraintAdjustment
    {
        enum : UInt32
        {
            NoAdjustment    = 0,
            SlideX          = 1,
            SlideY          = 2,
            FlipX           = 4,
            FlipY           = 8,
            ResizeX         = 16,
            ResizeY         = 32
        };
    };

    LClient *client() const;
    wl_resource *resource() const;

    const LSize &size() const;
    const LRect &anchorRect() const;
    UInt32 anchor() const;
    UInt32 gravity() const;
    const LPoint &offset() const;

#if LOUVRE_XDG_WM_BASE_VERSION >=3
    bool isReactive() const;
    const LSize &parentSize() const;
    UInt32 parentConfigureSerial() const;
#endif

    LPoint calculatePopupPosition(const LRect &outputRect, const LPoint &parentPosition);

    UInt32 constraintAdjustment() const;

    class LPositionerPrivate;
    LPositionerPrivate *imp() const;
private:

    LPositionerPrivate *m_imp = nullptr;



};

#endif // LPOSITIONER_H
