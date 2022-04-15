#ifndef WPOSITIONER_H
#define WPOSITIONER_H

#include <WNamespaces.h>

class Wpp::WPositioner
{
public:
    WPositioner(WClient *client);

    WClient *getClient();
    wl_resource *getResource();

    const Size getSize();
    Int32 getWidth();
    Int32 getHeight();
    const Rect getRect();
    Anchor getAnchor();
    Gravity getGravity();
    const Point getOffset();

    ConstraintAdjustment getConstraintAdjustment();
private:
    friend class Wpp::Extensions::XdgShell::WmBase;
    friend class Wpp::Extensions::XdgShell::Popup;
    friend class Wpp::Extensions::XdgShell::Positioner;

    wl_resource *_resource = nullptr;
    WClient *_client = nullptr;

    Size _size = {0,0};
    Rect _anchorRect;
    Anchor _anchor;
    Gravity _gravity;
    ConstraintAdjustment _constraintAdjustment;
    Point _offset;
    Size _parentSize;

    WSurface *_linkedSurface = nullptr;
};

#endif // WPOSITIONER_H
