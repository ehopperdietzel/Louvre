#ifndef MYSEAT_H
#define MYSEAT_H

#include <WNamespaces.h>
#include <WSeat.h>
#include <WPoint.h>
#include <WRect.h>
#include "MySurface.h"

using namespace Wpp;

class MySeat : public WSeat
{
public:
    MySeat(WCompositor *compositor);

    void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) override;
    void pointerMoveEvent(float dx, float dy) override;
    void pointerClickEvent(UInt32 button, UInt32 state) override;
    void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) override;
    void keyEvent(UInt32 keyCode, UInt32 keyState) override;

    MyCompositor *comp;

    WPoint movingSurfaceInitPos, movingSurfaceInitCursorPos;

    bool isLeftMouseButtonPressed = false;

    // Active surfaces
    MySurface *movingSurface = nullptr;
    MySurface *resizingSurface = nullptr;
    MySurface *cursorSurface = nullptr;

    // Active resize edge
    ResizeEdge resizeEdge;

    // Resize surface initial rect
    WRect resizeInitSurfaceRect;
    WRect resizeInitSurfaceDecoration;

    // Resize surface initial mouse pos
    WPoint resizeInitMousePos;
};

#endif // MYSEAT_H
