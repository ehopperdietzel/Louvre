#ifndef LCOMPOSITOR_H
#define LCOMPOSITOR_H

#include <libinput.h>
#include <stdio.h>
#include <GLES2/gl2.h>

#include <list>
#include <algorithm>
#include <LClient.h>

#include <LNamespaces.h>
#include <condition_variable>
#include <sys/eventfd.h>

#include <mutex>
#include <thread>
#include <LOpenGL.h>

using namespace std;

class Louvre::LCompositor
{
public:
    LCompositor();

    virtual ~LCompositor(){};

    /* Create requests */
    virtual LOutput *createOutputRequest();
    virtual LClient *createClientRequest(wl_client *client);
    virtual LSurface *createSurfaceRequest(wl_resource *surface, LClient *client);
    virtual LSeat *createSeatRequest();
    virtual LToplevelRole *createToplevelRequest(wl_resource *toplevel, LSurface *surface);
    virtual LPopupRole *createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner);

    /* Destroy requests */
    virtual void destroyOutputRequest(LOutput *output);
    virtual void destroyClientRequest(LClient *client);
    virtual void destroySurfaceRequest(LSurface *surface);
    virtual void destroyToplevelRequest(LToplevelRole *toplevel);
    virtual void destroyPopupRequest(LPopupRole *popup);


    void start();
    void riseSurface(LSurface *surface);

    LCursor *cursor() const;
    LSeat *seat() const;

    // Output
    void repaintAllOutputs();
    void addOutput(LOutput *output);
    void removeOutput(LOutput *output);
    const list<LSurface*>&surfaces() const;
    const list<LOutput*>&outputs() const;


    list<LClient*>clients;

    std::thread::id mainThreadId() const {return p_threadId;}

    mutex renderMutex;

private:
    friend class LWayland;
    friend class LBackend;
    friend class WInput;
    friend class LSurface;
    friend class LOutput;
    friend class Globals::Compositor;
    friend class Globals::Surface;
    friend class Louvre::Globals::Pointer;


    LCursor *p_cursor = nullptr;
    LSeat *p_seat = nullptr;



    std::thread::id p_threadId;

    // Outputs
    list<LOutput*>p_outputs;

    // Surfaces
    list<LSurface*>p_surfaces;

    int libinputFd, waylandFd;
    eventfd_t libinputVal, waylandVal = 1;

    LOpenGL *p_painter;

    bool _started = false;
};

#endif // LCOMPOSITOR_H
