#include "WRegion.h"

using namespace Wpp;

WRegion::WRegion()
{

}

void WRegion::copy(const WRegion &region)
{
    clear();
    std::copy(region.p_rects.begin(),region.p_rects.end(),std::back_inserter(p_rects));
}

void WRegion::clear()
{
    p_rects.clear();
}

void WRegion::addRect(const WRect &rect)
{
    WRegionRect regionRect = {rect, true};
    p_rects.push_back(regionRect);
}

void WRegion::subtractRect(const WRect &rect)
{
    WRegionRect regionRect = {rect, false};
    p_rects.push_back(regionRect);
}

void WRegion::multiply(double factor)
{
    for(WRegionRect &rect : p_rects)
        rect.rect *= factor;
}

bool WRegion::containsPoint(const WPoint &point)
{
    bool contains = false;

    for(std::list<WRegionRect>::reverse_iterator r = p_rects.rbegin(); r != p_rects.rend(); r++)
    {
        if((*r).rect.containsPoint(point))
        {
            contains = (*r).add;
            break;
        }
    }

    return contains;
}
