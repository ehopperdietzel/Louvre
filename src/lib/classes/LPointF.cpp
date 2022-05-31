#include <LPointF.h>
#include <LPoint.h>

using namespace Louvre;

LPointF &LPointF::operator+=(const LPoint &p)
{
    p_x += p.p_x;
    p_y += p.p_y;
    return *this;
}

LPointF &LPointF::operator-=(const LPoint &p)
{
    p_x -= p.p_x;
    p_y -= p.p_y;
    return *this;
}

LPointF &LPointF::operator*=(const LPoint &p)
{
    p_x *= p.p_x;
    p_y *= p.p_y;
    return *this;
}

LPointF &LPointF::operator/=(const LPoint &p)
{
    p_x /= p.p_x;
    p_y /= p.p_y;
    return *this;
}

LPointF LPointF::operator+(const LPoint &p) const
{
    return LPointF(p_x+p.p_x,p_y+p.p_y);
}

LPointF LPointF::operator-(const LPoint &p) const
{
    return LPointF(p_x-p.p_x,p_y-p.p_y);
}

LPointF LPointF::operator*(const LPoint &p) const
{
    return LPointF(p_x*p.p_x,p_y*p.p_y);
}

LPointF LPointF::operator/(const LPoint &p) const
{
    return LPointF(p_x/p.p_x,p_y/p.p_y);
}

LPointF::LPointF(const LPoint &point)
{
    p_x = point.p_x;
    p_y = point.p_y;
}
