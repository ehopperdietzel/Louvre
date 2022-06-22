#ifndef LPointFF_H
#define LPointFF_H

#include <LNamespaces.h>

class Louvre::LPointF
{
public:

    // Constructors
    inline LPointF() : m_x(0), m_y(0) {}
    inline LPointF(double x, double y) : m_x(x), m_y(y){}
    LPointF(const LPoint &point);

    float x()       const   {return m_x;}
    float y()       const   {return m_y;}
    float w()       const   {return m_x;}
    float h()       const   {return m_y;}
    float width()   const   {return m_x;}
    float height()  const   {return m_y;}
    float area()    const   {return m_x*m_y;}

    void setX(Int32 x){m_x = x;}
    void setY(Int32 y){m_y = y;}
    void setW(Int32 x){m_x = x;}
    void setH(Int32 y){m_y = y;}
    void setWidth(Int32 x){m_x = x;}
    void setHeight(Int32 y){m_y = y;}

    inline LPointF &operator+=(Int32 factor)
    {
        m_x += factor;
        m_y += factor;
        return *this;
    }

    inline LPointF &operator-=(Int32 factor)
    {
        m_x -= factor;
        m_y -= factor;
        return *this;
    }

    inline LPointF &operator*=(Int32 factor)
    {
        m_x *= factor;
        m_y *= factor;
        return *this;
    }

    inline LPointF &operator/=(Int32 factor)
    {
        m_x /= factor;
        m_y /= factor;
        return *this;
    }

    inline LPointF &operator+=(double factor)
    {
        m_x += factor;
        m_y += factor;
        return *this;
    }

    inline LPointF &operator-=(double factor)
    {
        m_x -= factor;
        m_y -= factor;
        return *this;
    }

    inline LPointF &operator*=(double factor)
    {
        m_x *= factor;
        m_y *= factor;
        return *this;
    }

    inline LPointF &operator/=(double factor)
    {
        m_x /= factor;
        m_y /= factor;
        return *this;
    }

    inline LPointF &operator+=(const LPointF &p)
    {
        m_x += p.m_x;
        m_y += p.m_y;
        return *this;
    }

    inline LPointF &operator-=(const LPointF &p)
    {
        m_x -= p.m_x;
        m_y -= p.m_y;
        return *this;
    }

    inline LPointF &operator*=(const LPointF &p)
    {
        m_x *= p.m_x;
        m_y *= p.m_y;
        return *this;
    }

    inline LPointF &operator/=(const LPointF &p)
    {
        m_x /= p.m_x;
        m_y /= p.m_y;
        return *this;
    }

    LPointF &operator+=(const LPoint &p);
    LPointF &operator-=(const LPoint &p);
    LPointF &operator*=(const LPoint &p);
    LPointF &operator/=(const LPoint &p);

    inline LPointF operator+(Int32 factor) const
    {
        return LPointF(m_x+factor,m_y+factor);
    }

    inline LPointF operator-(Int32 factor) const
    {
        return LPointF(m_x-factor,m_y-factor);
    }

    inline LPointF operator*(Int32 factor) const
    {
        return LPointF(m_x*factor,m_y*factor);
    }

    inline LPointF operator/(Int32 factor) const
    {
        return LPointF(m_x/factor,m_y/factor);
    }

    inline LPointF operator+(double factor) const
    {
        return LPointF(m_x+factor,m_y+factor);
    }

    inline LPointF operator-(double factor) const
    {
        return LPointF(m_x-factor,m_y-factor);
    }

    inline LPointF operator*(double factor) const
    {
        return LPointF(m_x*factor,m_y*factor);
    }

    inline LPointF operator/(double factor) const
    {
        return LPointF(m_x/factor,m_y/factor);
    }

    inline LPointF operator+(const LPointF &p) const
    {
        return LPointF(m_x+p.m_x,m_y+p.m_y);
    }

    inline LPointF operator-(const LPointF &p) const
    {
        return LPointF(m_x-p.m_x,m_y-p.m_y);
    }

    inline LPointF operator*(const LPointF &p) const
    {
        return LPointF(m_x*p.m_x,m_y*p.m_y);
    }

    inline LPointF operator/(const LPointF &p) const
    {
        return LPointF(m_x*p.m_x,m_y*p.m_y);
    }

    LPointF operator+(const LPoint &p) const;
    LPointF operator-(const LPoint &p) const;
    LPointF operator*(const LPoint &p) const;
    LPointF operator/(const LPoint &p) const;

    inline bool operator==(const LPointF &p) const
    {
        return m_x == p.m_x && m_y == p.m_y;
    }

    inline bool operator!=(const LPointF &p) const
    {
        return m_x != p.m_x || m_y != p.m_y;
    }

private:
    friend class LRect;
    friend class LPoint;
    float m_x,m_y;
};
#endif // LPointFF_H
