#ifndef LREGION_H
#define LREGION_H

//#include <wayland-server.h>
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

    struct LRegionRect
    {
        LRect rect;
        bool add = true;
    };


    list<LRegionRect>&rects(){return p_rects;}
private:
    friend class Louvre::Globals::Compositor;
    friend class Louvre::Globals::Region;
    friend class Louvre::Globals::Surface;

    list<LRegionRect>p_rects;

    // Wayland
    wl_resource     *p_resource = nullptr;
    LClient         *p_client   = nullptr;

};

#endif // LREGION_H
