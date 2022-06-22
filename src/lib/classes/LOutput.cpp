#include "LOutput.h"
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

#include <LRegion.h>

using namespace Louvre;

LOutput::LOutput(){}

LOutput::~LOutput()
{
    //drmModeFreeConnector(_connector);
    //delete []_devName;
}


void LOutput::initializeGL()
{
    // Set clear screen color
    glClearColor(0.031f, 0.486f, 0.933f, 1.0f);

}


void LOutput::paintGL()
{

    // Get the painter
    LOpenGL *GL = painter();
    /*
    GL->clearScreen();


    LRegion region;

    for(int i = 0; i < 5000; i++)
    {
        Int32 x = rand() % 1000;
        Int32 y = rand() % 1000;
        Int32 w = 10 + rand() % 1000;
        Int32 h = 10 + rand() % 1000;
        region.addRect(LRect(x,y,w,h));
    }


    for(int i = 0; i < 5; i++)
    {
        Int32 x = rand() % 1000;
        Int32 y = rand() % 1000;
        Int32 w = 10 + rand() % 1000;
        Int32 h = 10 + rand() % 1000;
        region.subtractRect(LRect(x,y,w,h));
    }



    region.clip(LRect(0,0,rect().w(),rect().h()));
    //region.subtractRect(LRect(0,0,2000,600));


    //region.addRect(LRect(0,0,100,100));
    //region.subtractRect(LRect(0,0,100,20));
    //region.subtractRect(LRect(0,80,20,20));


    for(LRect &r : region.rects())
    {
        GL->drawColor(
                    r,
                    float(rand() % 1000)/1000.f,
                    float(rand() % 1000)/1000.f,
                    float(rand() % 1000)/1000.f,
                    0.2);
    }


    for(LRect &r : region.rects())
    {
        GL->drawColor(
                    r,
                    1,//float(rand() % 1000)/1000.f,
                    0,//float(rand() % 1000)/1000.f,
                    0,//float(rand() % 1000)/1000.f,
                    0.5);
    }
    return;

    */
    for(LSurface *surface : compositor()->surfaces())
        if(surface->toplevel() && surface->toplevel()->fullscreen())
        {
            if(surface->textureChanged() || compositor()->cursor()->hasHardwareSupport())
            {
                glDisable(GL_BLEND);
                GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(surface->pos(true),surface->size()));
                /*
                LRect r;
                for(const LRect &rect : surface->texture()->damages)
                {
                    r = LRect(surface->pos(true)+rect.topLeft()/surface->bufferScale(),rect.bottomRight()/surface->bufferScale());
                    //GL->drawColor(r,1.0,0.0,0.0,0.3);
                    GL->drawTexture(surface->texture(),rect,r);
                }
                */
                surface->requestNextFrame();
            }
            return;
        }


    // Clear screen
    GL->clearScreen();

    // Store minimized surfaces
    list<LSurface*>minimized;

    // Draw surfaces
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

        // Draw the surface
        GL->drawTexture(surface->texture(),LRect(LPoint(),surface->size(true)),LRect(surface->pos(true),surface->size()));

        // Input region
        //for(const LRect &rect : surface->inputRegion().rects())
            //GL->drawColor(LRect(surface->pos(true) + rect.topLeft(), rect.bottomRight()),1,0,1,0.2);

        // Opaque region
        //for(const LRect &rect : surface->opaqueRegion().rects())
            //GL->drawColor(LRect(surface->pos(true) + rect.topLeft(), rect.bottomRight()),1,0,1,0.2);

        // Damages
        //for(const LRect &rect : surface->damages().rects())
            //GL->drawColor(LRect(surface->pos(true) + rect.topLeft(), rect.bottomRight()),1,0,1,0.2);

        /*
        LRect r;
        for(const LRect &rect : surface->texture()->damages)
        {
            r = LRect(surface->pos(true)+rect.topLeft()/surface->bufferScale(),rect.bottomRight()/surface->bufferScale());
            GL->drawColor(r,1.0,0.0,0.0,0.3);
            //GL->drawTexture(surface->texture(),rect,r);
        }
        */

        /*
        for(LRect &r : surface->texture()->damages)
        {
            printf("%i %i\n",surface->size(true).h(),surface->size(true).w());
            printf("%i %i %i %i\n",r.x(),r.y(),r.w(),r.h());
            LRect rect = r/surface->bufferScale();


            GL->drawTexture(
                        surface->texture(),
                        r,
                        LRect(surface->pos(LSurface::SubRole)+rect.topLeft(),rect.bottomRight()));

            //GL->drawColor(LRect(surface->pos(LSurface::SubRole)+rect.topLeft(),rect.bottomRight()),1,0,0,0.3);


        }
        */

        // Tell the surface to render the next frame
        surface->requestNextFrame();
    }

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

}

void LOutput::plugged()
{

}

void LOutput::unplugged()
{

}




LCompositor *LOutput::compositor()
{
    return m_compositor;
}

void LOutput::setCompositor(LCompositor *compositor)
{
    m_compositor = compositor;
    m_renderThread = new std::thread(&LOutput::startRenderLoop,this);
    //LOutput::startRenderLoop(this);
}

void LOutput::setOutputScale(Int32 scale)
{
    _outputScale = scale;
    m_rectScaled = m_rect/getOutputScale();
}

Int32 LOutput::getOutputScale() const
{
    return _outputScale;
}

void LOutput::initialize()
{
    // Initialize the backend
    compositor()->imp()->m_backend->createGLContext(this);

    // Repaint FD
    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;

    // Timer FD
    timerPoll.events = POLLIN;
    timerPoll.fd = timerfd_create(CLOCK_MONOTONIC,0);

    LWayland::bindEGLDisplay(compositor()->imp()->m_backend->getEGLDisplay(this));

    setPainter(new LOpenGL());

    // Create cursor
    if(!compositor()->cursor())
        compositor()->imp()->m_cursor = new LCursor(this);

    initializeGL();

}

void LOutput::startRenderLoop(void *data)
{
    LOutput *output = (LOutput*)data;

    output->initialize();
    output->repaint();

    uint64_t res;
    itimerspec ts;
    ts.it_interval.tv_sec = 0;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = 0;

    ts.it_value.tv_nsec = 1000000000/output->refreshRate;
    timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

    while(true)
    {

        // Wait for a repaint request
        poll(&output->_renderPoll,1,-1);

        // Block the render
        output->scheduledRepaint = false;
        eventfd_read(output->_renderFd,&output->_renderValue);

        // Let the user do his painting
        output->m_compositor->imp()->m_renderMutex.lock();

        output->paintGL();

        if(!output->compositor()->cursor()->hasHardwareSupport())
            output->compositor()->cursor()->paint();

        output->m_compositor->imp()->m_renderMutex.unlock();

        // Tell the input loop to process events
        LWayland::forceUpdate();

        // Wait for the next frame
        //poll(&output->timerPoll,1,-1);

        //(void)read(output->timerPoll.fd, &res, sizeof(res));

        //timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

        // Show buffer on screen
        output->m_compositor->imp()->m_backend->flipPage(output);

    }
}

void LOutput::repaint()
{
    if(!scheduledRepaint)
    {
        scheduledRepaint = true;
        eventfd_write(_renderFd,1);
    }
}

const LRect &LOutput::rect(bool scaled) const
{
    if(scaled)
        return m_rectScaled;

    return m_rect;
}

EGLDisplay LOutput::getDisplay()
{
    return compositor()->imp()->m_backend->getEGLDisplay(this);
}

void LOutput::setPainter(LOpenGL *painter)
{
    m_painter = painter;
    m_painter->m_output = this;
}


