#include <WPoint.h>
#include <WPointF.h>

using namespace Wpp;

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
