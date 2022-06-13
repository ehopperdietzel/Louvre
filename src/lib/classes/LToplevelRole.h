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

    const LPoint &rolePos() const override;

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

    // Requests
    virtual void pong(UInt32 serial);
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
    virtual void maximizeChanged();
    virtual void fullscreenChanged();
    virtual void activatedChanged();
    virtual void maxSizeChanged(){};
    virtual void minSizeChanged(){};
    virtual void titleChanged();
    virtual void appIdChanged();

    // xdg_shell
    virtual void geometryChangeRequest();

    // Events
    void ping(UInt32 serial);
    void configure(UChar8 stateFlags);
    void configure(const LSize &size, UChar8 stateFlags);
    void configure(Int32 width, Int32 height, UChar8 stateFlags);

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
private:
    friend class LWayland;
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;

    struct TopLevelConfiguration
    {
        bool set = false;
        LSize size = LSize();
        UChar8 flags = Deactivated;
        UInt32 serial = 0;
    };

    UChar8 p_stateFlags = Deactivated;
    TopLevelConfiguration p_currentConf;
    TopLevelConfiguration p_sentConf;
    TopLevelConfiguration p_pendingConf;

    void dispachLastConfiguration();

    LSize p_minSize;
    LSize p_maxSize;
    LRect p_windowGeometry;

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *p_appId = nullptr;
    char *p_title = nullptr;

};

#endif // LTOPLEVELROLE_H
