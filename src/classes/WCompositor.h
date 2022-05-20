#ifndef WCOMPOSITOR_H
#define WCOMPOSITOR_H

#include <libinput.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include <list>
#include <algorithm>
#include <WClient.h>

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

    /* Create requests */
    virtual WOutput *createOutputRequest();
    virtual WClient *createClientRequest(wl_client *client);
    virtual WSurface *createSurfaceRequest(wl_resource *surface, WClient *client);
    virtual WSeat *createSeatRequest();
    virtual WToplevelRole *createToplevelRequest(wl_resource *toplevel, WSurface *surface);

    /* Destroy requests */
    virtual void destroyOutputRequest(WOutput *output);
    virtual void destroyClientRequest(WClient *client);
    virtual void destroySurfaceRequest(WSurface *surface);
    virtual void destroyTopLevelRequest(WToplevelRole *topLevel);


    void start();
    void riseSurface(WSurface *surface);

    WCursor *cursor() const;
    WSeat *seat() const;

    // Output
    void repaintAllOutputs();
    void addOutput(WOutput *output);
    void removeOutput(WOutput *output);
    const list<WSurface*>&surfaces() const;
    const list<WOutput*>&outputs() const;


    list<WClient*>clients;

    std::thread::id mainThreadId() const {return p_threadId;}

    mutex renderMutex;

private:
    friend class WWayland;
    friend class WBackend;
    friend class WInput;
    friend class WSurface;
    friend class WOutput;
    friend class Globals::Compositor;
    friend class Globals::Surface;
    friend class Wpp::Globals::Pointer;


    WCursor *p_cursor = nullptr;
    WSeat *p_seat = nullptr;



    std::thread::id p_threadId;

    // Outputs
    list<WOutput*>p_outputs;

    // Surfaces
    list<WSurface*>p_surfaces;

    int libinputFd, waylandFd;
    eventfd_t libinputVal, waylandVal = 1;

    WOpenGL *p_painter;

    bool _started = false;
};

#endif // WCOMPOSITOR_H
