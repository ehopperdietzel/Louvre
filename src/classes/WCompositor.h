#ifndef WCOMPOSITOR_H
#define WCOMPOSITOR_H

#include <libinput.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include <list>
#include <algorithm>
#include <WClient.h>

#include <sys/time.h>
#include <WNamespaces.h>
#include <condition_variable>
#include <sys/eventfd.h>

#include <mutex>

using namespace std;

class Wpp::WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL(WOutput *output) = 0;
    virtual void paintGL(WOutput *output) = 0;

    virtual WClient *newClientRequest(wl_client *client) = 0;
    virtual void clientDisconnectRequest(WClient *client) = 0;
    virtual void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) = 0;
    virtual void libinputEvent(libinput_event *ev) = 0;
    virtual void pointerMoveEvent(float dx, float dy) = 0;
    virtual void pointerClickEvent(UInt32 button, UInt32 state) = 0;
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) = 0;
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState) = 0;

    // Output
    void repaintAllOutputs();
    void addOutput(WOutput *output);
    void removeOutput(WOutput *output);
    const list<WOutput*>&outputs(){ return p_outputs; };

    UInt32 getMilliseconds();
    timespec getNanoseconds();

    list<WClient*>clients;

    mutex renderMutex;

private:
    friend class WWayland;
    friend class WInput;
    friend class WSurface;
    friend class WOutput;
    friend class Globals::Surface;
    friend class Wpp::Globals::Pointer;

    // Output scale
    list<WOutput*>p_outputs;

    int libinputFd, waylandFd;
    eventfd_t libinputVal, waylandVal = 1;

    bool _started = false;
};

#endif // WCOMPOSITOR_H
