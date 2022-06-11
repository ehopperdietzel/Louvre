#include "LToplevelRole.h"

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

using namespace Louvre;


/***************** Virtual Methods *****************/


LToplevelRole::LToplevelRole(wl_resource *toplevel, LSurface *surface) : LBaseSurfaceRole(toplevel, surface)
{
    p_appId = new char(1);
    p_title = new char(1);
    p_appId[0] = '\0';
    p_title[0] = '\0';

    p_stateFlags = Deactivated;
    p_currentConf = TopLevelConfiguration();
    p_currentConf.flags = Deactivated;
    p_currentConf.serial = 0;
    p_currentConf.set = false;
    p_currentConf.size = LSize();

    p_sentConf = p_currentConf;
    p_pendingConf = p_currentConf;

    p_minSize = LSize();
    p_maxSize = LSize();
    p_windowGeometry = LRect();
}

LToplevelRole::~LToplevelRole()
{
    delete []p_appId;
    delete []p_title;
}

const LPoint &LToplevelRole::rolePos() const
{
    p_rolePos = surface()->pos() -p_windowGeometry.topLeft();
    return p_rolePos;
}

void LToplevelRole::pong(UInt32)
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
        configure(destSize, state() &~ Maximized);

        if(compositor()->cursor()->position().x() >= destSize.x())
            surface()->setPos(destSize.w(), topbarHeight);
        else
            surface()->setPos(0, topbarHeight);
    }
    
    seat()->pointer()->startMovingTopLevel(this);
}

void LToplevelRole::startResizeRequest(Edge edge)
{
    seat()->pointer()->startResizingToplevel(this,edge);
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

void LToplevelRole::ping(UInt32 serial)
{
    xdg_wm_base_send_ping(surface()->client()->xdgWmBaseResource(),serial);
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
    configure(destSize, state() &~ Maximized);

    // Now we wait for the maximizeChanged()

    printf("Unmaximize\n");

}

void LToplevelRole::maximizeChanged()
{

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/compositor()->outputs().front()->getOutputScale();

    if(maximized())
    {
        compositor()->riseSurface(surface());
        surface()->setPos(0,topbarHeight);
        surface()->setMinimized(false);
    }
    else
    {
        if(seat()->pointer()->movingTopLevel() != this)
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
    if(fullscreen())
    {
        surface()->setPos(0,0);
    }
    else
    {
        surface()->setPos(0,200);
    }
}

void LToplevelRole::activatedChanged()
{

}

void LToplevelRole::minimizeRequest()
{
    surface()->setMinimized(true);

    if(surface() == seat()->pointer()->focusSurface())
        seat()->pointer()->setFocus(nullptr);

    if(surface() == seat()->keyboardFocusSurface())
        seat()->setKeyboardFocus(nullptr);

    if(this == seat()->pointer()->movingTopLevel())
        seat()->pointer()->stopMovingTopLevel();

    if(this == seat()->pointer()->resizingToplevel())
        seat()->pointer()->stopResizingToplevel();
}

void LToplevelRole::fullscreenRequest(LOutput *destOutput)
{
    LOutput *output;

    if(destOutput == nullptr)
        output = compositor()->outputs().front();
    else
        output = destOutput;

    configure(output->rect().bottomRight(), Activated | Fullscreen);
}

void LToplevelRole::showWindowMenuRequest(Int32 /*x*/, Int32 /*y*/)
{

}

/***************** Normal Methods *****************/


bool LToplevelRole::maximized() const
{
    return p_stateFlags & Maximized;
}

bool LToplevelRole::fullscreen() const
{
    return p_stateFlags & Fullscreen;
}

bool LToplevelRole::activated() const
{
    return p_stateFlags & Activated;
}

LToplevelRole::States LToplevelRole::state() const
{
    return (LToplevelRole::States)p_stateFlags;
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

void LToplevelRole::configure(Int32 width, Int32 height, UChar8 stateFlags)
{
    p_currentConf.set = true;
    p_currentConf.size = LSize(width,height);
    p_currentConf.flags = stateFlags;
}

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

void LToplevelRole::dispachLastConfiguration()
{

    if(!p_currentConf.set)
        return;

    p_currentConf.serial = LWayland::nextSerial();

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if(bool(p_currentConf.flags & LToplevelRole::Activated))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
        index++;
    }
    if(bool(p_currentConf.flags & LToplevelRole::Fullscreen))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
        index++;
    }
    if(bool(p_currentConf.flags & LToplevelRole::Maximized))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
        index++;
    }
    if(bool(p_currentConf.flags & LToplevelRole::Resizing))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_RESIZING;
        index++;
    }

#if LOUVRE_XDG_WM_BASE_VERSION >= 2
    if(wl_resource_get_version(resource()) >= 2)
    {
        if(bool(p_currentConf.flags & LToplevelRole::TiledBottom))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(bool(p_currentConf.flags & LToplevelRole::TiledLeft))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(bool(p_currentConf.flags & LToplevelRole::TiledRight))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(bool(p_currentConf.flags & LToplevelRole::TiledTop))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }
    }
#endif

    xdg_toplevel_send_configure(resource(),p_currentConf.size.w(),p_currentConf.size.h(),&dummy);
    wl_array_release(&dummy);


    if(surface()->p_xdgSurfaceResource != nullptr)
        xdg_surface_send_configure(surface()->p_xdgSurfaceResource,p_currentConf.serial);



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

