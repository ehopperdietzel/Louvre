#include <WPoint.h>
#include <WPointF.h>

using namespace Wpp;

inline WPoint &WPoint::operator+=(const WPointF &p)
{
    _x += p._x;
    _y += p._y;
    return *this;
}

inline WPoint &WPoint::operator-=(const WPointF &p)
{
    _x -= p._x;
    _y -= p._y;
    return *this;
}

inline WPoint &WPoint::operator*=(const WPointF &p)
{
    _x *= p._x;
    _y *= p._y;
    return *this;
}

inline WPoint &WPoint::operator/=(const WPointF &p)
{
    _x /= p._x;
    _y /= p._y;
    return *this;
}

inline WPoint WPoint::operator+(const WPointF &p)
{
    return WPoint(_x+p._x,_y+p._y);
}

inline WPoint WPoint::operator-(const WPointF &p)
{
    return WPoint(_x-p._x,_y-p._y);
}

inline WPoint WPoint::operator*(const WPointF &p)
{
    return WPoint(_x*p._x,_y*p._y);
}

inline WPoint WPoint::operator/(const WPointF &p)
{
    return WPoint(_x/p._x,_y/p._y);
}


WPoint::WPoint(const WPointF &pointF)
{
    _x = pointF._x;
    _y = pointF._y;
}
