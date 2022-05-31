#ifndef LPointFF_H
#define LPointFF_H

#include <LNamespaces.h>

class Louvre::LPointF
{
public:

    // Constructors
    inline LPointF() : p_x(0), p_y(0) {}
    inline LPointF(double x, double y) : p_x(x), p_y(y){}
    LPointF(const LPoint &point);

    float x()       const   {return p_x;}
    float y()       const   {return p_y;}
    float w()       const   {return p_x;}
    float h()       const   {return p_y;}
    float width()   const   {return p_x;}
    float height()  const   {return p_y;}
    float area()    const   {return p_x*p_y;}

    void setX(Int32 x){p_x = x;}
    void setY(Int32 y){p_y = y;}
    void setW(Int32 x){p_x = x;}
    void setH(Int32 y){p_y = y;}
    void setWidth(Int32 x){p_x = x;}
    void setHeight(Int32 y){p_y = y;}

    inline LPointF &operator+=(Int32 factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline LPointF &operator-=(Int32 factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline LPointF &operator*=(Int32 factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline LPointF &operator/=(Int32 factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline LPointF &operator+=(double factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline LPointF &operator-=(double factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline LPointF &operator*=(double factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline LPointF &operator/=(double factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline LPointF &operator+=(const LPointF &p)
    {
        p_x += p.p_x;
        p_y += p.p_y;
        return *this;
    }

    inline LPointF &operator-=(const LPointF &p)
    {
        p_x -= p.p_x;
        p_y -= p.p_y;
        return *this;
    }

    inline LPointF &operator*=(const LPointF &p)
    {
        p_x *= p.p_x;
        p_y *= p.p_y;
        return *this;
    }

    inline LPointF &operator/=(const LPointF &p)
    {
        p_x /= p.p_x;
        p_y /= p.p_y;
        return *this;
    }

    LPointF &operator+=(const LPoint &p);
    LPointF &operator-=(const LPoint &p);
    LPointF &operator*=(const LPoint &p);
    LPointF &operator/=(const LPoint &p);

    inline LPointF operator+(Int32 factor) const
    {
        return LPointF(p_x+factor,p_y+factor);
    }

    inline LPointF operator-(Int32 factor) const
    {
        return LPointF(p_x-factor,p_y-factor);
    }

    inline LPointF operator*(Int32 factor) const
    {
        return LPointF(p_x*factor,p_y*factor);
    }

    inline LPointF operator/(Int32 factor) const
    {
        return LPointF(p_x/factor,p_y/factor);
    }

    inline LPointF operator+(double factor) const
    {
        return LPointF(p_x+factor,p_y+factor);
    }

    inline LPointF operator-(double factor) const
    {
        return LPointF(p_x-factor,p_y-factor);
    }

    inline LPointF operator*(double factor) const
    {
        return LPointF(p_x*factor,p_y*factor);
    }

    inline LPointF operator/(double factor) const
    {
        return LPointF(p_x/factor,p_y/factor);
    }

    inline LPointF operator+(const LPointF &p) const
    {
        return LPointF(p_x+p.p_x,p_y+p.p_y);
    }

    inline LPointF operator-(const LPointF &p) const
    {
        return LPointF(p_x-p.p_x,p_y-p.p_y);
    }

    inline LPointF operator*(const LPointF &p) const
    {
        return LPointF(p_x*p.p_x,p_y*p.p_y);
    }

    inline LPointF operator/(const LPointF &p) const
    {
        return LPointF(p_x*p.p_x,p_y*p.p_y);
    }

    LPointF operator+(const LPoint &p) const;
    LPointF operator-(const LPoint &p) const;
    LPointF operator*(const LPoint &p) const;
    LPointF operator/(const LPoint &p) const;

    inline bool operator==(const LPointF &p) const
    {
        return p_x == p.p_x && p_y == p.p_y;
    }

    inline bool operator!=(const LPointF &p) const
    {
        return p_x != p.p_x || p_y != p.p_y;
    }

private:
    friend class LRect;
    friend class LPoint;
    float p_x,p_y;
};
#endif // LPointFF_H
