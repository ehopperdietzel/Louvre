#ifndef WPOSITIONER_H
#define WPOSITIONER_H

#include <WNamespaces.h>
#include <WRect.h>

class Wpp::WPositioner
{
public:
    WPositioner(WClient *client);

    WClient *client()           const { return p_client; };
    wl_resource *resource()     const { return p_resource; };

    const WSize &size()         const { return p_size; };
    const WRect &anchorRect()   const { return p_anchorRect; };
    Anchor anchor()             const { return p_anchor; };
    Gravity gravity()           const { return p_gravity; };
    const WPoint &offset()      const { return p_offset; };
    bool isReactive()           const { return p_isReactive; }

    WPoint calculatePopupPosition(const WRect &outputRect, const WPoint &parentPosition);

    ConstraintAdjustment constraintAdjustment() const { return p_constraintAdjustment; };

private:
    friend class Wpp::Extensions::XdgShell::WmBase;
    friend class Wpp::Extensions::XdgShell::Popup;
    friend class Wpp::Extensions::XdgShell::Positioner;

    wl_resource *p_resource = nullptr;
    WClient *p_client = nullptr;

    WSize p_size;
    WSize p_parentSize;
    WRect p_anchorRect;
    WPoint p_offset;

    Anchor p_anchor;
    Gravity p_gravity;
    ConstraintAdjustment p_constraintAdjustment;

    WSurface *p_linkedSurface = nullptr;

    bool p_isReactive = false;
};

#endif // WPOSITIONER_H
