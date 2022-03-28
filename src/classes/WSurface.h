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
    WSurface(UInt32 id, wl_resource *res, WClient *client, GLuint textureUnit = 0);
    virtual ~WSurface() = 0;

    // Requests
    virtual void moveStartRequest() = 0;
    virtual void maxSizeChangeRequest() = 0;
    virtual void minSizeChangeRequest() = 0;
    virtual void resizeStartRequest(ResizeEdge edge) = 0;
    virtual void geometryChangeRequest() = 0;
    virtual void typeChangeRequest() = 0;
    virtual void positionerChangeRequest() = 0;
    virtual void parentChangeRequest() = 0;
    virtual void bufferScaleChangeRequest() = 0;

    // Events
    void sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds);
    void sendPointerMotionEvent(double x, double y, UInt32 milliseconds);
    void sendPointerEnterEvent(double x, double y);
    void sendPointerLeaveEvent();
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState);
    void sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    void sendKeyboardEnterEvent();
    void sendKeyboardLeaveEvent();
    void sendConfigureEvent(Int32 width, Int32 height, SurfaceStateFlags states);

    // Surface info
    const char *getAppId();
    const char *getTitle();
    UInt32 getId();
    SurfaceType getType();

    // Size in surface coordinates
    Int32 getWidth();
    Int32 getHeight();
    Int32 getMinWidth();
    Int32 getMinHeight();
    Int32 getMaxWidth();
    Int32 getMaxHeight();
    const Rect getDecorationGeometry();

    // Popup positioner
    WPositioner *getPositioner();

    // Buffer
    Int32 getBufferScale();
    WTexture *getTexture();
    bool isDamaged();
    void applyDamages();

    // References
    wl_resource *getResource();
    WClient *getClient();
    WCompositor *getCompositor();

    // Hierarchy
    WSurface *getParent();
    const list<WSurface*>getChildren();


 private:
    friend class WCompositor;
    friend class Globals::Surface;
    friend class Extensions::XdgShell::WmBase;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;
    friend class Extensions::XdgShell::Popup;

    WTexture *_texture = nullptr;

    // Buffer
    void setBufferScale(Int32 scale);

    // Xdg Toplevel
    void setAppId(const char *appId);
    void setTitle(const char *title);

    SurfaceType _type = SurfaceType::Undefined;

    Int32 xdgShellVersion = -1;

    UInt32 moveSerial, pointerSerial, keyboardSerial, configureSerial = 0;

    WClient *_client = nullptr;
    WPositioner *_positioner = nullptr;
    WSurface *_parent = nullptr;

    wl_resource *_resource = nullptr;
    wl_resource *frame_callback = nullptr;
    wl_resource *buffer = nullptr;
    wl_resource *xdg_shell = nullptr;
    wl_resource *xdg_toplevel = nullptr;
    wl_resource *xdg_popup = nullptr;

    Size _maxSize = {1,1};
    Size _minSize = {1,1};
    Int32 _bufferScale = 1;
    UInt32 _id;
    char *_appId = new char[1];
    char *_title = new char[1];

    Rect _decorationGeometry;

    list<WSurface*>_children;

    bool _isDamaged = false;



};

#endif // WSURFACE_H
