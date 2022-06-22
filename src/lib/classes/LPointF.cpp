#include <LPointF.h>
#include <LPoint.h>

using namespace Louvre;

LPointF &LPointF::operator+=(const LPoint &p)
{
    m_x += p.m_x;
    m_y += p.m_y;
    return *this;
}

LPointF &LPointF::operator-=(const LPoint &p)
{
    m_x -= p.m_x;
    m_y -= p.m_y;
    return *this;
}

LPointF &LPointF::operator*=(const LPoint &p)
{
    m_x *= p.m_x;
    m_y *= p.m_y;
    return *this;
}

LPointF &LPointF::operator/=(const LPoint &p)
{
    m_x /= p.m_x;
    m_y /= p.m_y;
    return *this;
}

LPointF LPointF::operator+(const LPoint &p) const
{
    return LPointF(m_x+p.m_x,m_y+p.m_y);
}

LPointF LPointF::operator-(const LPoint &p) const
{
    return LPointF(m_x-p.m_x,m_y-p.m_y);
}

LPointF LPointF::operator*(const LPoint &p) const
{
    return LPointF(m_x*p.m_x,m_y*p.m_y);
}

LPointF LPointF::operator/(const LPoint &p) const
{
    return LPointF(m_x/p.m_x,m_y/p.m_y);
}

LPointF::LPointF(const LPoint &point)
{
    m_x = point.m_x;
    m_y = point.m_y;
}
