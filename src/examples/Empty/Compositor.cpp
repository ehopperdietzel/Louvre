#include "Compositor.h"

#include <OutputManager.h>
#include <Output.h>
#include <Surface.h>
#include <Seat.h>
#include <Pointer.h>
#include <Keyboard.h>
#include <Toplevel.h>
#include <Popup.h>

using namespace Louvre;

Compositor::Compositor(const char *backend) : LCompositor(backend) {}

/* Factory */

LOutputManager *Compositor::createOutputManagerRequest()
{
    return new OutputManager(this);
}

LOutput *Compositor::createOutputRequest()
{
    return new Output();
}

LSurface *Compositor::createSurfaceRequest(wl_resource *surface, LClient *client)
{
    return new Surface(surface,client);
}

LSeat *Compositor::createSeatRequest()
{
    return new Seat(this);
}

LPointer *Compositor::createPointerRequest(LSeat *seat)
{
    return new Pointer(seat);
}

LKeyboard *Compositor::createKeyboardRequest(LSeat *seat)
{
    return new Keyboard(seat);
}

LToplevelRole *Compositor::createToplevelRequest(wl_resource *toplevel, LSurface *surface)
{
    return new Toplevel(toplevel,surface);
}

LPopupRole *Compositor::createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner)
{
    return new Popup(popup,surface,positioner);
}
