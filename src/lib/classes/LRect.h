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
        m_topLeft.m_x       = x;
        m_topLeft.m_y       = y;
        m_bottomRight.m_x   = width;
        m_bottomRight.m_y   = height;
    }
    inline LRect(const LPoint &topLeft, const LPoint &bottomRight) : m_topLeft(topLeft), m_bottomRight(bottomRight){}

    Int32 x()       const   {return m_topLeft.m_x;}
    Int32 y()       const   {return m_topLeft.m_y;}
    Int32 w()       const   {return m_bottomRight.m_x;}
    Int32 h()       const   {return m_bottomRight.m_y;}
    Int32 width()   const   {return m_bottomRight.m_x;}
    Int32 height()  const   {return m_bottomRight.m_y;}
    Int32 area()    const   {return m_bottomRight.m_x*m_bottomRight.m_y;}

    bool containsPoint(const LPoint &point, bool inclusive = true) const
    {
        if(inclusive)
        {
            return point.m_x >= m_topLeft.m_x &&
                   point.m_x <= m_topLeft.m_x + m_bottomRight.m_x &&
                   point.m_y >= m_topLeft.m_y &&
                   point.m_y <= m_topLeft.m_y + m_bottomRight.m_y;
        }

        return point.m_x > m_topLeft.m_x &&
               point.m_x < m_topLeft.m_x + m_bottomRight.m_x &&
               point.m_y > m_topLeft.m_y &&
               point.m_y < m_topLeft.m_y + m_bottomRight.m_y;
    }

    LPoint topLeft()    const   {return m_topLeft;}
    LSize bottomRight() const   {return m_bottomRight;}

    void setX(Int32 x){m_topLeft.m_x = x;}
    void setY(Int32 y){m_topLeft.m_y = y;}
    void setW(Int32 width){m_bottomRight.m_x = width;}
    void setH(Int32 height){m_bottomRight.m_y = height;}
    void setWidth(Int32 width){m_bottomRight.m_x = width;}
    void setHeight(Int32 height){m_bottomRight.m_y = height;}

    // Retorna true si la recta resultante tiene area 0
    bool clip(const LRect &rect)
    {
        Int32 x0 = x();
        Int32 x1 = x0 + w();
        Int32 y0 = y();
        Int32 y1 = y0 + h();

        Int32 rx1 = rect.x() + rect.w();
        Int32 ry1 = rect.y() + rect.h();

        // X
        if(rect.x() > x0)
            x0 = rect.x();
        else if(rx1 < x0)
            x0 = rx1;

        // W
        if(rect.x() > x1)
            x1 = rect.x();
        else if(rx1 < x1)
            x1 = rx1;

        // Y
        if(rect.y() > y0)
            y0 = rect.y();
        else if(ry1 < y0)
            y0 = ry1;

        // H
        if(rect.y() > y1)
            y1 = rect.y();
        else if(ry1 < y1)
            y1 = ry1;

        setX(x0);
        setY(y0);
        setW(x1 - x0);
        setH(y1 - y0);

        return (w() == 0 || h() == 0 );
    }

    inline LRect &operator+=(Int32 factor)
    {
        m_topLeft.m_x += factor;
        m_topLeft.m_y += factor;
        m_bottomRight.m_x += factor;
        m_bottomRight.m_y += factor;
        return *this;
    }

    inline LRect &operator-=(Int32 factor)
    {
        m_topLeft.m_x -= factor;
        m_topLeft.m_y -= factor;
        m_bottomRight.m_x -= factor;
        m_bottomRight.m_y -= factor;
        return *this;
    }

    inline LRect &operator*=(Int32 factor)
    {
        m_topLeft.m_x *= factor;
        m_topLeft.m_y *= factor;
        m_bottomRight.m_x *= factor;
        m_bottomRight.m_y *= factor;
        return *this;
    }

    inline LRect &operator/=(Int32 factor)
    {
        m_topLeft.m_x /= factor;
        m_topLeft.m_y /= factor;
        m_bottomRight.m_x /= factor;
        m_bottomRight.m_y /= factor;
        return *this;
    }

    inline LRect &operator+=(double factor)
    {
        m_topLeft.m_x += factor;
        m_topLeft.m_y += factor;
        m_bottomRight.m_x += factor;
        m_bottomRight.m_y += factor;
        return *this;
    }

    inline LRect &operator-=(double factor)
    {
        m_topLeft.m_x -= factor;
        m_topLeft.m_y -= factor;
        m_bottomRight.m_x -= factor;
        m_bottomRight.m_y -= factor;
        return *this;
    }

    inline LRect &operator*=(double factor)
    {
        m_topLeft.m_x *= factor;
        m_topLeft.m_y *= factor;
        m_bottomRight.m_x *= factor;
        m_bottomRight.m_y *= factor;
        return *this;
    }

    inline LRect &operator/=(double factor)
    {
        m_topLeft.m_x /= factor;
        m_topLeft.m_y /= factor;
        m_bottomRight.m_x /= factor;
        m_bottomRight.m_y /= factor;
        return *this;
    }

    inline LRect &operator+=(const LRect &r)
    {
        m_topLeft.m_x += r.m_topLeft.m_x;
        m_topLeft.m_y += r.m_topLeft.m_y;
        m_bottomRight.m_x += r.m_bottomRight.m_x;
        m_bottomRight.m_y += r.m_bottomRight.m_y;
        return *this;
    }

    inline LRect &operator-=(const LRect &r)
    {
        m_topLeft.m_x -= r.m_topLeft.m_x;
        m_topLeft.m_y -= r.m_topLeft.m_y;
        m_bottomRight.m_x -= r.m_bottomRight.m_x;
        m_bottomRight.m_y -= r.m_bottomRight.m_y;
        return *this;
    }

    inline LRect &operator*=(const LRect &r)
    {
        m_topLeft.m_x *= r.m_topLeft.m_x;
        m_topLeft.m_y *= r.m_topLeft.m_y;
        m_bottomRight.m_x *= r.m_bottomRight.m_x;
        m_bottomRight.m_y *= r.m_bottomRight.m_y;
        return *this;
    }

    inline LRect &operator/=(const LRect &r)
    {
        m_topLeft.m_x /= r.m_topLeft.m_x;
        m_topLeft.m_y /= r.m_topLeft.m_y;
        m_bottomRight.m_x /= r.m_bottomRight.m_x;
        m_bottomRight.m_y /= r.m_bottomRight.m_y;
        return *this;
    }

    inline LRect operator+(Int32 factor) const
    {
        return LRect(m_topLeft.m_x+factor,m_topLeft.m_y+factor,m_bottomRight.m_x+factor,m_bottomRight.m_y+factor);
    }

    inline LRect operator-(Int32 factor) const
    {
        return LRect(m_topLeft.m_x-factor,m_topLeft.m_y-factor,m_bottomRight.m_x-factor,m_bottomRight.m_y-factor);
    }

    inline LRect operator*(Int32 factor) const
    {
        return LRect(m_topLeft.m_x*factor,m_topLeft.m_y*factor,m_bottomRight.m_x*factor,m_bottomRight.m_y*factor);
    }

    inline LRect operator/(Int32 factor) const
    {
        return LRect(m_topLeft.m_x/factor,m_topLeft.m_y/factor,m_bottomRight.m_x/factor,m_bottomRight.m_y/factor);
    }

    inline LRect operator+(double factor) const
    {
        return LRect(m_topLeft.m_x+factor,m_topLeft.m_y+factor,m_bottomRight.m_x+factor,m_bottomRight.m_y+factor);
    }

    inline LRect operator-(double factor) const
    {
        return LRect(m_topLeft.m_x-factor,m_topLeft.m_y-factor,m_bottomRight.m_x-factor,m_bottomRight.m_y-factor);
    }

    inline LRect operator*(double factor) const
    {
        return LRect(m_topLeft.m_x*factor,m_topLeft.m_y*factor,m_bottomRight.m_x*factor,m_bottomRight.m_y*factor);
    }

    inline LRect operator/(double factor) const
    {
        return LRect(m_topLeft.m_x/factor,m_topLeft.m_y/factor,m_bottomRight.m_x/factor,m_bottomRight.m_y/factor);
    }

    inline LRect operator+(const LRect &r) const
    {
        return LRect(m_topLeft.m_x+r.m_topLeft.m_x,m_topLeft.m_y+r.m_topLeft.m_y,m_bottomRight.m_x+r.m_bottomRight.m_x,m_bottomRight.m_y+r.m_bottomRight.m_y);
    }

    inline LRect operator-(const LRect &r) const
    {
        return LRect(m_topLeft.m_x-r.m_topLeft.m_x,m_topLeft.m_y-r.m_topLeft.m_y,m_bottomRight.m_x-r.m_bottomRight.m_x,m_bottomRight.m_y-r.m_bottomRight.m_y);
    }

    inline LRect operator*(const LRect &r) const
    {
        return LRect(m_topLeft.m_x*r.m_topLeft.m_x,m_topLeft.m_y*r.m_topLeft.m_y,m_bottomRight.m_x*r.m_bottomRight.m_x,m_bottomRight.m_y*r.m_bottomRight.m_y);
    }

    inline LRect operator/(const LRect &r) const
    {
        return LRect(m_topLeft.m_x/r.m_topLeft.m_x,m_topLeft.m_y/r.m_topLeft.m_y,m_bottomRight.m_x/r.m_bottomRight.m_x,m_bottomRight.m_y/r.m_bottomRight.m_y);
    }

    inline bool operator==(const LRect &p) const
    {
        return m_topLeft.m_x == p.m_topLeft.m_x && m_topLeft.m_y == p.m_topLeft.m_y && m_bottomRight.m_x == p.m_bottomRight.m_x && m_bottomRight.m_y == p.m_bottomRight.m_y;
    }

    inline bool operator!=(const LRect &p) const
    {
        return m_topLeft.m_x != p.m_topLeft.m_x || m_topLeft.m_y != p.m_topLeft.m_y || m_bottomRight.m_x != p.m_bottomRight.m_x || m_bottomRight.m_y != p.m_bottomRight.m_y;
    }

private:

    LPoint m_topLeft,m_bottomRight;
};


#endif // LRECT_H
