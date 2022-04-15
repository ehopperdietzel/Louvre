#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>
#include <WNamespaces.h>
#include <list>
#include <mutex>

#include <WRect.h>

using namespace std;

void createNullKeys();

class Wpp::WSurface
{
public:
    WSurface(wl_resource *surface, WClient *client, GLuint textureUnit = 0);
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
    const char *appId();
    const char *title();
    Wpp::SurfaceType type();

    // Size in surface coordinates
    Int32 width() { return texture()->size().w(); }
    Int32 height() { return texture()->size().h(); }
    WSize size(){ return texture()->size(); }
    WSize minSize(){ return _minSize; }
    WSize maxSize(){ return _maxSize; }
    WRect decorationGeometry(){ return _decorationGeometry; };

    // Popup positioner
    WPositioner *positioner();

    // Buffer
    Int32 bufferScale();
    WTexture *texture();
    bool isDamaged();
    void applyDamages();

    // References
    wl_resource *resource();
    WClient *client();
    WCompositor *compositor();

    // Hierarchy
    WSurface *parent();
    const list<WSurface*>children();

    //mutex surfaceMutex;

 private:
    friend class WWayland;
    friend class WCompositor;
    friend class WOutput;
    friend class Globals::Surface;
    friend class Wpp::Globals::Pointer;
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

    wl_resource *committedBuffer = nullptr;
    wl_resource *pendingBuffer = nullptr;

    wl_resource *xdg_shell = nullptr;
    wl_resource *xdg_toplevel = nullptr;
    wl_resource *xdg_popup = nullptr;

    WPoint _maxSize;
    WPoint _minSize;
    Int32 _bufferScale = 1;
    char *_appId = new char[1];
    char *_title = new char[1];

    WRect _decorationGeometry;

    list<WSurface*>_children;

    bool _isDamaged = false;



};

#endif // WSURFACE_H
