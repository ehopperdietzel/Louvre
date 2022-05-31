#include <LPoint.h>
#include <LPointF.h>

using namespace Louvre;

LPoint LPoint::constrainedToHeight(UInt32 size) const
{
    LPoint point;

    if(size == 0 || h() == 0)
        return point;

    point.setW((w()*size)/h());
    point.setH(size);
    
    return point;
}

LPoint &LPoint::operator+=(const LPointF &p)
{
    p_x += p.p_x;
    p_y += p.p_y;
    return *this;
}

LPoint &LPoint::operator-=(const LPointF &p)
{
    p_x -= p.p_x;
    p_y -= p.p_y;
    return *this;
}

LPoint &LPoint::operator*=(const LPointF &p)
{
    p_x *= p.p_x;
    p_y *= p.p_y;
    return *this;
}

LPoint &LPoint::operator/=(const LPointF &p)
{
    p_x /= p.p_x;
    p_y /= p.p_y;
    return *this;
}

LPoint LPoint::operator+(const LPointF &p) const
{
    return LPoint(p_x+p.p_x,p_y+p.p_y);
}

LPoint LPoint::operator-(const LPointF &p) const
{
    return LPoint(p_x-p.p_x,p_y-p.p_y);
}

LPoint LPoint::operator*(const LPointF &p) const
{
    return LPoint(p_x*p.p_x,p_y*p.p_y);
}

LPoint LPoint::operator/(const LPointF &p) const
{
    return LPoint(p_x/p.p_x,p_y/p.p_y);
}


LPoint::LPoint(const LPointF &pointF)
{
    p_x = pointF.p_x;
    p_y = pointF.p_y;
}
