#include "WToplevelRole.h"

#include <WSurface.h>
#include <string.h>
#include <WCompositor.h>
#include <WSeat.h>
#include <WWayland.h>
#include <WOutput.h>
#include <WPoint.h>

using namespace Wpp;


/***************** Virtual Methods *****************/


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

    if(maximized())
        configure(state()&~Maximized);
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

}

void WToplevelRole::maximizeRequest()
{
    // Get the main output
    WOutput *output = compositor()->outputs().front();

    // Tell the toplevel to maximize
    configure(output->size / output->getOutputScale(), Activated | Maximized);

    // We now wait for the maximizeChanged() event to move it to the top left corner
}

void WToplevelRole::unmaximizeRequest()
{
    // Get the main output
    WOutput *output = compositor()->outputs().front();

    // Tell the toplevel to maximize
    configure(output->size / output->getOutputScale() - WSize(200,200), Activated);

    // We now wait for the maximizeChanged() event to move it to (200,200)

    printf("Unmaximize\n");

}

void WToplevelRole::maximizeChanged()
{
    if(maximized())
        surface()->setPos(0,0);
    else
    {
        if(seat()->movingTopLevel() != this)
            surface()->setPos(100,100);
    }
}

/***************** Normal Methods *****************/


bool WToplevelRole::maximized() const
{
    return (p_stateFlags & Maximized);
}

WSurface *WToplevelRole::surface() const
{
    return p_surface;
}

WToplevelRole::StateFlags WToplevelRole::state() const
{
    return p_stateFlags;
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

void WToplevelRole::configure(Int32 width, Int32 height, StateFlags states)
{
    p_currentConf.set = true;
    p_currentConf.size.setW(width);
    p_currentConf.size.setH(height);
    p_currentConf.flags = states;
}

void WToplevelRole::configure(const WSize &size, StateFlags states)
{
    configure(size.w(),size.h(),states);
}

void WToplevelRole::configure(StateFlags states)
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
    if(!p_currentConf.set)
        return;

    p_currentConf.serial = WWayland::nextSerial();

    surface()->ack_configure = false;

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if((p_currentConf.flags & (StateFlags)Activated))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
        index++;

        if(seat()->activeTopLevel() && seat()->activeTopLevel() != this)
            seat()->activeTopLevel()->configure(seat()->activeTopLevel()->p_currentConf.flags & ~Activated);

        seat()->p_activeTopLevel = this;

    }
    if(p_currentConf.flags & (StateFlags)Fullscreen)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
        index++;
    }
    if(p_currentConf.flags & (StateFlags)Maximized)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
        index++;
    }
    if(p_currentConf.flags & (StateFlags)Resizing)
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_RESIZING;
        index++;
    }

    if(wl_resource_get_version(resource()) >= 2)
    {
        if(p_currentConf.flags & (StateFlags)TiledBottom)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(p_currentConf.flags & (StateFlags)TiledLeft)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(p_currentConf.flags & (StateFlags)TiledRight)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(p_currentConf.flags & (StateFlags)TiledTop)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }
    }

    xdg_toplevel_send_configure(resource(),p_currentConf.size.w(),p_currentConf.size.h(),&dummy);
    wl_array_release(&dummy);


    if(surface()->p_xdg_shell != nullptr)
        xdg_surface_send_configure(surface()->p_xdg_shell,p_currentConf.serial);



    p_currentConf.set = false;

    p_sentConf = p_currentConf;

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

