#include "LToplevel.h"

#include <LSurface.h>
#include <string.h>
#include <LCompositor.h>
#include <LSeat.h>
#include <LWayland.h>
#include <LOutput.h>
#include <LPoint.h>
#include <LCursor.h>

using namespace Louvre;


/***************** Virtual Methods *****************/


LToplevelRole::LToplevelRole(wl_resource *toplevel, LSurface *surface)
{
    p_resource = toplevel;
    p_surface = surface;
}

LToplevelRole::~LToplevelRole()
{

}

void LToplevelRole::startMoveRequest()
{
    if(maximized())
    {
        // Get the main output
        LOutput *output = compositor()->outputs().front();

        // Topbar height
        Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

        // Dest size
        LSize destSize = (output->rect(false).bottomRight() - LSize(0,topbarHeight)) / 2 / output->getOutputScale();

        // Tell the toplevel to maximize
        configure(destSize, state()&~Maximized);

        if(compositor()->cursor()->position().x() >= destSize.x())
            surface()->setPos(destSize.w(), topbarHeight);
        else
            surface()->setPos(0, topbarHeight);
    }
    
    seat()->startMovingTopLevel(this);
}

void LToplevelRole::startResizeRequest(Edge edge)
{
    seat()->startResizingToplevel(this,edge);
}

void LToplevelRole::configureRequest()
{
    configure(Activated);
}

void LToplevelRole::titleChanged()
{

}

void LToplevelRole::appIdChanged()
{

}

void LToplevelRole::geometryChangeRequest()
{

}

void LToplevelRole::maximizeRequest()
{
    // Get the main output
    LOutput *output = compositor()->outputs().front();

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

    // Tell the toplevel to maximize
    configure(output->rect(false).bottomRight() / output->getOutputScale() - LSize(0,topbarHeight), Activated | Maximized);

    // We now wait for the maximizeChanged() event to move it to the top left corner
}

void LToplevelRole::unmaximizeRequest()
{
    // Get the main output
    LOutput *output = compositor()->outputs().front();

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

    // Dest size
    LSize destSize = (output->rect(false).bottomRight() - LSize(0,topbarHeight)) / 2 / output->getOutputScale();

    // Tell the toplevel to maximize
    configure(destSize, state()&~Maximized);

    // Now we wait for the maximizeChanged()

    printf("Unmaximize\n");

}

void LToplevelRole::maximizeChanged()
{

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/compositor()->outputs().front()->getOutputScale();

    if(maximized())
    {
        surface()->setPos(0,topbarHeight);
        surface()->setMinimized(false);
    }
    else
    {
        if(seat()->movingTopLevel() != this)
        {
            // Get the main output
            LOutput *output = compositor()->outputs().front();

            // Dest size
            LSize destSize = (output->rect(false).bottomRight() - LSize(0,topbarHeight)) / 2 / output->getOutputScale();

            surface()->setPos(destSize/2);
        }
    }
}

void LToplevelRole::fullscreenChanged()
{
    return;
    if(maximized())
    {
        surface()->setPos(0,0);
        surface()->setMinimized(false);
    }
}

void LToplevelRole::activatedChanged()
{

}

void LToplevelRole::minimizeRequest()
{
    surface()->setMinimized(true);

    if(surface() == seat()->pointerFocusSurface())
        seat()->setPointerFocus(nullptr);

    if(surface() == seat()->keyboardFocusSurface())
        seat()->setKeyboardFocus(nullptr);

    if(this == seat()->movingTopLevel())
        seat()->stopMovingTopLevel();

    if(this == seat()->resizingToplevel())
        seat()->stopResizingToplevel();
}

void LToplevelRole::fullscreenRequest(LOutput *destOutput)
{
    LOutput *output;

    if(destOutput == nullptr)
        output = compositor()->outputs().front();
    else
        output = destOutput;

    configure(output->rect(false).bottomRight() / output->getOutputScale(), Activated | Fullscreen);
}


/***************** Normal Methods *****************/


bool LToplevelRole::maximized() const
{
    return (p_stateFlags & Maximized);
}

bool LToplevelRole::fullscreen() const
{
    return (p_stateFlags & Fullscreen);
}

bool LToplevelRole::activated() const
{
    return (p_stateFlags & Activated);
}


LSurface *LToplevelRole::surface() const
{
    return p_surface;
}

LToplevelRole::StateFlags LToplevelRole::state() const
{
    return p_stateFlags;
}

wl_resource *LToplevelRole::resource() const
{
    return p_resource;
}

const char *LToplevelRole::appId() const
{
    return p_appId;
}

const char *LToplevelRole::title() const
{
    return p_title;
}

const LSize &LToplevelRole::minSize() const
{
    return p_minSize;
}

const LSize &LToplevelRole::maxSize() const
{
    return p_maxSize;
}

void LToplevelRole::configure(Int32 width, Int32 height, StateFlags states)
{
    p_currentConf.set = true;
    p_currentConf.size.setW(width);
    p_currentConf.size.setH(height);
    p_currentConf.flags = states;
}

void LToplevelRole::configure(const LSize &size, StateFlags states)
{
    configure(size.w(),size.h(),states);
}

void LToplevelRole::configure(StateFlags states)
{
    configure(windowGeometry().bottomRight(),states);
}

const LRect &LToplevelRole::windowGeometry() const
{
    return p_windowGeometry;
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

LCompositor *LToplevelRole::compositor() const
{
    return surface()->compositor();
}

LSeat *LToplevelRole::seat() const
{
    return compositor()->seat();
}

void LToplevelRole::dispachLastConfiguration()
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

void LToplevelRole::setAppId(const char *appId)
{
    delete []p_appId;
    p_appId = new char[strlen(appId)+1];
    strcpy(p_appId,appId);
    appIdChanged();
}

void LToplevelRole::setTitle(const char *title)
{
    delete []p_title;
    p_title = new char[strlen(title)+1];
    strcpy(p_title,title);
    titleChanged();
}

