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
#include <WTypes.h>

using namespace std;

class WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL() = 0;
    virtual void paintGL() = 0;
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

    void setFocusSurface(WSurface *surface);
    WSurface *getFocusSurface();


    UInt32 getMilliseconds();

    list<WClient*>clients;

    WTexture *cursorTexture = nullptr;
private:
    bool readyToDraw = false;
    void mainLoop();

    WSurface *_focusSurface = nullptr;

    double _pointerX = 0.0;
    double _pointerY = 0.0;

    timespec startTime;
};

#endif // WCOMPOSITOR_H
