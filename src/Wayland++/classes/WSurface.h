#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>
#include <WNamespaces.h>
#include <list>

using namespace std;

void createNullKeys();

class WaylandPlus::WSurface
{
public:
    WSurface(UInt32 id, wl_resource *res, WClient *client);
    virtual ~WSurface() = 0;

    WTexture *texture = new WTexture();

    // Requests
    virtual void moveStartRequest()= 0;
    virtual void maxSizeChangeRequest(Int32 width, Int32 height) = 0;
    virtual void minSizeChangeRequest(Int32 width, Int32 height) = 0;
    virtual void resizeStartRequest(ResizeEdge edge) = 0;
    virtual void geometryChangeRequest(Int32 x, Int32 y, Int32 width, Int32 height) = 0;
    virtual void typeChangeRequest() = 0;
    virtual void positionerChangeRequest() = 0;
    virtual void parentChangeRequest() = 0;

    // Events
    void sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds);
    void sendPointerMotionEvent(double x, double y, UInt32 milliseconds);
    void sendPointerEnterEvent(double x, double y);
    void sendPointerLeaveEvent();
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds);
    void sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    void sendKeyboardEnterEvent();
    void sendKeyboardLeaveEvent();
    void sendConfigureEvent(Int32 width, Int32 height, SurfaceStateFlags states);

    const char *getAppId();
    const char *getTitle();

    void setPos(int x, int y);
    void setX(int x);
    void setY(int y);
    void setXWithoutDecoration(Int32 x);
    void setYWithoutDecoration(Int32 y);
    int getX();
    int getY();

    int getWidth();
    int getHeight();

    Rect getRectWithoutDecoration();

    WPositioner *getPositioner();

    Int32 getMinWidth();
    Int32 getMinHeight();
    Int32 getMaxWidth();
    Int32 getMaxHeight();

    void setMinWidth(Int32 width);
    void setMinHeight(Int32 height);
    void setMaxWidth(Int32 width);
    void setMaxHeight(Int32 height);

    int mapXtoLocal(int xGlobal);
    int mapYtoLocal(int yGlobal);

    bool containsPoint(Int32 x, Int32 y, bool withoutDecoration = false);

    Int32 getBufferScale();
    void setBufferScale(Int32 scale);

    wl_resource *getResource();
    WClient *getClient();
    WCompositor *getCompositor();
    UInt32 getId();
    SurfaceType getType();
    WSurface *getParent();
    const list<WSurface*>getChildren();

 private:
    friend class Globals::Surface;
    friend class Extensions::XdgShell::WmBase;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;
    friend class Extensions::XdgShell::Popup;

    void setAppId(const char *appId);
    void setTitle(const char *title);

    SurfaceType _type = SurfaceType::Undefined;

    Int32 xdgShellVersion = -1;

    UInt32 moveSerial, pointerSerial, configureSerial = 0;

    WClient *_client = nullptr;
    WPositioner *_positioner = nullptr;
    WSurface *_parent = nullptr;

    wl_resource *_resource = nullptr;
    wl_resource *frame_callback = nullptr;
    wl_resource *buffer = nullptr;
    wl_resource *xdg_shell = nullptr;
    wl_resource *xdg_toplevel = nullptr;
    wl_resource *xdg_popup = nullptr;


    int _posX = 0;
    int _posY = 0;
    Int32 _maxWidth,_maxHeight,_minWidth,_minHeight = -1;
    Int32 _bufferScale = 1;
    UInt32 _id;
    char *_appId = new char(1);
    char *_title = new char(1);

    Rect _decorationGeometry;

    list<WSurface*>_children;



};

#endif // WSURFACE_H
