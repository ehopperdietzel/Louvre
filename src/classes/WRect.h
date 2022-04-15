#ifndef WRECT_H
#define WRECT_H

#include <WNamespaces.h>
#include <WPoint.h>

class Wpp::WRect
{
public:

    // Constructors
    inline WRect() : _x(0), _y(0), _w(0), _h(0) {}
    inline WRect(Int32 x, Int32 y, Int32 width, Int32 height) : _x(x), _y(y), _w(width), _h(height){}
    inline WRect(double x, double y, double width, double height) : _x(x), _y(y), _w(width), _h(height){}
    inline WRect(const WPoint &topLeft, const WPoint &bottomRight) : _x(topLeft._x), _y(topLeft._y), _w(bottomRight._x), _h(bottomRight._y){}

    Int32 x(){return _x;}
    Int32 y(){return _y;}
    Int32 w(){return _w;}
    Int32 h(){return _h;}
    Int32 width(){return _w;}
    Int32 height(){return _h;}
    const WPoint topLeft(){return WPoint(_x,_y);}
    const WSize bottomRight(){return WSize(_w,_h);}
    void setX(Int32 x){_x = x;}
    void setY(Int32 y){_y = y;}
    void setW(Int32 width){_w = width;}
    void setH(Int32 height){_h = height;}
    void setWidth(Int32 width){_w = width;}
    void setHeight(Int32 height){_h = height;}

    inline WRect &operator+=(Int32 factor)
    {
        _x += factor;
        _y += factor;
        _w += factor;
        _h += factor;
        return *this;
    }

    inline WRect &operator-=(Int32 factor)
    {
        _x -= factor;
        _y -= factor;
        _w -= factor;
        _h -= factor;
        return *this;
    }

    inline WRect &operator*=(Int32 factor)
    {
        _x *= factor;
        _y *= factor;
        _w *= factor;
        _h *= factor;
        return *this;
    }

    inline WRect &operator/=(Int32 factor)
    {
        _x /= factor;
        _y /= factor;
        _w /= factor;
        _h /= factor;
        return *this;
    }

    inline WRect &operator+=(double factor)
    {
        _x += factor;
        _y += factor;
        _w += factor;
        _h += factor;
        return *this;
    }

    inline WRect &operator-=(double factor)
    {
        _x -= factor;
        _y -= factor;
        _w -= factor;
        _h -= factor;
        return *this;
    }

    inline WRect &operator*=(double factor)
    {
        _x *= factor;
        _y *= factor;
        _w *= factor;
        _h *= factor;
        return *this;
    }

    inline WRect &operator/=(double factor)
    {
        _x /= factor;
        _y /= factor;
        _w /= factor;
        _h /= factor;
        return *this;
    }

    inline WRect &operator+=(const WRect &r)
    {
        _x += r._x;
        _y += r._y;
        _w += r._w;
        _h += r._h;
        return *this;
    }

    inline WRect &operator-=(const WRect &r)
    {
        _x -= r._x;
        _y -= r._y;
        _w -= r._w;
        _h -= r._h;
        return *this;
    }

    inline WRect &operator*=(const WRect &r)
    {
        _x *= r._x;
        _y *= r._y;
        _w *= r._w;
        _h *= r._h;
        return *this;
    }

    inline WRect &operator/=(const WRect &r)
    {
        _x /= r._x;
        _y /= r._y;
        _w /= r._w;
        _h /= r._h;
        return *this;
    }

    inline WRect operator+(Int32 factor)
    {
        return WRect(_x+factor,_y+factor,_w+factor,_h+factor);
    }

    inline WRect operator-(Int32 factor)
    {
        return WRect(_x-factor,_y-factor,_w-factor,_h-factor);
    }

    inline WRect operator*(Int32 factor)
    {
        return WRect(_x*factor,_y*factor,_w*factor,_h*factor);
    }

    inline WRect operator/(Int32 factor)
    {
        return WRect(_x/factor,_y/factor,_w/factor,_h/factor);
    }

    inline WRect operator+(double factor)
    {
        return WRect(_x+factor,_y+factor,_w+factor,_h+factor);
    }

    inline WRect operator-(double factor)
    {
        return WRect(_x-factor,_y-factor,_w-factor,_h-factor);
    }

    inline WRect operator*(double factor)
    {
        return WRect(_x*factor,_y*factor,_w*factor,_h*factor);
    }

    inline WRect operator/(double factor)
    {
        return WRect(_x/factor,_y/factor,_w/factor,_h/factor);
    }

    inline WRect operator+(const WRect &r)
    {
        return WRect(_x+r._x,_y+r._y,_w+r._w,_h+r._h);
    }

    inline WRect operator-(const WRect &r)
    {
        return WRect(_x-r._x,_y-r._y,_w-r._w,_h-r._h);
    }

    inline WRect operator*(const WRect &r)
    {
        return WRect(_x*r._x,_y*r._y,_w*r._w,_h*r._h);
    }

    inline WRect operator/(const WRect &r)
    {
        return WRect(_x/r._x,_y/r._y,_w/r._w,_h/r._h);
    }

    inline bool operator==(const WRect &p)
    {
        return _x == p._x && _y == p._y && _w == p._w && _h == p._h;
    }

    inline bool operator!=(const WRect &p)
    {
        return _x != p._x || _y != p._y || _w != p._w || _h != p._h;
    }

private:
    Int32 _x,_y,_w,_h;
};

#endif // WRECT_H
