#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>
#include <MySurface.h>
#include <WPointF.h>

using namespace Wpp;

class MyCompositor : public WCompositor
{
public:
    MyCompositor();

    /***************
     **  FACTORY  **
     ***************/

    /*

    // Output
    WOutput *createOutputRequest() override;

    // Surface
    WSurface *createSurfaceRequest(wl_resource *surface, WClient *client) override;
    void destroySurfaceRequest(WSurface *surface) override;

    // Surface roles
    WToplevelRole *createToplevelRequest(wl_resource *toplevel, WSurface *surface) override;

    // Seat
    WSeat *createSeatRequest() override;

    // Surfaces list ( orderer from back to front )
    list<MySurface*>surfacesList;

    WTexture *defaultCursorTexture,*backgroundTexture;
    WPoint backgroundPos;

    void riseSurface(MySurface *surface);
    MySurface *surfaceAt(const WPoint &point);

    WCursor *cursor = nullptr;

    */

};

#endif // MYCOMPOSITOR_H
