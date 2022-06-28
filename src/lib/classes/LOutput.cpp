#include <LOutputPrivate.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/timerfd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <LCompositorPrivate.h>
#include <LWayland.h>
#include <LOpenGL.h>
#include <LCursor.h>
#include <LToplevelRole.h>
#include <SOIL/SOIL.h>
#include <LRegion.h>

using namespace Louvre;

LOutput::LOutput()
{
    m_imp = new LOutputPrivate();
    m_imp->m_output = this;
}

LOutput::~LOutput()
{
    delete m_imp;
}

LTexture *background;

void LOutput::initializeGL()
{
    // Set clear screen color
    glClearColor(0.031f, 0.486f, 0.933f, 1.0f);

    damage[0].addRect(rect());
    damage[1].addRect(rect());

    background = LOpenGL::loadTexture("wallpaper.png");
}

int it = 0;
void LOutput::paintGL(Int32 currentBuffer)
{

    // Get the painter
    LOpenGL *GL = painter();

    /*
    for(LSurface *surface : compositor()->surfaces())
    {
        if(surface->toplevel() && surface->toplevel()->fullscreen())
        {
            glDisable(GL_BLEND);

            // Draw surface
            GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(surface->pos(true),surface->size()));

            glEnable(GL_BLEND);

            // Ask the client to render the next frame
            surface->requestNextFrame();

            return;
        }
    }

    //GL->clearScreen();
    GL->drawTexture(background,LRect(LPoint(),background->size()),rect());

    // Store minimized surfaces
    list<LSurface*>minimized;

    for(LSurface *surface : compositor()->surfaces())
    {
        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor)
            continue;

        // Skip if minimized
        if(surface->minimized())
        {
            minimized.push_back(surface);
            continue;
        }

        // Draw surface
        GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(surface->pos(true),surface->size()));

        // Ask the client to render the next frame
        surface->requestNextFrame();
    }
    */

    for(LSurface *surface : compositor()->surfaces())
    {
        if(surface->toplevel() && surface->toplevel()->fullscreen())
        {
            glDisable(GL_BLEND);

            // Draw surface
            GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(surface->pos(true),surface->size()));

            glEnable(GL_BLEND);

            // Ask the client to render the next frame
            surface->requestNextFrame();

            return;
        }
    }


    // Store minimized surfaces
    list<LSurface*>minimized;

    LRegion backgroundDamage;
    backgroundDamage.addRect(rect());
    LPoint backgroundPos = rect().topLeft();


    // Background damage
    for(LSurface *surface : compositor()->surfaces())
    {
        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor || surface->minimized())
            continue;

        for(const LRect &r : surface->opaqueRegion().rects())
            backgroundDamage.subtractRect(LRect(surface->pos(true)+r.topLeft()-backgroundPos,r.bottomRight()));
    }


    // Draw opaque region
    glDisable(GL_BLEND);

    for(const LRect &r : backgroundDamage.rects())
        GL->drawTexture(background,r*getOutputScale(),r);


    //glDisable(GL_BLEND);
    //GL->clearScreen();


    // Step 1: Calculates convert prev and new surface damages to global
    for(list<LSurface*>::const_iterator s = compositor()->surfaces().cbegin(); s != compositor()->surfaces().cend(); s++)
    {
        LSurface *surface = *s;

        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor)
            continue;

        // Skip if minimized
        if(surface->minimized())
        {
            minimized.push_back(surface);
            continue;
        }

        LRegion opa;
        opa.copy(surface->opaqueRegion());
        opa.offset(surface->pos(true));
        LRegion tra;
        tra.copy(surface->translucentRegion());
        tra.offset(surface->pos(true));

        for(list<LSurface*>::const_iterator p = std::next(s, 1); p != compositor()->surfaces().cend(); p++)
        {
            LSurface *after = *p;

            // Skip some types surfaces
            if( after->roleType() == LSurface::Undefined || after->roleType() == LSurface::Cursor || after->minimized())
                continue;

            for(const LRect &r : after->opaqueRegion().rects())
            {
                LRect sub = LRect(after->pos(true)+r.topLeft(),r.bottomRight());
                opa.subtractRect(sub);
                tra.subtractRect(sub);
            }

        }


        // Draw opaque region
        glDisable(GL_BLEND);

        for(const LRect &r : opa.rects())
        {
            GL->drawTexture(surface->texture(),
                            LRect(r.topLeft()-surface->pos(true),r.bottomRight())*surface->bufferScale(),
                            r-LRect(backgroundPos,LSize()));
        }

        // Draw translucen region
        glEnable(GL_BLEND);

        for(const LRect &r : tra.rects())
        {
            GL->drawTexture(surface->texture(),
                            LRect(r.topLeft()-surface->pos(true),r.bottomRight())*surface->bufferScale(),
                             r-LRect(backgroundPos,LSize()));
        }

        // Ask the client to render the next frame
        surface->requestNextFrame();
    }


    // Draw top bar
    UInt32 topbarHeight = LOUVRE_TB_H/getOutputScale();
    GL->drawColor(LRect(0,0,rect().w(),topbarHeight), 1.f, 1.f, 1.f, 0.9f);

    // Draw minimized surfaces
    Int32 xOffset = 4;
    LSize thumbnailSize;
    for(LSurface *surface : minimized)
    {
        thumbnailSize = surface->size(true).constrainedToHeight(topbarHeight-8);
        GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(LPoint(xOffset,4),thumbnailSize));
        xOffset += thumbnailSize.w() + 4;
    }

    /*
    // Get the painter
    LOpenGL *GL = painter();

    Int32 prevBuffer = 1 - currentBuffer;

    // Store minimized surfaces
    list<LSurface*>minimized;

    LRegion backgroundDamage;

    if(it < 2)
    {
        backgroundDamage.addRect(rect());
        it++;
    }


    // Step 1: Calculates convert prev and new surface damages to global
    for(list<LSurface*>::const_iterator s = compositor()->surfaces().cbegin(); s != compositor()->surfaces().cend(); s++)
    {
        LSurface *surface = *s;

        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor)
            continue;

        // Skip if minimized
        if(surface->minimized())
        {
            minimized.push_back(surface);
            continue;
        }

        // Current rect of the surface
        LRect currentRect = LRect(surface->pos(true),surface->size());

        // If pos changed damages the entire surface
        if(surface->rect[prevBuffer] != currentRect || surface->rect[currentBuffer] != currentRect)
        {
            surface->damage[currentBuffer].clear();
            surface->damage[currentBuffer].addRect(LRect(LPoint(),surface->size()));
        }
        else
        {
            // Else use damages of this frame
            surface->damage[currentBuffer].copy(surface->damages());
        }

        // Prev local damages to global
        for(const LRect &r : surface->damage[prevBuffer].rects())
            surface->globalDamage.addRect(LRect(surface->pos(true)+r.topLeft(),r.bottomRight()));

        // Local damages to global
        for(const LRect &r : surface->damage[currentBuffer].rects())
            surface->globalDamage.addRect(LRect(surface->pos(true)+r.topLeft(),r.bottomRight()));

    }

    // Step 2: Calculate damage with respect to other surfaces
    for(list<LSurface*>::const_iterator s = compositor()->surfaces().cbegin(); s != compositor()->surfaces().cend(); s++)
    {
        LSurface *surface = *s;

        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor || surface->minimized())
            continue;

        for(list<LSurface*>::const_iterator p = std::next(s, 1); p != compositor()->surfaces().cend(); p++)
        {
            LSurface *after = *p;

            // Current rect of the surface
            LRect currentRect = LRect(after->pos(true),after->size());

            // Skip some types surfaces
            if( after->roleType() == LSurface::Undefined || after->roleType() == LSurface::Cursor || after->minimized())
                continue;

            // If surface in front changed pos or size
            if(currentRect != after->rect[prevBuffer] || currentRect != after->rect[currentBuffer])
            {
                surface->globalDamage.addRect(currentRect);
                surface->globalDamage.addRect(after->rect[prevBuffer]);
                surface->globalDamage.addRect(after->rect[currentBuffer]);
            }
            else
            {
                for(const LRect &r : after->globalDamage.rects())
                    surface->globalDamage.addRect(r);
            }

            // Remove opaque areas
            for(const LRect &r : after->opaqueRegion().rects())
                surface->globalDamage.subtractRect(LRect(after->pos(true)+r.topLeft(),r.bottomRight()));


            // Add the damage to the surface after (to force it to repaint transparent areas)
            for(const LRect &r : surface->globalDamage.rects())
                after->globalDamage.addRect(r);
        }

    }

    // Step 3: calculate background damage
    for(LSurface *surface : compositor()->surfaces())
    {
        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor || surface->minimized())
            continue;

        // Current rect of the surface
        LRect currentRect = LRect(surface->pos(true),surface->size());

        // Clip to surface size
        surface->globalDamage.clip(currentRect);

        // Clip to screen rect
        surface->globalDamage.clip(rect());

        // If pos changed damages the entire surface
        if(surface->rect[prevBuffer] != currentRect || surface->rect[currentBuffer] != currentRect)
        {
            backgroundDamage.addRect(currentRect);
            backgroundDamage.addRect(surface->rect[prevBuffer]);
            backgroundDamage.addRect(surface->rect[currentBuffer]);
        }
        else
        {
            for(const LRect &r : surface->globalDamage.rects())
                backgroundDamage.addRect(r);
        }

        for(const LRect &r : surface->opaqueRegion().rects())
            backgroundDamage.subtractRect(LRect(surface->pos(true)+r.topLeft(),r.bottomRight()));
    }

    // Draw background
    for(const LRect &r : backgroundDamage.rects())
        GL->drawColor(r,0.5,0.5,0,1);

    // Draw the surfaces
    for(LSurface *surface : compositor()->surfaces())
    {
        // Skip some types surfaces
        if( surface->roleType() == LSurface::Undefined || surface->roleType() == LSurface::Cursor || surface->minimized())
            continue;

        // Current rect of the surface
        LRect currentRect = LRect(surface->pos(true),surface->size());

        // Save current rect
        surface->rect[currentBuffer] = currentRect;

        // Draw the surface
        for(const LRect &r : surface->globalDamage.rects())
        {
            GL->drawTexture(
                        surface->texture(),
                        LRect(r.topLeft()-surface->pos(true),r.bottomRight())*surface->bufferScale(),
                        r);
        }


        // Clear damages of prev frame
        surface->globalDamage.clear();

        // Ask the client to render the next frame
        surface->requestNextFrame();
    }

    return;
    // Draw top bar
    UInt32 topbarHeight = LOUVRE_TB_H/getOutputScale();
    GL->drawColor(LRect(0,0,rect().w(),topbarHeight), 1.f, 1.f, 1.f, 0.7f);

    // Draw minimized surfaces
    Int32 xOffset = 4;
    LSize thumbnailSize;
    for(LSurface *surface : minimized)
    {
        thumbnailSize = surface->size(true).constrainedToHeight(topbarHeight-8);
        GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(LPoint(xOffset,4),thumbnailSize));
        xOffset += thumbnailSize.w() + 4;
    }
    */


    if(sc)
    {
        sc = false;
        UChar8 *pix = new UChar8[2880*1800*4];
        glReadPixels(0,0,2880,1800,GL_RGBA,GL_UNSIGNED_BYTE,pix);

        SOIL_save_image("/home/eduardo/Desktop/sh.bmp",SOIL_SAVE_TYPE_BMP,2880,1800,4,pix);

        delete []pix;
    }
}

