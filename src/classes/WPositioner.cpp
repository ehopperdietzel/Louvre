#include "WPositioner.h"
#include <WClient.h>
#include <WCompositor.h>

using namespace Wpp;

WPositioner::WPositioner(WClient *client)
{
    _client = client;

    _client->positioners.push_back(this);
    //_client->newPositioner(this);
}

WClient *WPositioner::getClient()
{
    return _client;
}

wl_resource *WPositioner::getResource()
{
    return _resource;
}

const Size WPositioner::getSize()
{
    return _size;
}

Int32 WPositioner::getWidth()
{
    return _size.width;
}

Int32 WPositioner::getHeight()
{
    return _size.height;
}

const Rect WPositioner::getRect()
{
    return _anchorRect;
}

Anchor WPositioner::getAnchor()
{
    return _anchor;
}

Gravity WPositioner::getGravity()
{
    return _gravity;
}

const Point WPositioner::getOffset()
{
    return _offset;
}

ConstraintAdjustment WPositioner::getConstraintAdjustment()
{
    return _constraintAdjustment;
}
