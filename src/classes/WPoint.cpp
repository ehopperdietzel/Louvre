#include <WPoint.h>
#include <WPointF.h>

using namespace Wpp;

WPoint WPoint::constrainedToHeight(UInt32 size) const
{
    WPoint point;

    if(size == 0 || h() == 0)
        return point;

    point.setW((w()*size)/h());
    point.setH(size);
    
    return point;
}

WPoint &WPoint::operator+=(const WPointF &p)
{
    p_x += p.p_x;
    p_y += p.p_y;
    return *this;
}

WPoint &WPoint::operator-=(const WPointF &p)
{
    p_x -= p.p_x;
    p_y -= p.p_y;
    return *this;
}

WPoint &WPoint::operator*=(const WPointF &p)
{
    p_x *= p.p_x;
    p_y *= p.p_y;
    return *this;
}

WPoint &WPoint::operator/=(const WPointF &p)
{
    p_x /= p.p_x;
    p_y /= p.p_y;
    return *this;
}

WPoint WPoint::operator+(const WPointF &p) const
{
    return WPoint(p_x+p.p_x,p_y+p.p_y);
}

WPoint WPoint::operator-(const WPointF &p) const
{
    return WPoint(p_x-p.p_x,p_y-p.p_y);
}

WPoint WPoint::operator*(const WPointF &p) const
{
    return WPoint(p_x*p.p_x,p_y*p.p_y);
}

WPoint WPoint::operator/(const WPointF &p) const
{
    return WPoint(p_x/p.p_x,p_y/p.p_y);
}


WPoint::WPoint(const WPointF &pointF)
{
    p_x = pointF.p_x;
    p_y = pointF.p_y;
}
