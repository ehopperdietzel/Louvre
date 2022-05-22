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

    typedef unsigned char StateFlags;

    enum TopLevelStates : unsigned char
    {
        Deactivated = 0,
        Maximized = 1,
        Fullscreen = 2,
        Resizing = 4,
        Activated = 8,
        TiledLeft = 16,
        TiledRight = 32,
        TiledTop = 64,
        TiledBottom = 128
    };

    // Requests
    virtual void startMoveRequest();
    virtual void startResizeRequest(Edge edge);
    virtual void configureRequest(){};
    virtual void maximizeRequest();
    virtual void unmaximizeRequest();

    // State change notification
    virtual void maximizeChanged();
    virtual void maxSizeChanged(){};
    virtual void minSizeChanged(){};
    virtual void titleChanged();
    virtual void appIdChanged();

    // xdg_shell
    virtual void geometryChangeRequest();


    // Events
    void configure(StateFlags states);
    void configure(const WSize &size, StateFlags states);
    void configure(Int32 width, Int32 height, StateFlags states);


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
    bool maximized() const;
    StateFlags state() const;
private:
    friend class WWayland;
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;

    struct TopLevelConfiguration
    {
        bool set = false;
        WSize size;
        StateFlags flags;
        UInt32 serial;
    };

    StateFlags p_stateFlags;
    TopLevelConfiguration p_currentConf;
    TopLevelConfiguration p_sentConf;
    TopLevelConfiguration p_pendingConf;

    void dispachLastConfiguration();
    wl_resource *p_resource = nullptr;
    WSurface *p_surface = nullptr;

    WSize p_minSize, p_maxSize;
    WRect p_windowGeometry;

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *p_appId = new char[1];
    char *p_title = new char[1];
};

#endif // WTOPLEVELROLE_H
