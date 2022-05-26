#ifndef WPOINT_H
#define WPOINT_H

#include <WNamespaces.h>

class Wpp::WPoint
{
public:

    // Constructors
    inline WPoint() : p_x(0), p_y(0) {}
    inline WPoint(Int32 x, Int32 y) : p_x(x), p_y(y){}
    inline WPoint(double x, double y) : p_x(x), p_y(y){}
    WPoint(const WPointF &pointF);

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

    WPoint constrainedToHeight(UInt32 size) const;

    inline WPoint &operator+=(Int32 factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline WPoint &operator-=(Int32 factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline WPoint &operator*=(Int32 factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline WPoint &operator/=(Int32 factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline WPoint &operator+=(double factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline WPoint &operator-=(double factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline WPoint &operator*=(double factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline WPoint &operator/=(double factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline WPoint &operator+=(const WPoint &p)
    {
        p_x += p.p_x;
        p_y += p.p_y;
        return *this;
    }

    inline WPoint &operator-=(const WPoint &p)
    {
        p_x -= p.p_x;
        p_y -= p.p_y;
        return *this;
    }

    inline WPoint &operator*=(const WPoint &p)
    {
        p_x *= p.p_x;
        p_y *= p.p_y;
        return *this;
    }

    inline WPoint &operator/=(const WPoint &p)
    {
        p_x /= p.p_x;
        p_y /= p.p_y;
        return *this;
    }

    WPoint &operator+=(const WPointF &p);
    WPoint &operator-=(const WPointF &p);
    WPoint &operator*=(const WPointF &p);
    WPoint &operator/=(const WPointF &p);


    inline WPoint operator+(Int32 factor) const
    {
        return WPoint(p_x+factor,p_y+factor);
    }

    inline WPoint operator-(Int32 factor) const
    {
        return WPoint(p_x-factor,p_y-factor);
    }

    inline WPoint operator*(Int32 factor) const
    {
        return WPoint(p_x*factor,p_y*factor);
    }

    inline WPoint operator/(Int32 factor) const
    {
        return WPoint(p_x/factor,p_y/factor);
    }

    inline WPoint operator+(double factor) const
    {
        return WPoint(p_x+factor,p_y+factor);
    }

    inline WPoint operator-(double factor) const
    {
        return WPoint(p_x-factor,p_y-factor);
    }

    inline WPoint operator*(double factor) const
    {
        return WPoint(p_x*factor,p_y*factor);
    }

    inline WPoint operator/(double factor) const
    {
        return WPoint(p_x/factor,p_y/factor);
    }

    inline WPoint operator+(const WPoint &p) const
    {
        return WPoint(p_x+p.p_x,p_y+p.p_y);
    }

    inline WPoint operator-(const WPoint &p) const
    {
        return WPoint(p_x-p.p_x,p_y-p.p_y);
    }

    inline WPoint operator*(const WPoint &p) const
    {
        return WPoint(p_x*p.p_x,p_y*p.p_y);
    }

    inline WPoint operator/(const WPoint &p) const
    {
        return WPoint(p_x/p.p_x,p_y/p.p_y);
    }

    WPoint operator+(const WPointF &p) const;
    WPoint operator-(const WPointF &p) const;
    WPoint operator*(const WPointF &p) const;
    WPoint operator/(const WPointF &p) const;


    inline bool operator==(const WPoint &p) const
    {
        return p_x == p.p_x && p_y == p.p_y;
    }

    inline bool operator!=(const WPoint &p) const
    {
        return p_x != p.p_x || p_y != p.p_y;
    }

private:
    friend class WRect;
    friend class WPointF;
    Int32 p_x,p_y;
};

#endif // WPOINT_H
