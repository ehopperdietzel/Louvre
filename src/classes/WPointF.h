#ifndef WPointFF_H
#define WPointFF_H

#include <WNamespaces.h>

class Wpp::WPointF
{
public:

    // Constructors
    inline WPointF() : _x(0), _y(0) {}
    inline WPointF(Int32 x, Int32 y) : _x(x), _y(y){}
    inline WPointF(double x, double y) : _x(x), _y(y){}
    WPointF(const WPoint &point);

    Int32 x(){return _x;}
    Int32 y(){return _y;}
    Int32 w(){return _x;}
    Int32 h(){return _y;}
    Int32 width(){return _x;}
    Int32 height(){return _y;}

    void setX(Int32 x){_x = x;}
    void setY(Int32 y){_y = y;}
    void setW(Int32 x){_x = x;}
    void setH(Int32 y){_y = y;}
    void setWidth(Int32 x){_x = x;}
    void setHeight(Int32 y){_y = y;}

    inline WPointF &operator+=(Int32 factor)
    {
        _x += factor;
        _y += factor;
        return *this;
    }

    inline WPointF &operator-=(Int32 factor)
    {
        _x -= factor;
        _y -= factor;
        return *this;
    }

    inline WPointF &operator*=(Int32 factor)
    {
        _x *= factor;
        _y *= factor;
        return *this;
    }

    inline WPointF &operator/=(Int32 factor)
    {
        _x /= factor;
        _y /= factor;
        return *this;
    }

    inline WPointF &operator+=(double factor)
    {
        _x += factor;
        _y += factor;
        return *this;
    }

    inline WPointF &operator-=(double factor)
    {
        _x -= factor;
        _y -= factor;
        return *this;
    }

    inline WPointF &operator*=(double factor)
    {
        _x *= factor;
        _y *= factor;
        return *this;
    }

    inline WPointF &operator/=(double factor)
    {
        _x /= factor;
        _y /= factor;
        return *this;
    }

    inline WPointF &operator+=(const WPointF &p)
    {
        _x += p._x;
        _y += p._y;
        return *this;
    }

    inline WPointF &operator-=(const WPointF &p)
    {
        _x -= p._x;
        _y -= p._y;
        return *this;
    }

    inline WPointF &operator*=(const WPointF &p)
    {
        _x *= p._x;
        _y *= p._y;
        return *this;
    }

    inline WPointF &operator/=(const WPointF &p)
    {
        _x /= p._x;
        _y /= p._y;
        return *this;
    }

    inline WPointF &operator+=(const WPoint &p);
    inline WPointF &operator-=(const WPoint &p);
    inline WPointF &operator*=(const WPoint &p);
    inline WPointF &operator/=(const WPoint &p);

    inline WPointF operator+(Int32 factor)
    {
        return WPointF(_x+factor,_y+factor);
    }

    inline WPointF operator-(Int32 factor)
    {
        return WPointF(_x-factor,_y-factor);
    }

    inline WPointF operator*(Int32 factor)
    {
        return WPointF(_x*factor,_y*factor);
    }

    inline WPointF operator/(Int32 factor)
    {
        return WPointF(_x/factor,_y/factor);
    }

    inline WPointF operator+(double factor)
    {
        return WPointF(_x+factor,_y+factor);
    }

    inline WPointF operator-(double factor)
    {
        return WPointF(_x-factor,_y-factor);
    }

    inline WPointF operator*(double factor)
    {
        return WPointF(_x*factor,_y*factor);
    }

    inline WPointF operator/(double factor)
    {
        return WPointF(_x/factor,_y/factor);
    }

    inline WPointF operator+(const WPointF &p)
    {
        return WPointF(_x+p._x,_y+p._y);
    }

    inline WPointF operator-(const WPointF &p)
    {
        return WPointF(_x-p._x,_y-p._y);
    }

    inline WPointF operator*(const WPointF &p)
    {
        return WPointF(_x*p._x,_y*p._y);
    }

    inline WPointF operator/(const WPointF &p)
    {
        return WPointF(_x*p._x,_y*p._y);
    }

    inline WPointF operator+(const WPoint &p);
    inline WPointF operator-(const WPoint &p);
    inline WPointF operator*(const WPoint &p);
    inline WPointF operator/(const WPoint &p);

    inline bool operator==(const WPointF &p)
    {
        return _x == p._x && _y == p._y;
    }

    inline bool operator!=(const WPointF &p)
    {
        return _x != p._x || _y != p._y;
    }

private:
    friend class WRect;
    friend class WPoint;
    float _x,_y;
};
#endif // WPointFF_H