void LOutput::plugged()
{

}

void LOutput::unplugged()
{

}




LCompositor *LOutput::compositor()
{
    return m_imp->m_compositor;
}

void LOutput::LOutputPrivate::setCompositor(LCompositor *compositor)
{
    m_compositor = compositor;
    m_renderThread = new std::thread(&LOutputPrivate::startRenderLoop,m_output);
    //LOutput::startRenderLoop(this);
}

void LOutput::setOutputScale(Int32 scale)
{
    m_imp->_outputScale = scale;
    m_imp->m_rectScaled = LRect(m_imp->m_rect.topLeft(),m_imp->m_rect.bottomRight()/getOutputScale());
}

Int32 LOutput::getOutputScale() const
{
    return m_imp->_outputScale;
}

void LOutput::LOutputPrivate::initialize()
{
    // Initialize the backend
    m_compositor->imp()->m_backend->createGLContext(m_output);

    // Repaint FD
    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;

    // Timer FD
    timerPoll.events = POLLIN;
    timerPoll.fd = timerfd_create(CLOCK_MONOTONIC,0);

    LWayland::bindEGLDisplay(m_compositor->imp()->m_backend->getEGLDisplay(m_output));

    m_output->setPainter(new LOpenGL());

    // Create cursor
    if(!m_compositor->cursor())
        m_compositor->imp()->m_cursor = new LCursor(m_output);

    m_output->initializeGL();

}

