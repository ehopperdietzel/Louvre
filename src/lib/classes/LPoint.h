#ifndef LPOINT_H
#define LPOINT_H

#include <LNamespaces.h>

class Louvre::LPoint
{
public:

    // Constructors
    inline LPoint() : p_x(0), p_y(0) {}
    inline LPoint(Int32 x, Int32 y) : p_x(x), p_y(y){}
    inline LPoint(double x, double y) : p_x(x), p_y(y){}
    LPoint(const LPointF &pointF);

    Int32 x()       const   {return p_x;}
    Int32 y()       const   {return p_y;}
    Int32 w()       const   {return p_x;}
    Int32 h()       const   {return p_y;}
    Int32 width()   const   {return p_x;}
    Int32 height()  const   {return p_y;}
    Int32 area()    const   {return p_x*p_y;}

    void setX(Int32 x){p_x = x;}
    void setY(Int32 y){p_y = y;}
    void setW(Int32 x){p_x = x;}
    void setH(Int32 y){p_y = y;}
    void setWidth(Int32 x){p_x = x;}
    void setHeight(Int32 y){p_y = y;}

    LPoint constrainedToHeight(UInt32 size) const;

    inline LPoint &operator+=(Int32 factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline LPoint &operator-=(Int32 factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline LPoint &operator*=(Int32 factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline LPoint &operator/=(Int32 factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline LPoint &operator+=(double factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline LPoint &operator-=(double factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline LPoint &operator*=(double factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline LPoint &operator/=(double factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline LPoint &operator+=(const LPoint &p)
    {
        p_x += p.p_x;
        p_y += p.p_y;
        return *this;
    }

    inline LPoint &operator-=(const LPoint &p)
    {
        p_x -= p.p_x;
        p_y -= p.p_y;
        return *this;
    }

    inline LPoint &operator*=(const LPoint &p)
    {
        p_x *= p.p_x;
        p_y *= p.p_y;
        return *this;
    }

    inline LPoint &operator/=(const LPoint &p)
    {
        p_x /= p.p_x;
        p_y /= p.p_y;
        return *this;
    }

    LPoint &operator+=(const LPointF &p);
    LPoint &operator-=(const LPointF &p);
    LPoint &operator*=(const LPointF &p);
    LPoint &operator/=(const LPointF &p);


    inline LPoint operator+(Int32 factor) const
    {
        return LPoint(p_x+factor,p_y+factor);
    }

    inline LPoint operator-(Int32 factor) const
    {
        return LPoint(p_x-factor,p_y-factor);
    }

    inline LPoint operator*(Int32 factor) const
    {
        return LPoint(p_x*factor,p_y*factor);
    }

    inline LPoint operator/(Int32 factor) const
    {
        return LPoint(p_x/factor,p_y/factor);
    }

    inline LPoint operator+(double factor) const
    {
        return LPoint(p_x+factor,p_y+factor);
    }

    inline LPoint operator-(double factor) const
    {
        return LPoint(p_x-factor,p_y-factor);
    }

    inline LPoint operator*(double factor) const
    {
        return LPoint(p_x*factor,p_y*factor);
    }

    inline LPoint operator/(double factor) const
    {
        return LPoint(p_x/factor,p_y/factor);
    }

    inline LPoint operator+(const LPoint &p) const
    {
        return LPoint(p_x+p.p_x,p_y+p.p_y);
    }

    inline LPoint operator-(const LPoint &p) const
    {
        return LPoint(p_x-p.p_x,p_y-p.p_y);
    }

    inline LPoint operator*(const LPoint &p) const
    {
        return LPoint(p_x*p.p_x,p_y*p.p_y);
    }

    inline LPoint operator/(const LPoint &p) const
    {
        return LPoint(p_x/p.p_x,p_y/p.p_y);
    }

    LPoint operator+(const LPointF &p) const;
    LPoint operator-(const LPointF &p) const;
    LPoint operator*(const LPointF &p) const;
    LPoint operator/(const LPointF &p) const;


    inline bool operator==(const LPoint &p) const
    {
        return p_x == p.p_x && p_y == p.p_y;
    }

    inline bool operator!=(const LPoint &p) const
    {
        return p_x != p.p_x || p_y != p.p_y;
    }

private:
    friend class LRect;
    friend class LPointF;
    Int32 p_x = 0;
    Int32 p_y = 0;
};

#endif // LPOINT_H
