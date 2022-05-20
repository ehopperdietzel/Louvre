/*
#ifndef MYTOPLEVEL_H
#define MYTOPLEVEL_H

#include <WToplevelRole.h>
#include "MySeat.h"
#include "MyCompositor.h"

using namespace Wpp;

class MyToplevel : public WToplevelRole
{
public:
    MyToplevel(wl_resource *toplevel, WSurface *surface);

    void startMoveRequest() override;
    void maxSizeChanged() override;
    void minSizeChanged() override;
    void startResizeRequest(ResizeEdge edge) override;
    void configureRequest() override;

    MySeat *mySeat;
    MySurface *mySurface;
    MyCompositor *myCompositor;
};

#endif // MYTOPLEVEL_H
*/