void LOutput::LOutputPrivate::startRenderLoop(void *data)
{
    LOutput *output = (LOutput*)data;

    output->m_imp->initialize();
    output->repaint();

    uint64_t res;
    itimerspec ts;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = 0;

    ts.it_value.tv_nsec = 1000000000/output->refreshRate;
    timerfd_settime(output->m_imp->timerPoll.fd, 0, &ts, NULL);

    Int32 currentBuffer = 0;

    while(true)
    {

        // Wait for a repaint request
        poll(&output->m_imp->_renderPoll,1,-1);

        // Check if disconnected
        if(output->imp()->m_initializeResult == LOutput::Pending)
        {
            output->imp()->m_initializeResult = LOutput::Stopped;
            return;
        }

        // Block the render
        output->m_imp->scheduledRepaint = false;
        eventfd_read(output->m_imp->_renderFd,&output->m_imp->_renderValue);

        // Let the user do his painting
        output->m_imp->m_compositor->imp()->m_renderMutex.lock();

        output->paintGL(currentBuffer);
        currentBuffer = 1 - currentBuffer;

        if(!output->compositor()->cursor()->hasHardwareSupport())
            output->compositor()->cursor()->paint();

        output->m_imp->m_compositor->imp()->m_renderMutex.unlock();

        // Tell the input loop to process events
        LWayland::forceUpdate();

        // Wait for the next frame
        poll(&output->imp()->timerPoll,1,-1);

        (void)read(output->imp()->timerPoll.fd, &res, sizeof(res));

        timerfd_settime(output->imp()->timerPoll.fd, 0, &ts, NULL);

        // Show buffer on screen
        output->m_imp->m_compositor->imp()->m_backend->flipPage(output);

    }
}

