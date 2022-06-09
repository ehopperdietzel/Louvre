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

#include <LBackend.h>
#include <LCompositor.h>
#include <LWayland.h>
#include <LOpenGL.h>
#include <LCursor.h>
#include <LToplevelRole.h>

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
    return p_compositor;
}

void LOutput::setCompositor(LCompositor *compositor)
{
    p_compositor = compositor;
    p_renderThread = new std::thread(&LOutput::startRenderLoop,this);
    //LOutput::startRenderLoop(this);
}

void LOutput::setOutputScale(Int32 scale)
{
    _outputScale = scale;
    p_rectScaled = p_rect/getOutputScale();
}

Int32 LOutput::getOutputScale() const
{
    return _outputScale;
}

void LOutput::initialize()
{
    // Initialize the backend
    LBackend::createGLContext(this);

    // Repaint FD
    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;

    // Timer FD
    timerPoll.events = POLLIN;
    timerPoll.fd = timerfd_create(CLOCK_MONOTONIC,0);

    LWayland::bindEGLDisplay(LBackend::getEGLDisplay(this));

    setPainter(new LOpenGL());

    // Create cursor
    if(!compositor()->cursor())
        compositor()->p_cursor = new LCursor(this);

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
        output->p_compositor->renderMutex.lock();

        output->paintGL();

        if(!output->compositor()->cursor()->hasHardwareSupport())
            output->compositor()->cursor()->paint();

        output->p_compositor->renderMutex.unlock();

        // Tell the input loop to process events
        LWayland::forceUpdate();

        // Wait for the next frame
        poll(&output->timerPoll,1,-1);

        ssize_t r = read(output->timerPoll.fd, &res, sizeof(res));
        (void)r;
        
        timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

        // Show buffer on screen
        //if(LWayland::mainOutput() != output)
        LBackend::flipPage(output);

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
        return p_rectScaled;

    return p_rect;
}

EGLDisplay LOutput::getDisplay()
{
    return LBackend::getEGLDisplay(this);
}

void LOutput::setPainter(LOpenGL *painter)
{
    p_painter = painter;
    p_painter->p_output = this;
}


