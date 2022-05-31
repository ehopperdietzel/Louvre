#ifndef LTOPLEVEL_H
#define LTOPLEVEL_H

#include <LNamespaces.h>
#include <LSize.h>
#include <LRect.h>

class Louvre::LToplevelRole
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
    virtual void configureRequest();
    virtual void maximizeRequest();
    virtual void unmaximizeRequest();
    virtual void minimizeRequest();
    virtual void fullscreenRequest(LOutput *destOutput);

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
    void configure(StateFlags states);
    void configure(const LSize &size, StateFlags states);
    void configure(Int32 width, Int32 height, StateFlags states);


    const LRect &windowGeometry() const;
    LSize calculateResizeRect(const LPoint &cursorPosDelta, const LSize &initialSize, Edge edge);

    LCompositor *compositor() const;
    LSeat *seat() const;
    LSurface *surface() const;


    wl_resource *resource() const;
    const char *appId() const;
    const char *title() const;
    const LSize &minSize() const;
    const LSize &maxSize() const;
    bool maximized() const;
    bool fullscreen() const;
    bool activated() const;
    StateFlags state() const;
private:
    friend class WWayland;
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Surface;
    friend class Extensions::XdgShell::Toplevel;

    struct TopLevelConfiguration
    {
        bool set = false;
        LSize size;
        StateFlags flags;
        UInt32 serial;
    };

    StateFlags p_stateFlags;
    TopLevelConfiguration p_currentConf;
    TopLevelConfiguration p_sentConf;
    TopLevelConfiguration p_pendingConf;

    void dispachLastConfiguration();
    wl_resource *p_resource = nullptr;
    LSurface *p_surface = nullptr;

    LSize p_minSize, p_maxSize;
    LRect p_windowGeometry;

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *p_appId = new char[1];
    char *p_title = new char[1];
};

#endif // LTOPLEVEL_H
