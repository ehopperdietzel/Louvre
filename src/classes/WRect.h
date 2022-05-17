#ifndef WRECT_H
#define WRECT_H

#include <WNamespaces.h>
#include <WPoint.h>

class Wpp::WRect
{
public:

    // Constructors
    inline WRect() {}
    inline WRect(Int32 x, Int32 y, Int32 width, Int32 height)
    {
        p_topLeft.p_x       = x;
        p_topLeft.p_y       = y;
        p_bottomRight.p_x   = width;
        p_bottomRight.p_y   = height;
    }
    inline WRect(const WPoint &topLeft, const WPoint &bottomRight) : p_topLeft(topLeft), p_bottomRight(bottomRight){}

    Int32 x()       const   {return p_topLeft.p_x;}
    Int32 y()       const   {return p_topLeft.p_y;}
    Int32 w()       const   {return p_bottomRight.p_x;}
    Int32 h()       const   {return p_bottomRight.p_y;}
    Int32 width()   const   {return p_bottomRight.p_x;}
    Int32 height()  const   {return p_bottomRight.p_y;}
    Int32 area()    const   {return p_bottomRight.p_x*p_bottomRight.p_y;}

    bool containsPoint(const WPoint &point) const
    {
        return point.p_x >= p_topLeft.p_x &&
               point.p_x <= p_topLeft.p_x + p_bottomRight.p_x &&
               point.p_y >= p_topLeft.p_y &&
               point.p_y <= p_topLeft.p_y + p_bottomRight.p_y;
    }

    WPoint topLeft()    const   {return p_topLeft;}
    WSize bottomRight() const   {return p_bottomRight;}

    void setX(Int32 x){p_topLeft.p_x = x;}
    void setY(Int32 y){p_topLeft.p_y = y;}
    void setW(Int32 width){p_bottomRight.p_x = width;}
    void setH(Int32 height){p_bottomRight.p_y = height;}
    void setWidth(Int32 width){p_bottomRight.p_x = width;}
    void setHeight(Int32 height){p_bottomRight.p_y = height;}

    inline WRect &operator+=(Int32 factor)
    {
        p_topLeft.p_x += factor;
        p_topLeft.p_y += factor;
        p_bottomRight.p_x += factor;
        p_bottomRight.p_y += factor;
        return *this;
    }

    inline WRect &operator-=(Int32 factor)
    {
        p_topLeft.p_x -= factor;
        p_topLeft.p_y -= factor;
        p_bottomRight.p_x -= factor;
        p_bottomRight.p_y -= factor;
        return *this;
    }

    inline WRect &operator*=(Int32 factor)
    {
        p_topLeft.p_x *= factor;
        p_topLeft.p_y *= factor;
        p_bottomRight.p_x *= factor;
        p_bottomRight.p_y *= factor;
        return *this;
    }

    inline WRect &operator/=(Int32 factor)
    {
        p_topLeft.p_x /= factor;
        p_topLeft.p_y /= factor;
        p_bottomRight.p_x /= factor;
        p_bottomRight.p_y /= factor;
        return *this;
    }

    inline WRect &operator+=(double factor)
    {
        p_topLeft.p_x += factor;
        p_topLeft.p_y += factor;
        p_bottomRight.p_x += factor;
        p_bottomRight.p_y += factor;
        return *this;
    }

    inline WRect &operator-=(double factor)
    {
        p_topLeft.p_x -= factor;
        p_topLeft.p_y -= factor;
        p_bottomRight.p_x -= factor;
        p_bottomRight.p_y -= factor;
        return *this;
    }

    inline WRect &operator*=(double factor)
    {
        p_topLeft.p_x *= factor;
        p_topLeft.p_y *= factor;
        p_bottomRight.p_x *= factor;
        p_bottomRight.p_y *= factor;
        return *this;
    }

    inline WRect &operator/=(double factor)
    {
        p_topLeft.p_x /= factor;
        p_topLeft.p_y /= factor;
        p_bottomRight.p_x /= factor;
        p_bottomRight.p_y /= factor;
        return *this;
    }

    inline WRect &operator+=(const WRect &r)
    {
        p_topLeft.p_x += r.p_topLeft.p_x;
        p_topLeft.p_y += r.p_topLeft.p_y;
        p_bottomRight.p_x += r.p_bottomRight.p_x;
        p_bottomRight.p_y += r.p_bottomRight.p_y;
        return *this;
    }

    inline WRect &operator-=(const WRect &r)
    {
        p_topLeft.p_x -= r.p_topLeft.p_x;
        p_topLeft.p_y -= r.p_topLeft.p_y;
        p_bottomRight.p_x -= r.p_bottomRight.p_x;
        p_bottomRight.p_y -= r.p_bottomRight.p_y;
        return *this;
    }

