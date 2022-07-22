#include <LToplevelRolePrivate.h>
#include <LBaseSurfaceRolePrivate.h>
#include <LSurface.h>
#include <string.h>
#include <LCompositor.h>
#include <LSeat.h>
#include <LWayland.h>
#include <LOutput.h>
#include <LPoint.h>
#include <LCursor.h>
#include <xdg-shell.h>

#include <LPointer.h>
#include <LKeyboard.h>

#include <LTime.h>

using namespace Louvre;


LToplevelRole::LToplevelRole(wl_resource *toplevel, LSurface *surface) : LBaseSurfaceRole(toplevel, surface)
{
    m_imp = new LToplevelRolePrivate();
    m_imp->toplevel = this;
    m_imp->appId = new char[1];
    m_imp->title = new char[1];
    m_imp->appId[0] = '\0';
    m_imp->title[0] = '\0';

    m_imp->stateFlags = Deactivated;
    m_imp->currentConf = LToplevelRolePrivate::TopLevelConfiguration();
    m_imp->currentConf.flags = Deactivated;
    m_imp->currentConf.serial = 0;
    m_imp->currentConf.set = false;
    m_imp->currentConf.size = LSize();

    m_imp->sentConf = m_imp->currentConf;
    m_imp->pendingConf = m_imp->currentConf;

    m_imp->minSize = LSize();
    m_imp->maxSize = LSize();
    m_imp->windowGeometry = LRect();

}

LToplevelRole::~LToplevelRole()
{
    delete []m_imp->appId;
    delete []m_imp->title;
    delete m_imp;
}

