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

using namespace std;

class WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL() = 0;
    virtual void paintGL() = 0;
    virtual void libinputEvent(libinput_event *ev) = 0;
    virtual void pointerPosChanged(double x, double y) = 0;
    virtual void pointerClickEvent(int x, int y, uint32_t button, libinput_button_state state) = 0;

    void repaint();

    int screenWidth();
    int screenHeight();

    double getPointerX();
    double getPointerY();

    void setPointerPos(double x, double y);

    void setFocusSurface(WSurface *surface);
    WSurface *getFocusSurface();


    uint64_t getMilliseconds();

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
