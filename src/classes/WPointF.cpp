#include <WPointF.h>
#include <WPoint.h>

using namespace Wpp;

WPointF &WPointF::operator+=(const WPoint &p)
{
    p_x += p.p_x;
    p_y += p.p_y;
    return *this;
}

WPointF &WPointF::operator-=(const WPoint &p)
{
    p_x -= p.p_x;
    p_y -= p.p_y;
    return *this;
}

WPointF &WPointF::operator*=(const WPoint &p)
{
    p_x *= p.p_x;
    p_y *= p.p_y;
    return *this;
}

WPointF &WPointF::operator/=(const WPoint &p)
{
    p_x /= p.p_x;
    p_y /= p.p_y;
    return *this;
}

WPointF WPointF::operator+(const WPoint &p) const
{
    return WPointF(p_x+p.p_x,p_y+p.p_y);
}

WPointF WPointF::operator-(const WPoint &p) const
{
    return WPointF(p_x-p.p_x,p_y-p.p_y);
}

WPointF WPointF::operator*(const WPoint &p) const
{
    return WPointF(p_x*p.p_x,p_y*p.p_y);
}

WPointF WPointF::operator/(const WPoint &p) const
{
    return WPointF(p_x/p.p_x,p_y/p.p_y);
}

WPointF::WPointF(const WPoint &point)
{
    p_x = point.p_x;
    p_y = point.p_y;
}
