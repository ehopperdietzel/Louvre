#ifndef LREGION_H
#define LREGION_H

#include <list>
#include <LNamespaces.h>
#include <LRect.h>

using namespace std;

class Louvre::LRegion
{
public:
    LRegion();

    void copy(const LRegion &region);
    void clear();
    void addRect(const LRect &rect);
    void subtractRect(const LRect &rect);
    void multiply(double factor);
    bool containsPoint(const LPoint &point);
    void simplify();
    void offset(const LPoint &offset);

    /* Recorta las rectas */
    void clip(const LRect &rect);


    const list<LRect>&rects() const;
private:
    friend class Louvre::Globals::Compositor;
    friend class Louvre::Globals::Region;
    friend class Louvre::Globals::Surface;

    list<LRect>m_rects;


};

#endif // LREGION_H