    inline WRect &operator*=(const WRect &r)
    {
        p_topLeft.p_x *= r.p_topLeft.p_x;
        p_topLeft.p_y *= r.p_topLeft.p_y;
        p_bottomRight.p_x *= r.p_bottomRight.p_x;
        p_bottomRight.p_y *= r.p_bottomRight.p_y;
        return *this;
    }

    inline WRect &operator/=(const WRect &r)
    {
        p_topLeft.p_x /= r.p_topLeft.p_x;
        p_topLeft.p_y /= r.p_topLeft.p_y;
        p_bottomRight.p_x /= r.p_bottomRight.p_x;
        p_bottomRight.p_y /= r.p_bottomRight.p_y;
        return *this;
    }

    inline WRect operator+(Int32 factor) const
    {
        return WRect(p_topLeft.p_x+factor,p_topLeft.p_y+factor,p_bottomRight.p_x+factor,p_bottomRight.p_y+factor);
    }

    inline WRect operator-(Int32 factor) const
    {
        return WRect(p_topLeft.p_x-factor,p_topLeft.p_y-factor,p_bottomRight.p_x-factor,p_bottomRight.p_y-factor);
    }

    inline WRect operator*(Int32 factor) const
    {
        return WRect(p_topLeft.p_x*factor,p_topLeft.p_y*factor,p_bottomRight.p_x*factor,p_bottomRight.p_y*factor);
    }

    inline WRect operator/(Int32 factor) const
    {
        return WRect(p_topLeft.p_x/factor,p_topLeft.p_y/factor,p_bottomRight.p_x/factor,p_bottomRight.p_y/factor);
    }

    inline WRect operator+(double factor) const
    {
        return WRect(p_topLeft.p_x+factor,p_topLeft.p_y+factor,p_bottomRight.p_x+factor,p_bottomRight.p_y+factor);
    }

    inline WRect operator-(double factor) const
    {
        return WRect(p_topLeft.p_x-factor,p_topLeft.p_y-factor,p_bottomRight.p_x-factor,p_bottomRight.p_y-factor);
    }

    inline WRect operator*(double factor) const
    {
        return WRect(p_topLeft.p_x*factor,p_topLeft.p_y*factor,p_bottomRight.p_x*factor,p_bottomRight.p_y*factor);
    }

    inline WRect operator/(double factor) const
    {
        return WRect(p_topLeft.p_x/factor,p_topLeft.p_y/factor,p_bottomRight.p_x/factor,p_bottomRight.p_y/factor);
    }

    inline WRect operator+(const WRect &r) const
    {
        return WRect(p_topLeft.p_x+r.p_topLeft.p_x,p_topLeft.p_y+r.p_topLeft.p_y,p_bottomRight.p_x+r.p_bottomRight.p_x,p_bottomRight.p_y+r.p_bottomRight.p_y);
    }

    inline WRect operator-(const WRect &r) const
    {
        return WRect(p_topLeft.p_x-r.p_topLeft.p_x,p_topLeft.p_y-r.p_topLeft.p_y,p_bottomRight.p_x-r.p_bottomRight.p_x,p_bottomRight.p_y-r.p_bottomRight.p_y);
    }

    inline WRect operator*(const WRect &r) const
    {
        return WRect(p_topLeft.p_x*r.p_topLeft.p_x,p_topLeft.p_y*r.p_topLeft.p_y,p_bottomRight.p_x*r.p_bottomRight.p_x,p_bottomRight.p_y*r.p_bottomRight.p_y);
    }

    inline WRect operator/(const WRect &r) const
    {
        return WRect(p_topLeft.p_x/r.p_topLeft.p_x,p_topLeft.p_y/r.p_topLeft.p_y,p_bottomRight.p_x/r.p_bottomRight.p_x,p_bottomRight.p_y/r.p_bottomRight.p_y);
    }

    inline bool operator==(const WRect &p) const
    {
        return p_topLeft.p_x == p.p_topLeft.p_x && p_topLeft.p_y == p.p_topLeft.p_y && p_bottomRight.p_x == p.p_bottomRight.p_x && p_bottomRight.p_y == p.p_bottomRight.p_y;
    }

    inline bool operator!=(const WRect &p) const
    {
        return p_topLeft.p_x != p.p_topLeft.p_x || p_topLeft.p_y != p.p_topLeft.p_y || p_bottomRight.p_x != p.p_bottomRight.p_x || p_bottomRight.p_y != p.p_bottomRight.p_y;
    }

private:

    WPoint p_topLeft,p_bottomRight;
};


#endif // WRECT_H
