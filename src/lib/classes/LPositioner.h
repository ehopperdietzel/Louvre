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

    wl_resource *m_resource = nullptr;
    LClient *m_client = nullptr;

    LSize m_size;
    LRect m_anchorRect;
    LPoint m_offset;

    LAnchor m_anchor = LAnchor::LNone;
    LGravity m_gravity = LGravity::LNone;
    LConstraintAdjustment m_constraintAdjustment = LConstraintAdjustment::LNone;

    LSurface *m_linkedSurface = nullptr;

#if LOUVRE_XDG_WM_BASE_VERSION >=3
    bool m_isReactive = false;
    LSize m_parentSize;
    UInt32 m_parentConfigureSerial;
#endif

};

#endif // LPOSITIONER_H
