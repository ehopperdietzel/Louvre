#ifndef LSURFACE_H
#define LSURFACE_H

#include <LTexture.h>
#include <LNamespaces.h>
#include <list>
#include <mutex>
#include <LRegion.h>
#include <LRect.h>

using namespace std;

/// Representation of a client surface (usually refered as window).
///
/// A surface is the representation of a client window that can be drawn on screen.
/// It has a position, size, texture, and a role.\n
/// Initially all surfaces have no role (Undefined), you must wait for them to get a role (notified with typeChangedRequest())
/// in order to be able to draw them on screen.
/// A list of all created surfaces can be accessed from the LCompositor class, check the LCompositor::surfaces() method.
/// A list of all surfaces created by a client can be accessed from the LClient class, check the LClient::surfaces() method.
class Louvre::LSurface
{
public:

    LRegion damage[2];
    LRegion globalDamage;
    LRect rect[2];

    /* Roles */
    LCursorRole *cursor() const;
    LToplevelRole *toplevel() const;
    LPopupRole *popup() const;
    LSubsurfaceRole *subsurface() const;
    LBaseSurfaceRole *role() const;

    enum RoleTypes : UInt32
    {
        Undefined = 0,
        Toplevel = 1,
        Popup = 2,
        Subsurface = 3,
        Cursor = 4
    };

    LSurface(wl_resource *surface, LClient *client, GLuint textureUnit = 1);
    virtual ~LSurface();

    /* Requests */

    // wl_surface
    virtual void roleChanged();
    virtual void parentChanged();
    virtual void bufferScaleChanged();
    virtual void bufferSizeChanged();
    virtual void opaqueRegionChanged();
    virtual void inputRegionChanged();

    // Input
    LSeat *seat() const;

    // Surface info
    UInt32 roleType() const;

    // Size in surface coordinates
    const LPoint &pos(bool useRolePos = false) const;
    void setPos(const LPoint &newPos);
    void setPos(Int32 x, Int32 y);
    void setX(Int32 x);
    void setY(Int32 y, bool useRolePos = false);
    const LSize &size(bool useBufferSize = false) const;
    const LRegion &inputRegion() const;
    const LRegion &opaqueRegion() const;
    const LRegion &translucentRegion() const;
    const LRegion &damages() const;
    bool inputRegionContainsPoint(const LPoint &surfacePos,const LPoint &point);
    LPoint mapToLocal(const LPoint &point);

    bool minimized() const;
    void setMinimized(bool state);

    // Buffer
    Int32 bufferScale() const;
    LTexture *texture() const;
    bool isDamaged() const;
    bool textureChanged() const;
    void requestNextFrame();

    // References
    wl_resource *resource() const;
    wl_resource *xdgSurfaceResource() const;
    LClient *client() const;
    LCompositor *compositor() const;

    // Hierarchy
    LSurface *parent() const;
    LSurface *topParent() const;
    const list<LSurface*>&children() const;

    class LSurfacePrivate;
    LSurfacePrivate *imp() const;
 private:

    LSurfacePrivate *m_imp = nullptr;

};

#endif // LSURFACE_H
