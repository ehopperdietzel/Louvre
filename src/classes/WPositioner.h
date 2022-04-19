#ifndef WPOSITIONER_H
#define WPOSITIONER_H

#include <WNamespaces.h>
#include <WRect.h>

class Wpp::WPositioner
{
public:
    WPositioner(WClient *client);

    WClient     *client()   { return _client; };
    wl_resource *resource() { return _resource; };

    WSize   size()       { return _size; };
    WRect   anchorRect() { return _anchorRect; };
    Anchor  anchor()     { return _anchor; };
    Gravity gravity()    { return _gravity; };
    WPoint  offset()     { return _offset; };
    bool    isReactive() { return _isReactive; }

    WPoint calculatePopupPosition(const WRect &outputRect, const WPoint &parentPosition);

    ConstraintAdjustment constraintAdjustment() { return _constraintAdjustment; };

private:
    friend class Wpp::Extensions::XdgShell::WmBase;
    friend class Wpp::Extensions::XdgShell::Popup;
    friend class Wpp::Extensions::XdgShell::Positioner;

    wl_resource *_resource = nullptr;
    WClient *_client = nullptr;

    WSize _size;
    WSize _parentSize;
    WRect _anchorRect;
    WPoint _offset;

    Anchor _anchor;
    Gravity _gravity;
    ConstraintAdjustment _constraintAdjustment;

    WSurface *_linkedSurface = nullptr;

    bool _isReactive = false;
};

#endif // WPOSITIONER_H
