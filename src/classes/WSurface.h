#ifndef WSURFACE_H
#define WSURFACE_H

#include <WTexture.h>
#include <WNamespaces.h>
#include <list>
#include <mutex>
#include <WRegion.h>
#include <WRect.h>

using namespace std;

class Wpp::WSurface
{
public:

    /* Roles */
    WToplevelRole *toplevel() const;

    enum SurfaceType : UInt32
    {
        Undefined = 0,
        Toplevel = 1,
        Popup = 2,
        Subsurface = 3,
        Cursor = 4
    };

    WSurface(wl_resource *surface, WClient *client, GLuint textureUnit = 1);
    virtual ~WSurface();

    /* Requests */

    // wl_surface
    virtual void typeChangeRequest(){};

    // xdg_popup
    virtual void positionerChangeRequest(){};
    virtual void parentChangeRequest(){};
    virtual void bufferScaleChangeRequest(){};
    virtual void bufferSizeChangeRequest();
    virtual void grabSeatRequest(){};

    void sendConfigurePopupEvent(Int32 x, Int32 y, Int32 width, Int32 height);

    // Input
    WSeat *seat() const;

    // Surface info
    SurfaceType type() const { return current.type; }

    // Size in surface coordinates
    const WPoint &pos(bool useRolePos = false) const;
    void setPos(const WPoint &newPos);
    void setPos(Int32 x, Int32 y);
    void setX(Int32 x);
    void setY(Int32 y, bool useRolePos = false);
    Int32 width()               const { return texture()->size().w(); }
    Int32 height()              const { return texture()->size().h(); }
    const WSize &size()         const { return texture()->size(); }
    const WRegion &inputRegion() const { return current.inputRegion; }
    bool inputRegionContainsPoint(const WPoint &surfacePos,const WPoint &point);
    WPoint mapToLocal(const WPoint &point);

    bool minimized() const;
    void setMinimized(bool state);

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
    friend class WToplevelRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;
    friend class Extensions::XdgShell::WmBase;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;
    friend class Extensions::XdgShell::Popup;

    // Roles
    WToplevelRole *p_toplevelRole = nullptr;

    WTexture *p_texture = nullptr;
    list<WSurface*>p_children;

    // Buffer
    void setBufferScale(Int32 scale);

    WClient     *p_client         = nullptr;
    WPositioner *p_positioner     = nullptr;
    WSurface    *p_parent         = nullptr;

    wl_resource *p_resource       = nullptr;

    wl_resource *p_xdg_shell       = nullptr;
    wl_resource *p_xdg_popup       = nullptr;
    wl_resource *p_frameCallback   = nullptr;

    WPoint p_hotspot;
    Int32 p_bufferScale = 1;

    bool p_isDamaged = false;

    WPoint p_pos = WPoint(200,200);
    bool p_minimized = false;
    mutable WPoint p_xdgPos;

    // Configure event
    void dispachLastConfiguration();



};

#endif // WSURFACE_H
