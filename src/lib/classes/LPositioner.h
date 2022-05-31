#ifndef LPOSITIONER_H
#define LPOSITIONER_H

#include <LNamespaces.h>
#include <LRect.h>

class Louvre::LPositioner
{
public:
    LPositioner(LClient *client);

    LClient *client()           const { return p_client; };
    wl_resource *resource()     const { return p_resource; };

    const LSize &size()         const { return p_size; };
    const LRect &anchorRect()   const { return p_anchorRect; };
    Anchor anchor()             const { return p_anchor; };
    Gravity gravity()           const { return p_gravity; };
    const LPoint &offset()      const { return p_offset; };
    bool isReactive()           const { return p_isReactive; }

    LPoint calculatePopupPosition(const LRect &outputRect, const LPoint &parentPosition);

    ConstraintAdjustment constraintAdjustment() const { return p_constraintAdjustment; };

private:
    friend class Louvre::Extensions::XdgShell::WmBase;
    friend class Louvre::Extensions::XdgShell::Popup;
    friend class Louvre::Extensions::XdgShell::Positioner;

    wl_resource *p_resource = nullptr;
    LClient *p_client = nullptr;

    LSize p_size;
    LSize p_parentSize;
    LRect p_anchorRect;
    LPoint p_offset;

    Anchor p_anchor;
    Gravity p_gravity;
    ConstraintAdjustment p_constraintAdjustment;

    LSurface *p_linkedSurface = nullptr;

    bool p_isReactive = false;
};

#endif // LPOSITIONER_H
