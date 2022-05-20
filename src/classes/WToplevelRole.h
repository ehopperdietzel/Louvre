#ifndef WTOPLEVELROLE_H
#define WTOPLEVELROLE_H

#include <WNamespaces.h>
#include <WSize.h>
#include <WRect.h>

class Wpp::WToplevelRole
{
public:
    WToplevelRole(wl_resource *toplevel, WSurface *surface);
    virtual ~WToplevelRole();

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

    enum State : UInt32
    {
        Deactivated = 1, // 1
        Maximized = 2, // 1
        Fullscreen = 4, // 2
        Resizing = 8, // 3
        Activated = 16, // 4
        TiledLeft = 32, // 5
        TiledRight = 64, // 6
        TiledTop = 128, // 7
        TiledBottom = 256 // 8
    };


    // xdg_toplevel
    virtual void startMoveRequest();
    virtual void maxSizeChanged(){};
    virtual void minSizeChanged(){};
    virtual void startResizeRequest(Edge edge);
    virtual void configureRequest(){};

    virtual void titleChanged();
    virtual void appIdChanged();

    // xdg_shell
    virtual void geometryChangeRequest();


    // Events
    void configure(SurfaceStateFlags states);
    void configure(const WSize &size, SurfaceStateFlags states);
    void configure(Int32 width, Int32 height, SurfaceStateFlags states);



    const WRect &windowGeometry() const;
    WSize calculateResizeRect(const WPoint &cursorPosDelta, const WSize &initialSize, Edge edge);

    WCompositor *compositor() const;
    WSeat *seat() const;
    WSurface *surface() const;


    wl_resource *resource() const;
    const char *appId() const;
    const char *title() const;
    const WSize &minSize() const;
    const WSize &maxSize() const;
private:
    friend class WWayland;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;

    void dispachLastConfiguration();
    wl_resource *p_resource = nullptr;
    WSurface *p_surface = nullptr;
    bool p_pendingConfigure = false;
    WSize p_pendingConfigureSize, p_minSize, p_maxSize;
    WRect p_windowGeometry;
    SurfaceStateFlags p_pendingConfigureStates;

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *p_appId = new char[1];
    char *p_title = new char[1];
};

#endif // WTOPLEVELROLE_H
