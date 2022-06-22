#ifndef LPOINT_H
#define LPOINT_H

#include <LNamespaces.h>

class Louvre::LPoint
{
public:

    // Constructors
    inline LPoint() : m_x(0), m_y(0) {}
    inline LPoint(Int32 x, Int32 y) : m_x(x), m_y(y){}
    inline LPoint(double x, double y) : m_x(x), m_y(y){}
    LPoint(const LPointF &pointF);

    Int32 x()       const   {return m_x;}
    Int32 y()       const   {return m_y;}
    Int32 w()       const   {return m_x;}
    Int32 h()       const   {return m_y;}
    Int32 width()   const   {return m_x;}
    Int32 height()  const   {return m_y;}
    Int32 area()    const   {return m_x*m_y;}

    void setX(Int32 x){m_x = x;}
    void setY(Int32 y){m_y = y;}
    void setW(Int32 x){m_x = x;}
    void setH(Int32 y){m_y = y;}
    void setWidth(Int32 x){m_x = x;}
    void setHeight(Int32 y){m_y = y;}

    LPoint constrainedToHeight(UInt32 size) const;

    inline LPoint &operator+=(Int32 factor)
    {
        m_x += factor;
        m_y += factor;
        return *this;
    }

    inline LPoint &operator-=(Int32 factor)
    {
        m_x -= factor;
        m_y -= factor;
        return *this;
    }

    inline LPoint &operator*=(Int32 factor)
    {
        m_x *= factor;
        m_y *= factor;
        return *this;
    }

    inline LPoint &operator/=(Int32 factor)
    {
        m_x /= factor;
        m_y /= factor;
        return *this;
    }

    inline LPoint &operator+=(double factor)
    {
        m_x += factor;
        m_y += factor;
        return *this;
    }

    inline LPoint &operator-=(double factor)
    {
        m_x -= factor;
        m_y -= factor;
        return *this;
    }

    inline LPoint &operator*=(double factor)
    {
        m_x *= factor;
        m_y *= factor;
        return *this;
    }

    inline LPoint &operator/=(double factor)
    {
        m_x /= factor;
        m_y /= factor;
        return *this;
    }

    inline LPoint &operator+=(const LPoint &p)
    {
        m_x += p.m_x;
        m_y += p.m_y;
        return *this;
    }

    inline LPoint &operator-=(const LPoint &p)
    {
        m_x -= p.m_x;
        m_y -= p.m_y;
        return *this;
    }

    inline LPoint &operator*=(const LPoint &p)
    {
        m_x *= p.m_x;
        m_y *= p.m_y;
        return *this;
    }

    inline LPoint &operator/=(const LPoint &p)
    {
        m_x /= p.m_x;
        m_y /= p.m_y;
        return *this;
    }

    LPoint &operator+=(const LPointF &p);
    LPoint &operator-=(const LPointF &p);
    LPoint &operator*=(const LPointF &p);
    LPoint &operator/=(const LPointF &p);


    inline LPoint operator+(Int32 factor) const
    {
        return LPoint(m_x+factor,m_y+factor);
    }

    inline LPoint operator-(Int32 factor) const
    {
        return LPoint(m_x-factor,m_y-factor);
    }

    inline LPoint operator*(Int32 factor) const
    {
        return LPoint(m_x*factor,m_y*factor);
    }

    inline LPoint operator/(Int32 factor) const
    {
        return LPoint(m_x/factor,m_y/factor);
    }

    inline LPoint operator+(double factor) const
    {
        return LPoint(m_x+factor,m_y+factor);
    }

    inline LPoint operator-(double factor) const
    {
        return LPoint(m_x-factor,m_y-factor);
    }

    inline LPoint operator*(double factor) const
    {
        return LPoint(m_x*factor,m_y*factor);
    }

    inline LPoint operator/(double factor) const
    {
        return LPoint(m_x/factor,m_y/factor);
    }

    inline LPoint operator+(const LPoint &p) const
    {
        return LPoint(m_x+p.m_x,m_y+p.m_y);
    }

    inline LPoint operator-(const LPoint &p) const
    {
        return LPoint(m_x-p.m_x,m_y-p.m_y);
    }

    inline LPoint operator*(const LPoint &p) const
    {
        return LPoint(m_x*p.m_x,m_y*p.m_y);
    }

    inline LPoint operator/(const LPoint &p) const
    {
        return LPoint(m_x/p.m_x,m_y/p.m_y);
    }

    LPoint operator+(const LPointF &p) const;
    LPoint operator-(const LPointF &p) const;
    LPoint operator*(const LPointF &p) const;
    LPoint operator/(const LPointF &p) const;


    inline bool operator==(const LPoint &p) const
    {
        return m_x == p.m_x && m_y == p.m_y;
    }

    inline bool operator!=(const LPoint &p) const
    {
        return m_x != p.m_x || m_y != p.m_y;
    }

private:
    friend class LRect;
    friend class LPointF;
    Int32 m_x = 0;
    Int32 m_y = 0;
};

#endif // LPOINT_H
