#ifndef LSURFACE_H
#define LSURFACE_H

#include <LTexture.h>
#include <LNamespaces.h>
#include <list>
#include <mutex>
#include <LRegion.h>
#include <LRect.h>

using namespace std;

class Louvre::LSurface
{
public:

    /* Roles */
    LToplevelRole *toplevel() const;
    LPopupRole *popup() const;

    enum SurfaceType : UInt32
    {
        Undefined = 0,
        Toplevel = 1,
        Popup = 2,
        Subsurface = 3,
        Cursor = 4
    };

    enum PosMode
    {
        SubRole = -1,
        NoRole = 0,
        AddRole = 1
    };

    LSurface(wl_resource *surface, LClient *client, GLuint textureUnit = 1);
    virtual ~LSurface();

    /* Requests */

    // wl_surface
    virtual void typeChangeRequest(){};

    // xdg_popup
    virtual void parentChangeRequest(){};
    virtual void bufferScaleChangeRequest(){};
    virtual void bufferSizeChangeRequest();

    // Input
    LSeat *seat() const;

    // Surface info
    SurfaceType type() const { return current.type; }

    // Size in surface coordinates
    const LPoint &pos(PosMode mode = NoRole) const;
    void setPos(const LPoint &newPos);
    void setPos(Int32 x, Int32 y);
    void setX(Int32 x);
    void setY(Int32 y, bool useRolePos = false);
    const LSize &size(bool useBufferSize = false) const;
    const LRegion &inputRegion() const { return current.inputRegion; }
    bool inputRegionContainsPoint(const LPoint &surfacePos,const LPoint &point);
    LPoint mapToLocal(const LPoint &point);

    bool minimized() const;
    void setMinimized(bool state);

    // Buffer
    Int32 bufferScale() const { return p_bufferScale; }
    LTexture *texture() const  { return p_texture; }
    bool isDamaged() const { return p_isDamaged; }
    void applyDamages();
    void requestNextFrame();

    // References
    wl_resource *resource() const { return p_resource; }
    wl_resource *xdgSurfaceResource() const;
    LClient *client() const { return p_client; }
    LCompositor *compositor() const;

    // Hierarchy
    LSurface *parent() const { return p_parent; };
    LSurface *topParent() const;
    list<LSurface*>&children() {return p_children;}
    const LPoint &hotspot() const {return p_hotspot;}

    struct State
    {
        SurfaceType type = SurfaceType::Undefined;
        wl_resource *buffer = nullptr;
        LRegion inputRegion;
        LSize size;
    };

    State current,pending;
    bool ack_configure = true;
 private:
    friend class LWayland;
    friend class LCompositor;
    friend class LOutput;
    friend class LPointer;
    friend class LToplevelRole;
    friend class LPopupRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;
    friend class Extensions::XdgShell::WmBase;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;
    friend class Extensions::XdgShell::Popup;

    // Roles
    LToplevelRole *p_toplevelRole = nullptr;
    LPopupRole *p_popupRole = nullptr;

    LTexture *p_texture = nullptr;
    list<LSurface*>p_children;

    // Buffer
    void setBufferScale(Int32 scale);

    LClient     *p_client         = nullptr;
    LSurface    *p_parent         = nullptr;

    wl_resource *p_resource       = nullptr;

    wl_resource *p_xdgSurfaceResource = nullptr;
    wl_resource *p_frameCallback   = nullptr;

    LPoint p_hotspot;
    Int32 p_bufferScale = 1;

    bool p_isDamaged = false;

    LPoint p_pos = LPoint(200,200);
    bool p_minimized = false;
    mutable LPoint p_xdgPos;

    // Configure event
    void dispachLastConfiguration();



};

#endif // LSURFACE_H
