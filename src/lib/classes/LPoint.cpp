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
    m_x += p.m_x;
    m_y += p.m_y;
    return *this;
}

LPoint &LPoint::operator-=(const LPointF &p)
{
    m_x -= p.m_x;
    m_y -= p.m_y;
    return *this;
}

LPoint &LPoint::operator*=(const LPointF &p)
{
    m_x *= p.m_x;
    m_y *= p.m_y;
    return *this;
}

LPoint &LPoint::operator/=(const LPointF &p)
{
    m_x /= p.m_x;
    m_y /= p.m_y;
    return *this;
}

LPoint LPoint::operator+(const LPointF &p) const
{
    return LPoint(m_x+p.m_x,m_y+p.m_y);
}

LPoint LPoint::operator-(const LPointF &p) const
{
    return LPoint(m_x-p.m_x,m_y-p.m_y);
}

LPoint LPoint::operator*(const LPointF &p) const
{
    return LPoint(m_x*p.m_x,m_y*p.m_y);
}

LPoint LPoint::operator/(const LPointF &p) const
{
    return LPoint(m_x/p.m_x,m_y/p.m_y);
}


LPoint::LPoint(const LPointF &pointF)
{
    m_x = pointF.m_x;
    m_y = pointF.m_y;
}
