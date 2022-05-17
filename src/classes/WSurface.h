#ifndef WSURFACE_H
#define WSURFACE_H

//#include <wayland-server.h>
#include <WTexture.h>
#include <WNamespaces.h>
#include <list>
#include <mutex>
#include <WRegion.h>
#include <WRect.h>

// Input
#include <WPointer.h>
#include <WKeyboard.h>

using namespace std;

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
    virtual void bufferSizeChangeRequest() = 0;
    virtual void grabSeatRequest() = 0;


    virtual void configureToplevelRequest() = 0;
    virtual void resizingChanged() = 0;

    void sendConfigureToplevelEvent(Int32 width, Int32 height, SurfaceStateFlags states);
    void sendConfigurePopupEvent(Int32 x, Int32 y, Int32 width, Int32 height);

    // Input
    WPointer &pointer();
    WKeyboard &keyboard();

    // Surface info
    const char *appId()     const { return p_appId; }
    const char *title()     const { return p_title; }
    Wpp::SurfaceType type() const { return current.type; }

    // Size in surface coordinates
    Int32 width()               const { return texture()->size().w(); }
    Int32 height()              const { return texture()->size().h(); }
    const WSize &size()         const { return texture()->size(); }
    const WSize &minSize()      const { return p_minSize; }
    const WSize &maxSize()      const { return p_maxSize; }
    const WRect &decorationGeometry() const { return current.windowGeometry; }
    WSize calculateResizeRect(const WPoint &cursorPosDelta, const WSize &initialSize, ResizeEdge edge);
    const WRegion &inputRegion() const { return current.inputRegion; }
    bool inputRegionContainsPoint(const WPoint &surfacePos,const WPoint &point);

    // Popup positioner
    WPositioner *positioner() const { return p_positioner; }

    // Buffer
    Int32 bufferScale() const { return p_bufferScale; }
    WTexture *texture() const  { return p_texture; }
    bool isDamaged() const { return p_isDamaged; }
    void applyDamages();
    void requestNextFrame();

    // References
    wl_resource *resource() const { return p_resource; }
    WClient *client() const { return p_client; }
    WCompositor *compositor() const;

    // Hierarchy
    WSurface *parent() const { return p_parent; };
    WSurface *topParent() const;
    list<WSurface*>&children() {return p_children;}
    const WPoint &hotspot() const {return p_hotspot;}

    struct State
    {
        SurfaceType type = SurfaceType::Undefined;
        wl_resource *buffer = nullptr;
        WRect windowGeometry = WRect();
        WRegion inputRegion;
    };

    State current,pending;
    bool ack_configure = true;
    Int32 t = 0;
 private:
    friend class WWayland;
    friend class WCompositor;
    friend class WOutput;
    friend class WPointer;
    friend class Globals::Surface;
    friend class Globals::Pointer;
    friend class Extensions::XdgShell::WmBase;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;
    friend class Extensions::XdgShell::Popup;

    WTexture *p_texture = nullptr;
    list<WSurface*>p_children;

    // Input
    WPointer p_pointer = WPointer(this);
    WKeyboard p_keyboard = WKeyboard(this);

    // Buffer
    void setBufferScale(Int32 scale);

    // Xdg Toplevel
    void setAppId(const char *appId);
    void setTitle(const char *title);

    Int32 xdgShellVersion = -1;

    UInt32 moveSerial, pointerSerial, keyboardSerial, configureSerial = 0;

    WClient     *p_client         = nullptr;
    WPositioner *p_positioner     = nullptr;
    WSurface    *p_parent         = nullptr;

    wl_resource *p_resource       = nullptr;

    wl_resource *p_xdg_shell       = nullptr;
    wl_resource *p_xdg_toplevel    = nullptr;
    wl_resource *p_xdg_popup       = nullptr;
    wl_resource *p_frameCallback   = nullptr;

    WPoint p_hotspot;
    WSize p_maxSize, p_minSize;
    Int32 p_bufferScale = 1;

    char *p_appId = new char[1];
    char *p_title = new char[1];



    bool p_isDamaged = false;


    // Configure event
    void dispachLastConfiguration();
    bool p_pendingConfigure = false;
    WSize p_pendingConfigureSize;
    SurfaceStateFlags p_pendingConfigureStates;


};

#endif // WSURFACE_H
