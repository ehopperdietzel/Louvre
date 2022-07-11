#include "LToplevelRole.h"
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


/***************** Virtual Methods *****************/


LToplevelRole::LToplevelRole(wl_resource *toplevel, LSurface *surface) : LBaseSurfaceRole(toplevel, surface)
{
    m_appId = new char(1);
    m_title = new char(1);
    m_appId[0] = '\0';
    m_title[0] = '\0';

    m_stateFlags = Deactivated;
    m_currentConf = TopLevelConfiguration();
    m_currentConf.flags = Deactivated;
    m_currentConf.serial = 0;
    m_currentConf.set = false;
    m_currentConf.size = LSize();

    m_sentConf = m_currentConf;
    m_pendingConf = m_currentConf;

    m_minSize = LSize();
    m_maxSize = LSize();
    m_windowGeometry = LRect();
}

LToplevelRole::~LToplevelRole()
{
    delete []m_appId;
    delete []m_title;
}

const LPoint &LToplevelRole::rolePos() const
{
    baseImp()->rolePos = surface()->pos() -m_windowGeometry.topLeft();
    return baseImp()->rolePos;
}

void LToplevelRole::pong(UInt32)
{

}

void LToplevelRole::startMoveRequest()
{
    if(maximized())
    {
        // Get the main output
        LOutput *output = compositor()->cursor()->output();

        // Topbar height
        Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

        // Dest size
        LSize destSize = (output->rect().topLeft() + output->rect(false).bottomRight() - LSize(0,topbarHeight)) / 2 / output->getOutputScale();

        // Tell the toplevel to maximize
        configure(destSize, state() &~ Maximized);

        if(compositor()->cursor()->position().x() >= destSize.x())
            surface()->setPos(LPoint(destSize.w(), topbarHeight)+output->rect().topLeft());
        else
            surface()->setPos(LPoint(0, topbarHeight)+output->rect().topLeft());
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

void LToplevelRole::unsetFullscreenRequest()
{
    configure(Activated);
}

void LToplevelRole::titleChanged()
{

}

void LToplevelRole::appIdChanged()
{

}

void LToplevelRole::geometryChanged()
{

}

void LToplevelRole::ping(UInt32 serial)
{
    xdg_wm_base_send_ping(surface()->client()->xdgWmBaseResource(),serial);
}

void LToplevelRole::setMaximizedRequest()
{
    // Get the main output
    LOutput *output = compositor()->cursor()->output();

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

    // Tell the toplevel to maximize
    configure(output->rect().bottomRight() - LSize(0,topbarHeight), Activated | Maximized);

    // We now wait for the maximizeChanged() event to move it to the top left corner
}

void LToplevelRole::unsetMaximizedRequest()
{
    // Get the main output
    LOutput *output = compositor()->cursor()->output();

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
    // Get the main output
    LOutput *output = compositor()->cursor()->output();

    // Topbar height
    Int32 topbarHeight = (LOUVRE_TB_H+2)/output->getOutputScale();

    if(maximized())
    {
        compositor()->riseSurface(surface());
        surface()->setPos(output->rect().x(),output->rect().y()+topbarHeight);
        surface()->setMinimized(false);
    }
    else
    {
        if(seat()->pointer()->movingTopLevel() != this)
        {

            // Dest size
            LSize destSize = (output->rect(false).bottomRight() - LSize(0,topbarHeight)) / 2 / output->getOutputScale();

            surface()->setPos(output->rect().topLeft() + destSize/2);
        }
    }
}

void LToplevelRole::fullscreenChanged()
{
    if(fullscreen())
        surface()->setPos(0,0);
    else
        surface()->setPos(0,(LOUVRE_TB_H+2)/compositor()->outputs().front()->getOutputScale());
}

void LToplevelRole::activatedChanged()
{

}

void LToplevelRole::maxSizeChanged()
{

}

void LToplevelRole::minSizeChanged()
{

}

void LToplevelRole::setMinimizedRequest()
{
    surface()->setMinimized(true);

    if(surface() == seat()->pointer()->focusSurface())
        seat()->pointer()->setFocus(nullptr);

    if(surface() == seat()->keyboard()->focusSurface())
        seat()->keyboard()->setFocus(nullptr);

    if(this == seat()->pointer()->movingTopLevel())
        seat()->pointer()->stopMovingTopLevel();

    if(this == seat()->pointer()->resizingToplevel())
        seat()->pointer()->stopResizingToplevel();
}

void LToplevelRole::setFullscreenRequest(LOutput *destOutput)
{
    LOutput *output;

    if(destOutput == nullptr)
        output = compositor()->cursor()->output();
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
    return m_stateFlags & Maximized;
}

bool LToplevelRole::fullscreen() const
{
    return m_stateFlags & Fullscreen;
}

bool LToplevelRole::activated() const
{
    return m_stateFlags & Activated;
}

LToplevelRole::States LToplevelRole::state() const
{
    return (LToplevelRole::States)m_stateFlags;
}

const char *LToplevelRole::appId() const
{
    return m_appId;
}

const char *LToplevelRole::title() const
{
    return m_title;
}

const LSize &LToplevelRole::minSize() const
{
    return m_minSize;
}

const LSize &LToplevelRole::maxSize() const
{
    return m_maxSize;
}

void LToplevelRole::configure(Int32 width, Int32 height, UChar8 stateFlags)
{
    m_currentConf.set = true;
    m_currentConf.size = LSize(width,height);
    m_currentConf.flags = stateFlags;
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
    return m_windowGeometry;
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

    if(!m_currentConf.set)
        return;

    m_currentConf.serial = LWayland::nextSerial();

    wl_array dummy;
    wl_array_init(&dummy);
    UInt32 index = 0;

    if(bool(m_currentConf.flags & LToplevelRole::Activated))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
        index++;
    }
    if(bool(m_currentConf.flags & LToplevelRole::Fullscreen))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
        index++;
    }
    if(bool(m_currentConf.flags & LToplevelRole::Maximized))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
        index++;
    }
    if(bool(m_currentConf.flags & LToplevelRole::Resizing))
    {
        wl_array_add(&dummy, sizeof(xdg_toplevel_state));
        xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
        s[index] = XDG_TOPLEVEL_STATE_RESIZING;
        index++;
    }

#if LOUVRE_XDG_WM_BASE_VERSION >= 2
    if(wl_resource_get_version(resource()) >= 2)
    {
        if(bool(m_currentConf.flags & LToplevelRole::TiledBottom))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(bool(m_currentConf.flags & LToplevelRole::TiledLeft))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(bool(m_currentConf.flags & LToplevelRole::TiledRight))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(bool(m_currentConf.flags & LToplevelRole::TiledTop))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }
    }
#endif

    xdg_toplevel_send_configure(resource(),m_currentConf.size.w(),m_currentConf.size.h(),&dummy);
    wl_array_release(&dummy);


    if(surface()->xdgSurfaceResource())
        xdg_surface_send_configure(surface()->xdgSurfaceResource(),m_currentConf.serial);



    m_currentConf.set = false;

    m_sentConf = m_currentConf;

}

void LToplevelRole::setAppId(const char *appId)
{
    delete []m_appId;
    m_appId = new char[strlen(appId)+1];
    strcpy(m_appId,appId);
    appIdChanged();
}

void LToplevelRole::setTitle(const char *title)
{
    delete []m_title;
    m_title = new char[strlen(title)+1];
    strcpy(m_title,title);
    titleChanged();
}

