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
#include <thread>
#include <WOpenGL.h>

using namespace std;

class Wpp::WCompositor
{
public:
    WCompositor();
    void start();

    virtual void initializeGL(WOutput *output) = 0;
    virtual void paintGL(WOutput *output) = 0;

    virtual WSeat *configureSeat() = 0;
    virtual WClient *newClientRequest(wl_client *client) = 0;
    virtual void clientDisconnectRequest(WClient *client) = 0;


    WSeat *seat(){return p_seat;}

    // Output
    void repaintAllOutputs();
    void addOutput(WOutput *output);
    void removeOutput(WOutput *output);
    const list<WOutput*>&outputs(){ return p_outputs; };

    UInt32 getMilliseconds();
    timespec getNanoseconds();

    list<WClient*>clients;

    std::thread::id mainThreadId(){return p_threadId;}

    mutex renderMutex;

private:
    friend class WWayland;
    friend class WBackend;
    friend class WInput;
    friend class WSurface;
    friend class WOutput;
    friend class Globals::Surface;
    friend class Wpp::Globals::Pointer;


    WSeat *p_seat = nullptr;



    std::thread::id p_threadId;

    // Outputs
    list<WOutput*>p_outputs;

    int libinputFd, waylandFd;
    eventfd_t libinputVal, waylandVal = 1;

    WOpenGL *p_painter;

    bool _started = false;
};

#endif // WCOMPOSITOR_H
