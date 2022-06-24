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
    LCompositor(const char *backendPath);

    virtual ~LCompositor();

    virtual void initialize(){};

    /* Create requests */
    virtual LOutput *createOutputRequest();
    virtual LClient *createClientRequest(wl_client *client);
    virtual LSurface *createSurfaceRequest(wl_resource *surface, LClient *client);

    virtual LSeat *createSeatRequest();
    virtual LPointer *createPointerRequest(LSeat *seat);
    virtual LKeyboard *createKeyboardRequest(LSeat *seat);

    virtual LToplevelRole *createToplevelRequest(wl_resource *toplevel, LSurface *surface);
    virtual LPopupRole *createPopupRequest(wl_resource *popup, LSurface *surface, LPositioner *positioner);
    virtual LSubsurfaceRole *createSubsurfaceRequest(wl_resource *subsurface, LSurface *surface);

    /* Destroy requests */
    virtual void destroyOutputRequest(LOutput *output);
    virtual void destroyClientRequest(LClient *client);
    virtual void destroySurfaceRequest(LSurface *surface);
    virtual void destroyToplevelRequest(LToplevelRole *toplevel);
    virtual void destroyPopupRequest(LPopupRole *popup);
    virtual void destroyCursorRequest(LCursorRole *cursor);


    void start();
    void riseSurface(LSurface *surface);
    bool loadBackend(const char *backendPath);
   /** Insert or move a surface after another in the surfaces list.
     *
     * Insert or move (if already exists) surfaceToInsert after prevSurface
     * in the surfaces list. The function returns true only if surfaceToInsert is successfully inserted or moved.
     * If prevSurface doesn't exists in the list, false is returned and surfaceToInsert is not inserted or moved,
     * but also not removed from the list if already exists.
     *
     * @param prevSurface
     * The surface after the which surfaceToInsert is going to be inserted or moved.
     *
     * @param surfaceToInsert
     * The surface that's being inserted or moved.
     */
    bool insertSurfaceAfter(LSurface *prevSurface,LSurface *surfaceToInsert);
    bool insertSurfaceBefore(LSurface *nextSurface,LSurface *surfaceToInsert);


    LCursor *cursor() const;
    LSeat *seat() const;
    LDataSource *dataSelection() const;
    LDataSource *dataDrag() const;

    // Output
    void repaintAllOutputs();
    void addOutput(LOutput *output);
    void removeOutput(LOutput *output);
    const list<LSurface*>&surfaces() const;
    const list<LOutput*>&outputs() const;
    const list<LClient*>&clients() const;

    std::thread::id mainThreadId() const;

    class LCompositorPrivate;
    LCompositorPrivate *imp() const;

private:
    LCompositorPrivate *m_imp = nullptr;
};

#endif // LCOMPOSITOR_H
