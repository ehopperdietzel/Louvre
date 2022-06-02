#ifndef LPOSITIONER_H
#define LPOSITIONER_H

#include <LNamespaces.h>
#include <LRect.h>

class Louvre::LPositioner
{
public:
    LPositioner(LClient *client);

    enum class LAnchor : UChar8
    {
        LNone        = 0,
        LTop         = 1,
        LBottom      = 2,
        LLeft        = 3,
        LRight       = 4,
        LTopLeft     = 5,
        LBottomLeft  = 6,
        LTopRight    = 7,
        LBottomRight = 8
    };

    enum class LGravity : UChar8
    {
        LNone        = 0,
        LTop         = 1,
        LBottom      = 2,
        LLeft        = 3,
        LRight       = 4,
        LTopLeft     = 5,
        LBottomLeft  = 6,
        LTopRight    = 7,
        LBottomRight = 8
    };

    enum class LConstraintAdjustment : UChar8
    {
        LNone    = 0,
        LSlideX  = 1,
        LSlideY  = 2,
        LFlipX   = 4,
        LFlipY   = 8,
        LResizeX = 16,
        LResizeY = 32
    };

    LClient *client() const;
    wl_resource *resource() const;

    const LSize &size() const;
    const LRect &anchorRect() const;
    LAnchor anchor() const;
    LGravity gravity() const;
    const LPoint &offset() const;

#if LOUVRE_XDG_WM_BASE_VERSION >=3
    bool isReactive() const;
    const LSize &parentSize() const;
    UInt32 parentConfigureSerial() const;
#endif

    LPoint calculatePopupPosition(const LRect &outputRect, const LPoint &parentPosition);

    LConstraintAdjustment constraintAdjustment() const;

private:
    friend class Louvre::Extensions::XdgShell::WmBase;
    friend class Louvre::Extensions::XdgShell::Popup;
    friend class Louvre::Extensions::XdgShell::Positioner;

    wl_resource *p_resource = nullptr;
    LClient *p_client = nullptr;

    LSize p_size;
    LRect p_anchorRect;
    LPoint p_offset;

    LAnchor p_anchor = LAnchor::LNone;
    LGravity p_gravity = LGravity::LNone;
    LConstraintAdjustment p_constraintAdjustment = LConstraintAdjustment::LNone;

    LSurface *p_linkedSurface = nullptr;

#if LOUVRE_XDG_WM_BASE_VERSION >=3
    bool p_isReactive = false;
    LSize p_parentSize;
    UInt32 p_parentConfigureSerial;
#endif

};

#endif // LPOSITIONER_H
