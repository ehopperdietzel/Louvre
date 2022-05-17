#ifndef WPointFF_H
#define WPointFF_H

#include <WNamespaces.h>

class Wpp::WPointF
{
public:

    // Constructors
    inline WPointF() : p_x(0), p_y(0) {}
    inline WPointF(double x, double y) : p_x(x), p_y(y){}
    WPointF(const WPoint &point);

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

    inline WPointF &operator+=(Int32 factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline WPointF &operator-=(Int32 factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline WPointF &operator*=(Int32 factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline WPointF &operator/=(Int32 factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline WPointF &operator+=(double factor)
    {
        p_x += factor;
        p_y += factor;
        return *this;
    }

    inline WPointF &operator-=(double factor)
    {
        p_x -= factor;
        p_y -= factor;
        return *this;
    }

    inline WPointF &operator*=(double factor)
    {
        p_x *= factor;
        p_y *= factor;
        return *this;
    }

    inline WPointF &operator/=(double factor)
    {
        p_x /= factor;
        p_y /= factor;
        return *this;
    }

    inline WPointF &operator+=(const WPointF &p)
    {
        p_x += p.p_x;
        p_y += p.p_y;
        return *this;
    }

    inline WPointF &operator-=(const WPointF &p)
    {
        p_x -= p.p_x;
        p_y -= p.p_y;
        return *this;
    }

    inline WPointF &operator*=(const WPointF &p)
    {
        p_x *= p.p_x;
        p_y *= p.p_y;
        return *this;
    }

    inline WPointF &operator/=(const WPointF &p)
    {
        p_x /= p.p_x;
        p_y /= p.p_y;
        return *this;
    }

    WPointF &operator+=(const WPoint &p);
    WPointF &operator-=(const WPoint &p);
    WPointF &operator*=(const WPoint &p);
    WPointF &operator/=(const WPoint &p);

    inline WPointF operator+(Int32 factor) const
    {
        return WPointF(p_x+factor,p_y+factor);
    }

    inline WPointF operator-(Int32 factor) const
    {
        return WPointF(p_x-factor,p_y-factor);
    }

    inline WPointF operator*(Int32 factor) const
    {
        return WPointF(p_x*factor,p_y*factor);
    }

    inline WPointF operator/(Int32 factor) const
    {
        return WPointF(p_x/factor,p_y/factor);
    }

    inline WPointF operator+(double factor) const
    {
        return WPointF(p_x+factor,p_y+factor);
    }

    inline WPointF operator-(double factor) const
    {
        return WPointF(p_x-factor,p_y-factor);
    }

    inline WPointF operator*(double factor) const
    {
        return WPointF(p_x*factor,p_y*factor);
    }

    inline WPointF operator/(double factor) const
    {
        return WPointF(p_x/factor,p_y/factor);
    }

    inline WPointF operator+(const WPointF &p) const
    {
        return WPointF(p_x+p.p_x,p_y+p.p_y);
    }

    inline WPointF operator-(const WPointF &p) const
    {
        return WPointF(p_x-p.p_x,p_y-p.p_y);
    }

    inline WPointF operator*(const WPointF &p) const
    {
        return WPointF(p_x*p.p_x,p_y*p.p_y);
    }

    inline WPointF operator/(const WPointF &p) const
    {
        return WPointF(p_x*p.p_x,p_y*p.p_y);
    }

    WPointF operator+(const WPoint &p) const;
    WPointF operator-(const WPoint &p) const;
    WPointF operator*(const WPoint &p) const;
    WPointF operator/(const WPoint &p) const;

    inline bool operator==(const WPointF &p) const
    {
        return p_x == p.p_x && p_y == p.p_y;
    }

    inline bool operator!=(const WPointF &p) const
    {
        return p_x != p.p_x || p_y != p.p_y;
    }

private:
    friend class WRect;
    friend class WPoint;
    float p_x,p_y;
};
#endif // WPointFF_H
