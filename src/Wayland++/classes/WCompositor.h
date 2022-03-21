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
#include <condition_variable>
#include <sys/eventfd.h>

using namespace std;

class WaylandPlus::WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL() = 0;
    virtual void paintGL() = 0;

    virtual WClient *newClientRequest(wl_client *client) = 0;
    virtual void clientDisconnectRequest(WClient *client) = 0;

    virtual void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) = 0;

    virtual void libinputEvent(libinput_event *ev) = 0;
    virtual void pointerPosChanged(double x, double y, UInt32 milliseconds) = 0;
    virtual void pointerClickEvent(Int32 x, Int32 y, UInt32 button, UInt32 state, UInt32 milliseconds) = 0;
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) = 0;
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState) = 0;

    // Output
    void setOutputScale(Int32 scale);
    Int32 getOutputScale();

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
    timespec getNanoseconds();

    list<WClient*>clients;

private:
    friend class WWayland;
    friend class WInput;
    friend class WSurface;
    friend class Globals::Surface;
    friend class WaylandPlus::Globals::Pointer;

    // Output scale
    Int32 _outputScale = 1;

    bool scheduledDraw = false;
    UInt64 lastDraw = 0;
    void mainLoop();
    void flushClients();
    static void renderLoop(WCompositor *comp);

    WSurface *_pointerFocusSurface = nullptr;
    WSurface *_keyboardFocusSurface = nullptr;
    WSurface *_cursorSurface = nullptr;

    double _pointerX = 0.0;
    double _pointerY = 0.0;

    int renderFd, libinputFd, waylandFd;
    eventfd_t renderVal, libinputVal, waylandVal = 1;
};

#endif // WCOMPOSITOR_H
