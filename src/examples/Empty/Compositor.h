#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <LCompositor.h>

using namespace Louvre;

class Compositor : public LCompositor
{
public:
    Compositor(const char *backend);

    /* void initialize() override; */

    /* Create requests */

    LOutputManager *createOutputManagerRequest() override;
    LOutput *createOutputRequest() override;
    LSurface *createSurfaceRequest(wl_resource *surface, LClient *client) override;
    LSeat *createSeatRequest() override;
    LPointer *createPointerRequest(LSeat *seat) override;
    LKeyboard *createKeyboardRequest(LSeat *seat) override;
    LToplevelRole *createToplevelRequest(wl_resource *toplevel, LSurface *surface) override;
    LPopupRole *createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner) override;

    /*
    virtual LSubsurfaceRole *createSubsurfaceRequest(wl_resource *subsurface, LSurface *surface); */

    /* Destroy requests */

    /*
    virtual void destroyOutputRequest(LOutput *output);
    virtual void destroyClientRequest(LClient *client);
    virtual void destroySurfaceRequest(LSurface *surface);
    virtual void destroyToplevelRequest(LToplevelRole *toplevel);
    virtual void destroyPopupRequest(LPopupRole *popup);
    virtual void destroyCursorRequest(LCursorRole *cursor); */

};

#endif // COMPOSITOR_H
