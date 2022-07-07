#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <LToplevelRole.h>

using namespace Louvre;

class Toplevel : public LToplevelRole
{
public:
    Toplevel(wl_resource *toplevel, LSurface *surface);

    // Requests
    /* void startMoveRequest() override; */
    /* void startResizeRequest(Edge edge) override; */
    /* void configureRequest() override; */
    /* void setMaximizedRequest() override; */
    /* void unsetMaximizedRequest() override; */
    /* void setMinimizedRequest() override; */
    /* void setFullscreenRequest(LOutput *destOutput) override; */
    /* void unsetFullscreenRequest() override; */
    /* void showWindowMenuRequest(Int32 x, Int32 y) override; */

    // State changes
    /* void pong(UInt32 serial) override; */
    /* void maximizeChanged() override; */
    /* void fullscreenChanged() override; */
    /* void activatedChanged() override; */
    /* void maxSizeChanged() override; */
    /* void minSizeChanged() override; */
    /* void titleChanged() override; */
    /* void appIdChanged() override; */
    /* void geometryChanged() override; */
};

#endif // TOPLEVEL_H