UChar8 LToplevelRole::wmCapabilities() const
{
    return m_imp->wmCapabilities;
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 5
void LToplevelRole::setWmCapabilities(UChar8 capabilitiesFlags)
{
    if(wl_resource_get_version(resource()) < 5)
        return;

    m_imp->wmCapabilities = capabilitiesFlags;

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if(m_imp->wmCapabilities & WmCapabilities::WindowMenu)
    {
        wl_array_add(&dummy, sizeof(UInt32));
        UInt32 *s = (UInt32*)dummy.data;
        s[index] = XDG_TOPLEVEL_WM_CAPABILITIES_WINDOW_MENU;
        index++;
    }
    if(m_imp->wmCapabilities & WmCapabilities::Maximimize)
    {
        wl_array_add(&dummy, sizeof(UInt32));
        UInt32 *s = (UInt32*)dummy.data;
        s[index] = XDG_TOPLEVEL_WM_CAPABILITIES_MAXIMIZE;
        index++;
    }
    if(m_imp->wmCapabilities & WmCapabilities::Fullscreen)
    {
        wl_array_add(&dummy, sizeof(UInt32));
        UInt32 *s = (UInt32*)dummy.data;
        s[index] = XDG_TOPLEVEL_WM_CAPABILITIES_FULLSCREEN;
        index++;
    }
    if(m_imp->wmCapabilities & WmCapabilities::Minimize)
    {
        wl_array_add(&dummy, sizeof(UInt32));
        UInt32 *s = (UInt32*)dummy.data;
        s[index] = XDG_TOPLEVEL_WM_CAPABILITIES_MINIMIZE;
        index++;
    }

    xdg_toplevel_send_wm_capabilities(resource(), &dummy);
    wl_array_release(&dummy);
    configure(state());
}
#endif

void LToplevelRole::ping(UInt32 serial)
{
    xdg_wm_base_send_ping(surface()->client()->xdgWmBaseResource(),serial);
}


bool LToplevelRole::maximized() const
{
    return m_imp->stateFlags & Maximized;
}

bool LToplevelRole::fullscreen() const
{
    return m_imp->stateFlags & Fullscreen;
}

bool LToplevelRole::activated() const
{
    return m_imp->stateFlags & Activated;
}

LToplevelRole::States LToplevelRole::state() const
{
    return (LToplevelRole::States)m_imp->stateFlags;
}

LToplevelRole::LToplevelRolePrivate *LToplevelRole::imp() const
{
    return m_imp;
}

const char *LToplevelRole::appId() const
{
    return m_imp->appId;
}

const char *LToplevelRole::title() const
{
    return m_imp->title;
}

const LSize &LToplevelRole::minSize() const
{
    return m_imp->minSize;
}

const LSize &LToplevelRole::maxSize() const
{
    return m_imp->maxSize;
}

void LToplevelRole::configure(Int32 width, Int32 height, UChar8 stateFlags)
{
    m_imp->currentConf.set = true;
    m_imp->currentConf.size = LSize(width,height);
    m_imp->currentConf.flags = stateFlags;
}

void LToplevelRole::close()
{
    xdg_toplevel_send_close(resource());
}


#if LOUVRE_XDG_WM_BASE_VERSION >= 4
    void LToplevelRole::configureBounds(const LSize &bounds)
    {
        if(wl_resource_get_version(resource()) <= 4)
            return;
        m_imp->bounds = bounds;
        xdg_toplevel_send_configure_bounds(resource(),bounds.w(),bounds.h());
        configure(state());
    }

    const LSize &LToplevelRole::bounds() const
    {
        return m_imp->bounds;
    }
#endif

void LToplevelRole::configure(const LSize &size, UChar8 stateFlags)
{
    configure(size.w(),size.h(),stateFlags);
}

void LToplevelRole::configure(UChar8 stateFlags)
{
    configure(windowGeometry().bottomRight(),stateFlags);
}

const LRect &LToplevelRole::windowGeometry() const
{
    return m_imp->windowGeometry;
}

LSize LToplevelRole::calculateResizeRect(const LPoint &cursorPosDelta, const LSize &initialSize, Edge edge)
{
    LSize newSize = initialSize;
    switch(edge)
    {
        case Edge::Bottom:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
        }break;
        case Edge::Right:
        {
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case Edge::BottomRight:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case Edge::Top:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
        }break;
        case Edge::Left:
        {
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case Edge::TopLeft:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case Edge::BottomLeft:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case Edge::TopRight:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
    }

    return newSize;
}

void LToplevelRole::LToplevelRolePrivate::dispachLastConfiguration()
{

    if(!currentConf.set)
        return;

    currentConf.serial = LWayland::nextSerial();

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if(bool(currentConf.flags & LToplevelRole::Activated))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
        index++;
    }
    if(bool(currentConf.flags & LToplevelRole::Fullscreen))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
        index++;
    }
    if(bool(currentConf.flags & LToplevelRole::Maximized))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
        index++;
    }
    if(bool(currentConf.flags & LToplevelRole::Resizing))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_RESIZING;
        index++;
    }

#if LOUVRE_XDG_WM_BASE_VERSION >= 2
    if(wl_resource_get_version(toplevel->resource()) >= 2)
    {
        if(bool(currentConf.flags & LToplevelRole::TiledBottom))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(bool(currentConf.flags & LToplevelRole::TiledLeft))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(bool(currentConf.flags & LToplevelRole::TiledRight))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(bool(currentConf.flags & LToplevelRole::TiledTop))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }
    }
#endif

    xdg_toplevel_send_configure(toplevel->resource(),currentConf.size.w(),currentConf.size.h(),&dummy);
    wl_array_release(&dummy);


    if(toplevel->surface()->xdgSurfaceResource())
        xdg_surface_send_configure(toplevel->surface()->xdgSurfaceResource(),currentConf.serial);

    currentConf.set = false;

    sentConf = currentConf;

}

void LToplevelRole::LToplevelRolePrivate::setAppId(const char *newAppId)
{
    delete []appId;
    appId = new char[strlen(newAppId)+1];
    strcpy(appId,newAppId);
    toplevel->appIdChanged();
}

void LToplevelRole::LToplevelRolePrivate::setTitle(const char *newTitle)
{
    delete []title;
    title = new char[strlen(newTitle)+1];
    strcpy(title,newTitle);
    toplevel->titleChanged();
}

