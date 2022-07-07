#ifndef SURFACE_H
#define SURFACE_H

#include <LSurface.h>

using namespace Louvre;

class Surface : public LSurface
{
public:
    Surface(wl_resource *surface, LClient *client);
    LRegion dmg[2];
    LRegion dmgT;
    LRegion transT;
    LRegion opaqueT;

    LRegion opaqueR;
    LRegion transR;

    bool chg[2];
    LRect rct[2];

    bool changedOrder[2];
};

#endif // SURFACE_H
