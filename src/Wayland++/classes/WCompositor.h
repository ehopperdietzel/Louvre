#ifndef WCOMPOSITOR_H
#define WCOMPOSITOR_H

#include <libinput.h>
#include <stdio.h>
#include <GLES2/gl2.h>
#include <wayland-server.h>

#include <list>
#include <algorithm>
#include <WClient.h>

#include <sys/time.h>
#include <WNamespaces.h>

using namespace std;

class WaylandPlus::WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL() = 0;
    virtual void paintGL() = 0;

    virtual void newClient(WClient *client) = 0;
    virtual void clientDisconnected(WClient *client) = 0;

    virtual void newSurface(WSurface *surface) = 0;
    virtual void surfaceDestroyed(WSurface *surface) = 0;
    virtual void surfaceMoveEvent(WSurface *surface)= 0;
    virtual void surfaceMaxSizeChanged(WSurface *surface, Int32 width, Int32 height) = 0;
    virtual void surfaceMinSizeChanged(WSurface *surface, Int32 width, Int32 height) = 0;
    virtual void surfaceResizeRequest(WSurface *surface, ResizeEdge edge) = 0;
    virtual void surfaceGeometryChangedRequest(WSurface *surface, Int32 x, Int32 y, Int32 width, Int32 height) = 0;

    virtual void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) = 0;

    virtual void libinputEvent(libinput_event *ev) = 0;
    virtual void pointerPosChanged(double x, double y, UInt32 milliseconds) = 0;
    virtual void pointerClickEvent(Int32 x, Int32 y, UInt32 button, UInt32 state, UInt32 milliseconds) = 0;
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) = 0;
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState,UInt32 milliseconds) = 0;

    void repaint();

    Int32 screenWidth();
    Int32 screenHeight();

    double getPointerX();
    double getPointerY();

    void setPointerPos(double x, double y, UInt32 milliseconds);

    WSurface *getPointerFocusSurface();
    WSurface *getKeyboardFocusSurface();
    WSurface *getCursorSurface();
    UInt32 cursorXOffset, cursorYOffset = 0;

    void clearPointerFocus();
    void clearKeyboardFocus();


    UInt32 getMilliseconds();

    list<WClient*>clients;

    WSurface *cursorSurface = nullptr;
private:
    friend class WSurface;

    bool readyToDraw = false;
    void mainLoop();

    WSurface *_pointerFocusSurface = nullptr;
    WSurface *_keyboardFocusSurface = nullptr;

    double _pointerX = 0.0;
    double _pointerY = 0.0;

    timespec startTime;
};

#endif // WCOMPOSITOR_H
