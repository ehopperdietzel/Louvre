#include <WPointF.h>
#include <WPoint.h>

using namespace Wpp;

inline WPointF &WPointF::operator+=(const WPoint &p)
{
    _x += p._x;
    _y += p._y;
    return *this;
}

inline WPointF &WPointF::operator-=(const WPoint &p)
{
    _x -= p._x;
    _y -= p._y;
    return *this;
}

inline WPointF &WPointF::operator*=(const WPoint &p)
{
    _x *= p._x;
    _y *= p._y;
    return *this;
}

inline WPointF &WPointF::operator/=(const WPoint &p)
{
    _x /= p._x;
    _y /= p._y;
    return *this;
}

inline WPointF WPointF::operator+(const WPoint &p)
{
    return WPointF(_x+p._x,_y+p._y);
}

inline WPointF WPointF::operator-(const WPoint &p)
{
    return WPointF(_x-p._x,_y-p._y);
}

inline WPointF WPointF::operator*(const WPoint &p)
{
    return WPointF(_x*p._x,_y*p._y);
}

inline WPointF WPointF::operator/(const WPoint &p)
{
    return WPointF(_x/p._x,_y/p._y);
}

WPointF::WPointF(const WPoint &point)
{
    _x = point._x;
    _y = point._y;
}
