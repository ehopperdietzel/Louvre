#ifndef POINTER_H
#define POINTER_H

#include <LPointer.h>

using namespace Louvre;

class Pointer : public LPointer
{
public:
    Pointer(LSeat *seat);

    /* void pointerMoveEvent(float dx, float dy) override; */
    /* void pointerButtonEvent(UInt32 button, UInt32 state) override; */
    /* void pointerAxisEvent(double x, double y, UInt32 source) override; */
    /* void setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) override; */

};

#endif // POINTER_H
