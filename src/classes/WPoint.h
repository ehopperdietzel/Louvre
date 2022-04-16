#ifndef WPOINT_H
#define WPOINT_H

#include <WNamespaces.h>

class Wpp::WPoint
{
public:

    // Constructors
    inline WPoint() : _x(0), _y(0) {}
    inline WPoint(Int32 x, Int32 y) : _x(x), _y(y){}
    inline WPoint(double x, double y) : _x(x), _y(y){}
    WPoint(const WPointF &pointF);

    Int32 x(){return _x;}
    Int32 y(){return _y;}
    Int32 w(){return _x;}
    Int32 h(){return _y;}
    Int32 width(){return _x;}
    Int32 height(){return _y;}
    Int32 area(){return _x*_y;}

    void setX(Int32 x){_x = x;}
    void setY(Int32 y){_y = y;}
    void setW(Int32 x){_x = x;}
    void setH(Int32 y){_y = y;}
    void setWidth(Int32 x){_x = x;}
    void setHeight(Int32 y){_y = y;}

    inline WPoint &operator+=(Int32 factor)
    {
        _x += factor;
        _y += factor;
        return *this;
    }

    inline WPoint &operator-=(Int32 factor)
    {
        _x -= factor;
        _y -= factor;
        return *this;
    }

    inline WPoint &operator*=(Int32 factor)
    {
        _x *= factor;
        _y *= factor;
        return *this;
    }

    inline WPoint &operator/=(Int32 factor)
    {
        _x /= factor;
        _y /= factor;
        return *this;
    }

    inline WPoint &operator+=(double factor)
    {
        _x += factor;
        _y += factor;
        return *this;
    }

    inline WPoint &operator-=(double factor)
    {
        _x -= factor;
        _y -= factor;
        return *this;
    }

    inline WPoint &operator*=(double factor)
    {
        _x *= factor;
        _y *= factor;
        return *this;
    }

    inline WPoint &operator/=(double factor)
    {
        _x /= factor;
        _y /= factor;
        return *this;
    }

    inline WPoint &operator+=(const WPoint &p)
    {
        _x += p._x;
        _y += p._y;
        return *this;
    }

    inline WPoint &operator-=(const WPoint &p)
    {
        _x -= p._x;
        _y -= p._y;
        return *this;
    }

    inline WPoint &operator*=(const WPoint &p)
    {
        _x *= p._x;
        _y *= p._y;
        return *this;
    }

    inline WPoint &operator/=(const WPoint &p)
    {
        _x /= p._x;
        _y /= p._y;
        return *this;
    }

    inline WPoint &operator+=(const WPointF &p);
    inline WPoint &operator-=(const WPointF &p);
    inline WPoint &operator*=(const WPointF &p);
    inline WPoint &operator/=(const WPointF &p);


    inline WPoint operator+(Int32 factor)
    {
        return WPoint(_x+factor,_y+factor);
    }

    inline WPoint operator-(Int32 factor)
    {
        return WPoint(_x-factor,_y-factor);
    }

    inline WPoint operator*(Int32 factor)
    {
        return WPoint(_x*factor,_y*factor);
    }

    inline WPoint operator/(Int32 factor)
    {
        return WPoint(_x/factor,_y/factor);
    }

    inline WPoint operator+(double factor)
    {
        return WPoint(_x+factor,_y+factor);
    }

    inline WPoint operator-(double factor)
    {
        return WPoint(_x-factor,_y-factor);
    }

    inline WPoint operator*(double factor)
    {
        return WPoint(_x*factor,_y*factor);
    }

    inline WPoint operator/(double factor)
    {
        return WPoint(_x/factor,_y/factor);
    }

    inline WPoint operator+(const WPoint &p)
    {
        return WPoint(_x+p._x,_y+p._y);
    }

    inline WPoint operator-(const WPoint &p)
    {
        return WPoint(_x-p._x,_y-p._y);
    }

    inline WPoint operator*(const WPoint &p)
    {
        return WPoint(_x*p._x,_y*p._y);
    }

    inline WPoint operator/(const WPoint &p)
    {
        return WPoint(_x/p._x,_y/p._y);
    }

    inline WPoint operator+(const WPointF &p);
    inline WPoint operator-(const WPointF &p);
    inline WPoint operator*(const WPointF &p);
    inline WPoint operator/(const WPointF &p);

    inline bool operator==(const WPoint &p)
    {
        return _x == p._x && _y == p._y;
    }

    inline bool operator!=(const WPoint &p)
    {
        return _x != p._x || _y != p._y;
    }

private:
    friend class WRect;
    friend class WPointF;
    Int32 _x,_y;
};

#endif // WPOINT_H
