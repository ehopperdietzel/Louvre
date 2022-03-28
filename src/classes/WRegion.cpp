#include "WRegion.h"

using namespace WaylandPlus;

WRegion::WRegion(UInt32 id, wl_resource *resource, WClient *client)
{
    _id = id;
    _resource = resource;
    _client = client;
}

wl_resource *WRegion::getResource()
{
    return _resource;
}

UInt32 WRegion::getId()
{
    return _id;
}

WClient *WRegion::getClient()
{
    return _client;
}

void WRegion::addRect(Int32 x, Int32 y, Int32 width, Int32 height)
{
    WRegionRect rect = {x, y, width, height, true};
    rects.push_back(rect);
}

void WRegion::subtractRect(Int32 x, Int32 y, Int32 width, Int32 height)
{
    WRegionRect rect = {x, y, width, height, false};
    rects.push_back(rect);
}
