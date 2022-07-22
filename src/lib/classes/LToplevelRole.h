#ifndef LTOPLEVELROLE_H
#define LTOPLEVELROLE_H

#include <LBaseSurfaceRole.h>
#include <LSize.h>
#include <LRect.h>

class Louvre::LToplevelRole : public LBaseSurfaceRole
{
public:
    LToplevelRole(wl_resource *toplevel, LSurface *surface);
    virtual ~LToplevelRole();

    enum Edge : UInt32
    {
        Top = 1,
        Bottom = 2,
        Left = 4,
        TopLeft = 5,
        BottomLeft = 6,
        Right = 8,
        TopRight = 9,
        BottomRight = 10
    };

    enum States : UChar8
    {
        Deactivated    = 0,
        Maximized      = 1,
        Fullscreen     = 2,
        Resizing       = 4,
        Activated      = 8,

    #if LOUVRE_XDG_WM_BASE_VERSION >= 2
        TiledLeft      = 16,
        TiledRight     = 32,
        TiledTop       = 64,
        TiledBottom    = 128
    #endif
    };

    #if LOUVRE_XDG_WM_BASE_VERSION >= 5

    struct WmCapabilities
    {
        enum : UChar8
        {
            WindowMenu = 1,
            Maximimize = 2,
            Fullscreen = 4,
            Minimize = 8
        };
    };


    UChar8 wmCapabilities() const;
    void setWmCapabilities(UChar8 capabilitiesFlags);

    #endif

    // Requests
    const LPoint &rolePos() const override;
    virtual void startMoveRequest();
    virtual void startResizeRequest(Edge edge);
    virtual void configureRequest();
    virtual void setMaximizedRequest();
    virtual void unsetMaximizedRequest();
    virtual void setMinimizedRequest();
    virtual void setFullscreenRequest(LOutput *destOutput);
    virtual void unsetFullscreenRequest();
    virtual void showWindowMenuRequest(Int32 x, Int32 y);

    // State change notification
    virtual void pong(UInt32 serial);
    virtual void maximizeChanged();
    virtual void fullscreenChanged();
    virtual void activatedChanged();
    virtual void maxSizeChanged();
    virtual void minSizeChanged();
    virtual void titleChanged();
    virtual void appIdChanged();
    virtual void geometryChanged();

    // Events
    void ping(UInt32 serial);
    void configure(UChar8 stateFlags);
    void configure(const LSize &size, UChar8 stateFlags);
    void configure(Int32 width, Int32 height, UChar8 stateFlags);
    void close();

    #if LOUVRE_XDG_WM_BASE_VERSION >= 4
    void configureBounds(const LSize &bounds);
    const LSize &bounds() const;
    #endif

    const LRect &windowGeometry() const;
    LSize calculateResizeRect(const LPoint &cursorPosDelta, const LSize &initialSize, Edge edge);

    const char *appId() const;
    const char *title() const;
    const LSize &minSize() const;
    const LSize &maxSize() const;
    bool maximized() const;
    bool fullscreen() const;
    bool activated() const;
    States state() const;

    class LToplevelRolePrivate;
    LToplevelRolePrivate *imp() const;
private:

    LToplevelRolePrivate *m_imp = nullptr;



};

#endif // LTOPLEVELROLE_H
