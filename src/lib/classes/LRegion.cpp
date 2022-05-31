#include "LRegion.h"

using namespace Louvre;

LRegion::LRegion()
{

}

void LRegion::copy(const LRegion &region)
{
    clear();
    std::copy(region.p_rects.begin(),region.p_rects.end(),std::back_inserter(p_rects));
}

void LRegion::clear()
{
    p_rects.clear();
}

void LRegion::addRect(const LRect &rect)
{
    LRegionRect regionRect = {rect, true};
    p_rects.push_back(regionRect);
}

void LRegion::subtractRect(const LRect &rect)
{
    LRegionRect regionRect = {rect, false};
    p_rects.push_back(regionRect);
}

void LRegion::multiply(double factor)
{
    for(LRegionRect &rect : p_rects)
        rect.rect *= factor;
}

bool LRegion::containsPoint(const LPoint &point)
{
    bool contains = false;

    for(std::list<LRegionRect>::reverse_iterator r = p_rects.rbegin(); r != p_rects.rend(); r++)
    {
        if((*r).rect.containsPoint(point))
        {
            contains = (*r).add;
            break;
        }
    }

    return contains;
}
