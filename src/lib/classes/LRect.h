#ifndef LRECT_H
#define LRECT_H

#include <LNamespaces.h>
#include <LPoint.h>

class Louvre::LRect
{
public:

    // Constructors
    inline LRect() {}
    inline LRect(Int32 x, Int32 y, Int32 width, Int32 height)
    {
        p_topLeft.p_x       = x;
        p_topLeft.p_y       = y;
        p_bottomRight.p_x   = width;
        p_bottomRight.p_y   = height;
    }
    inline LRect(const LPoint &topLeft, const LPoint &bottomRight) : p_topLeft(topLeft), p_bottomRight(bottomRight){}

    Int32 x()       const   {return p_topLeft.p_x;}
    Int32 y()       const   {return p_topLeft.p_y;}
    Int32 w()       const   {return p_bottomRight.p_x;}
    Int32 h()       const   {return p_bottomRight.p_y;}
    Int32 width()   const   {return p_bottomRight.p_x;}
    Int32 height()  const   {return p_bottomRight.p_y;}
    Int32 area()    const   {return p_bottomRight.p_x*p_bottomRight.p_y;}

    bool containsPoint(const LPoint &point) const
    {
        return point.p_x >= p_topLeft.p_x &&
               point.p_x <= p_topLeft.p_x + p_bottomRight.p_x &&
               point.p_y >= p_topLeft.p_y &&
               point.p_y <= p_topLeft.p_y + p_bottomRight.p_y;
    }

    LPoint topLeft()    const   {return p_topLeft;}
    LSize bottomRight() const   {return p_bottomRight;}

    void setX(Int32 x){p_topLeft.p_x = x;}
    void setY(Int32 y){p_topLeft.p_y = y;}
    void setW(Int32 width){p_bottomRight.p_x = width;}
    void setH(Int32 height){p_bottomRight.p_y = height;}
    void setWidth(Int32 width){p_bottomRight.p_x = width;}
    void setHeight(Int32 height){p_bottomRight.p_y = height;}

    inline LRect &operator+=(Int32 factor)
    {
        p_topLeft.p_x += factor;
        p_topLeft.p_y += factor;
        p_bottomRight.p_x += factor;
        p_bottomRight.p_y += factor;
        return *this;
    }

    inline LRect &operator-=(Int32 factor)
    {
        p_topLeft.p_x -= factor;
        p_topLeft.p_y -= factor;
        p_bottomRight.p_x -= factor;
        p_bottomRight.p_y -= factor;
        return *this;
    }

    inline LRect &operator*=(Int32 factor)
    {
        p_topLeft.p_x *= factor;
        p_topLeft.p_y *= factor;
        p_bottomRight.p_x *= factor;
        p_bottomRight.p_y *= factor;
        return *this;
    }

    inline LRect &operator/=(Int32 factor)
    {
        p_topLeft.p_x /= factor;
        p_topLeft.p_y /= factor;
        p_bottomRight.p_x /= factor;
        p_bottomRight.p_y /= factor;
        return *this;
    }

    inline LRect &operator+=(double factor)
    {
        p_topLeft.p_x += factor;
        p_topLeft.p_y += factor;
        p_bottomRight.p_x += factor;
        p_bottomRight.p_y += factor;
        return *this;
    }

    inline LRect &operator-=(double factor)
    {
        p_topLeft.p_x -= factor;
        p_topLeft.p_y -= factor;
        p_bottomRight.p_x -= factor;
        p_bottomRight.p_y -= factor;
        return *this;
    }

    inline LRect &operator*=(double factor)
    {
        p_topLeft.p_x *= factor;
        p_topLeft.p_y *= factor;
        p_bottomRight.p_x *= factor;
        p_bottomRight.p_y *= factor;
        return *this;
    }

    inline LRect &operator/=(double factor)
    {
        p_topLeft.p_x /= factor;
        p_topLeft.p_y /= factor;
        p_bottomRight.p_x /= factor;
        p_bottomRight.p_y /= factor;
        return *this;
    }

    inline LRect &operator+=(const LRect &r)
    {
        p_topLeft.p_x += r.p_topLeft.p_x;
        p_topLeft.p_y += r.p_topLeft.p_y;
        p_bottomRight.p_x += r.p_bottomRight.p_x;
        p_bottomRight.p_y += r.p_bottomRight.p_y;
        return *this;
    }

    inline LRect &operator-=(const LRect &r)
    {
        p_topLeft.p_x -= r.p_topLeft.p_x;
        p_topLeft.p_y -= r.p_topLeft.p_y;
        p_bottomRight.p_x -= r.p_bottomRight.p_x;
        p_bottomRight.p_y -= r.p_bottomRight.p_y;
        return *this;
    }

