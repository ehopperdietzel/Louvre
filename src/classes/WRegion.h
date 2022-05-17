#ifndef WREGION_H
#define WREGION_H

//#include <wayland-server.h>
#include <list>
#include <WNamespaces.h>
#include <WRect.h>

using namespace std;

class Wpp::WRegion
{
public:
    WRegion();

    void copy(const WRegion &region);
    void clear();
    void addRect(const WRect &rect);
    void subtractRect(const WRect &rect);
    void multiply(double factor);
    bool containsPoint(const WPoint &point);

    struct WRegionRect
    {
        WRect rect;
        bool add = true;
    };


    list<WRegionRect>&rects(){return p_rects;}
private:
    friend class Wpp::Globals::Compositor;
    friend class Wpp::Globals::Region;
    friend class Wpp::Globals::Surface;

    list<WRegionRect>p_rects;

    // Wayland
    wl_resource     *p_resource = nullptr;
    WClient         *p_client   = nullptr;

};

#endif // WREGION_H
