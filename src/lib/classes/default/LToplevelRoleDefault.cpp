#include <LCompositor.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LKeyboard.h>
#include <LToplevelRolePrivate.h>
#include <LBaseSurfaceRolePrivate.h>

using namespace Louvre;

const LPoint &LToplevelRole::rolePos() const
{
    baseImp()->rolePos = surface()->pos() -m_imp->windowGeometry.topLeft();
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

    if(destOutput)
        output = destOutput;
    else
        output = compositor()->cursor()->output();


    configure(output->rect().bottomRight(), Activated | Fullscreen);
}

void LToplevelRole::showWindowMenuRequest(Int32 /*x*/, Int32 /*y*/)
{

}
