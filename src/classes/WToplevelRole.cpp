#include "WToplevelRole.h"

#include <WSurface.h>
#include <string.h>
#include <WCompositor.h>
#include <WSeat.h>
#include <WWayland.h>

using namespace Wpp;

WToplevelRole::WToplevelRole(wl_resource *toplevel, WSurface *surface)
{
    p_resource = toplevel;
    p_surface = surface;
}

WToplevelRole::~WToplevelRole()
{

}

void WToplevelRole::startMoveRequest()
{
    seat()->startMovingTopLevel(this);
}

void WToplevelRole::startResizeRequest(Edge edge)
{
    seat()->startResizingToplevel(this,edge);
}

void WToplevelRole::titleChanged()
{

}

void WToplevelRole::appIdChanged()
{

}

void WToplevelRole::geometryChangeRequest()
{
    if(seat()->resizingToplevel() == this)
        seat()->updateResizingToplevelPos();
}

WSurface *WToplevelRole::surface() const
{
    return p_surface;
}

wl_resource *WToplevelRole::resource() const
{
    return p_resource;
}

const char *WToplevelRole::appId() const
{
    return p_appId;
}

const char *WToplevelRole::title() const
{
    return p_title;
}

const WSize &WToplevelRole::minSize() const
{
    return p_minSize;
}

const WSize &WToplevelRole::maxSize() const
{
    return p_maxSize;
}

void WToplevelRole::configure(Int32 width, Int32 height, SurfaceStateFlags states)
{
    p_pendingConfigure = true;
    p_pendingConfigureSize.setW(width);
    p_pendingConfigureSize.setH(height);
    p_pendingConfigureStates = states;
}

void WToplevelRole::configure(const WSize &size, SurfaceStateFlags states)
{
    configure(size.w(),size.h(),states);
}

void WToplevelRole::configure(SurfaceStateFlags states)
{
    configure(windowGeometry().bottomRight(),states);
}

const WRect &WToplevelRole::windowGeometry() const
{
    return p_windowGeometry;
}

WSize WToplevelRole::calculateResizeRect(const WPoint &cursorPosDelta, const WSize &initialSize, Edge edge)
{
    WSize newSize = initialSize;
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

WCompositor *WToplevelRole::compositor() const
{
    return surface()->compositor();
}

WSeat *WToplevelRole::seat() const
{
    return compositor()->seat();
}

void WToplevelRole::dispachLastConfiguration()
{
    if(!p_pendingConfigure)
        return;

    surface()->ack_configure = false;

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if((p_pendingConfigureStates & (SurfaceStateFlags)State::Activated))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
        index++;

        if(seat()->activeTopLevel() && seat()->activeTopLevel() != this)
            seat()->activeTopLevel()->configure(seat()->activeTopLevel()->p_pendingConfigureStates & ~State::Activated);

        seat()->p_activeTopLevel = this;

    }
    if(p_pendingConfigureStates & (SurfaceStateFlags)State::Fullscreen)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
        index++;
    }
    if(p_pendingConfigureStates & (SurfaceStateFlags)State::Maximized)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
        index++;
    }
    if(p_pendingConfigureStates & (SurfaceStateFlags)State::Resizing)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_RESIZING;
        index++;
    }

    if(wl_resource_get_version(resource()) >= 2)
    {
        if(p_pendingConfigureStates & (SurfaceStateFlags)State::TiledBottom)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(p_pendingConfigureStates & (SurfaceStateFlags)State::TiledLeft)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(p_pendingConfigureStates & (SurfaceStateFlags)State::TiledRight)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(p_pendingConfigureStates & (SurfaceStateFlags)State::TiledTop)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }
    }

    xdg_toplevel_send_configure(resource(),p_pendingConfigureSize.w(),p_pendingConfigureSize.h(),&dummy);
    wl_array_release(&dummy);


    if(surface()->p_xdg_shell != nullptr)
        xdg_surface_send_configure(surface()->p_xdg_shell,WWayland::nextSerial());



    p_pendingConfigure = false;

}

void WToplevelRole::setAppId(const char *appId)
{
    delete []p_appId;
    p_appId = new char[strlen(appId)+1];
    strcpy(p_appId,appId);
    appIdChanged();
}

void WToplevelRole::setTitle(const char *title)
{
    delete []p_title;
    p_title = new char[strlen(title)+1];
    strcpy(p_title,title);
    titleChanged();
}