void LOutput::repaint()
{
    if(!m_imp->scheduledRepaint)
    {
        m_imp->scheduledRepaint = true;
        eventfd_write(m_imp->_renderFd,1);
    }
}

const LRect &LOutput::rect(bool scaled) const
{
    if(scaled)
        return m_imp->m_rectScaled;

    return m_imp->m_rect;
}

EGLDisplay LOutput::getDisplay()
{
    return compositor()->imp()->m_backend->getEGLDisplay(this);
}

LOutput::InitializeResult LOutput::initializeResult() const
{
    return m_imp->m_initializeResult;
}

void LOutput::setPos(const LPoint &pos)
{
    m_imp->m_rect.setX(pos.x());
    m_imp->m_rect.setY(pos.y());

    m_imp->m_rectScaled.setX(pos.x());
    m_imp->m_rectScaled.setY(pos.y());
}


void LOutput::setUserData(void *userData)
{
    m_imp->m_userData = userData;
}

void *LOutput::userData() const
{
    return m_imp->m_userData;
}

void LOutput::setPainter(LOpenGL *painter)
{
    m_imp->m_painter = painter;
    m_imp->m_painter->m_output = this;
}

LOpenGL *LOutput::painter() const
{
    return m_imp->m_painter;
}

void *LOutput::getData() const
{
    return m_imp->data;
}

LOutput::LOutputPrivate *LOutput::imp() const
{
    return m_imp;
}