    inline LRect &operator*=(const LRect &r)
    {
        p_topLeft.p_x *= r.p_topLeft.p_x;
        p_topLeft.p_y *= r.p_topLeft.p_y;
        p_bottomRight.p_x *= r.p_bottomRight.p_x;
        p_bottomRight.p_y *= r.p_bottomRight.p_y;
        return *this;
    }

    inline LRect &operator/=(const LRect &r)
    {
        p_topLeft.p_x /= r.p_topLeft.p_x;
        p_topLeft.p_y /= r.p_topLeft.p_y;
        p_bottomRight.p_x /= r.p_bottomRight.p_x;
        p_bottomRight.p_y /= r.p_bottomRight.p_y;
        return *this;
    }

    inline LRect operator+(Int32 factor) const
    {
        return LRect(p_topLeft.p_x+factor,p_topLeft.p_y+factor,p_bottomRight.p_x+factor,p_bottomRight.p_y+factor);
    }

    inline LRect operator-(Int32 factor) const
    {
        return LRect(p_topLeft.p_x-factor,p_topLeft.p_y-factor,p_bottomRight.p_x-factor,p_bottomRight.p_y-factor);
    }

    inline LRect operator*(Int32 factor) const
    {
        return LRect(p_topLeft.p_x*factor,p_topLeft.p_y*factor,p_bottomRight.p_x*factor,p_bottomRight.p_y*factor);
    }

    inline LRect operator/(Int32 factor) const
    {
        return LRect(p_topLeft.p_x/factor,p_topLeft.p_y/factor,p_bottomRight.p_x/factor,p_bottomRight.p_y/factor);
    }

    inline LRect operator+(double factor) const
    {
        return LRect(p_topLeft.p_x+factor,p_topLeft.p_y+factor,p_bottomRight.p_x+factor,p_bottomRight.p_y+factor);
    }

    inline LRect operator-(double factor) const
    {
        return LRect(p_topLeft.p_x-factor,p_topLeft.p_y-factor,p_bottomRight.p_x-factor,p_bottomRight.p_y-factor);
    }

    inline LRect operator*(double factor) const
    {
        return LRect(p_topLeft.p_x*factor,p_topLeft.p_y*factor,p_bottomRight.p_x*factor,p_bottomRight.p_y*factor);
    }

    inline LRect operator/(double factor) const
    {
        return LRect(p_topLeft.p_x/factor,p_topLeft.p_y/factor,p_bottomRight.p_x/factor,p_bottomRight.p_y/factor);
    }

    inline LRect operator+(const LRect &r) const
    {
        return LRect(p_topLeft.p_x+r.p_topLeft.p_x,p_topLeft.p_y+r.p_topLeft.p_y,p_bottomRight.p_x+r.p_bottomRight.p_x,p_bottomRight.p_y+r.p_bottomRight.p_y);
    }

    inline LRect operator-(const LRect &r) const
    {
        return LRect(p_topLeft.p_x-r.p_topLeft.p_x,p_topLeft.p_y-r.p_topLeft.p_y,p_bottomRight.p_x-r.p_bottomRight.p_x,p_bottomRight.p_y-r.p_bottomRight.p_y);
    }

    inline LRect operator*(const LRect &r) const
    {
        return LRect(p_topLeft.p_x*r.p_topLeft.p_x,p_topLeft.p_y*r.p_topLeft.p_y,p_bottomRight.p_x*r.p_bottomRight.p_x,p_bottomRight.p_y*r.p_bottomRight.p_y);
    }

    inline LRect operator/(const LRect &r) const
    {
        return LRect(p_topLeft.p_x/r.p_topLeft.p_x,p_topLeft.p_y/r.p_topLeft.p_y,p_bottomRight.p_x/r.p_bottomRight.p_x,p_bottomRight.p_y/r.p_bottomRight.p_y);
    }

    inline bool operator==(const LRect &p) const
    {
        return p_topLeft.p_x == p.p_topLeft.p_x && p_topLeft.p_y == p.p_topLeft.p_y && p_bottomRight.p_x == p.p_bottomRight.p_x && p_bottomRight.p_y == p.p_bottomRight.p_y;
    }

    inline bool operator!=(const LRect &p) const
    {
        return p_topLeft.p_x != p.p_topLeft.p_x || p_topLeft.p_y != p.p_topLeft.p_y || p_bottomRight.p_x != p.p_bottomRight.p_x || p_bottomRight.p_y != p.p_bottomRight.p_y;
    }

private:

    LPoint p_topLeft,p_bottomRight;
};


#endif // LRECT_H
