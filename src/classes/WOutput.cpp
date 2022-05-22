#include "WOutput.h"
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

#include <WBackend.h>
#include <WCompositor.h>
#include <WWayland.h>
#include <WOpenGL.h>
#include <WCursor.h>
#include <WToplevelRole.h>

using namespace Wpp;

WOutput::WOutput(){}

WOutput::~WOutput()
{
    //drmModeFreeConnector(_connector);
    //delete []_devName;
}

void WOutput::initializeGL()
{
    // Set clear screen color
    glClearColor(0.02, 0.282, 0.675, 1.0f);
}

void WOutput::paintGL()
{

    // Get the painter
    WOpenGL *GL = painter();

    // Clear screen
    GL->clearScreen();

    // Draw surfaces
    for(WSurface *surface : compositor()->surfaces())
    {
        if( surface->type() == WSurface::Undefined || surface->type() == WSurface::Cursor)
            continue;
   
        GL->drawTexture(surface->texture(),WRect(WPoint(),surface->texture()->size()),WRect(surface->pos(),surface->size()/surface->bufferScale()));
        surface->requestNextFrame();
    }

}

void WOutput::plugged()
{

}

void WOutput::unplugged()
{

}




WCompositor *WOutput::compositor()
{
    return p_compositor;
}

void WOutput::setCompositor(WCompositor *compositor)
{
    p_compositor = compositor;
    _renderThread = new std::thread(&WOutput::startRenderLoop,this);
    //WOutput::startRenderLoop(this);
}

void WOutput::setOutputScale(Int32 scale)
{
    _outputScale = scale;
}

Int32 WOutput::getOutputScale() const
{
    return _outputScale;
}

void WOutput::initialize()
{
    // Initialize the backend
    WBackend::createGLContext(this);

    // Repaint FD
    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;

    // Timer FD
    timerPoll.events = POLLIN;
    timerPoll.fd = timerfd_create(CLOCK_MONOTONIC,0);

    WWayland::bindEGLDisplay(WBackend::getEGLDisplay(this));

    setPainter(new WOpenGL());

    // Create cursor
    if(!compositor()->cursor())
        compositor()->p_cursor = new WCursor(this);

    initializeGL();

}

void WOutput::startRenderLoop(void *data)
{
    WOutput *output = (WOutput*)data;

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
        WWayland::forceUpdate();

        // Wait for the next frame
        poll(&output->timerPoll,1,-1);

        ssize_t r = read(output->timerPoll.fd, &res, sizeof(res));
        (void)r;
        
        timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

        // Show buffer on screen
        //if(WWayland::mainOutput() != output)
        WBackend::flipPage(output);

    }
}

void WOutput::repaint()
{
    if(!scheduledRepaint)
    {
        scheduledRepaint = true;
        eventfd_write(_renderFd,1);
    }
}

EGLDisplay WOutput::getDisplay()
{
    return WBackend::getEGLDisplay(this);
}

void WOutput::setPainter(WOpenGL *painter)
{
    p_painter = painter;
    p_painter->p_output = this;
}


