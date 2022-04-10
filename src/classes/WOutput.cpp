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

#include <xf86drm.h>
#include <xf86drmMode.h>


#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <WBackend.h>
#include <WCompositor.h>
#include <WWayland.h>
#include <WInput.h>

using namespace WaylandPlus;

WOutput::WOutput(){}

WOutput::~WOutput()
{
    //drmModeFreeConnector(_connector);
    //delete []_devName;
}

void WOutput::setCompositor(WCompositor *compositor)
{
    _compositor = compositor;
    _renderThread = new std::thread(&WOutput::startRenderLoop,this);
    //WOutput::startRenderLoop(this);
}

void WOutput::setOutputScale(Int32 scale)
{
    _outputScale = scale;
}

Int32 WOutput::getOutputScale()
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

    _compositor->initializeGL(this);

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
    ts.it_value.tv_nsec = 1000000000/60;
    timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

    while(true)
    {

        // Wait for a repaint request
        poll(&output->_renderPoll,1,-1);

        // Block the render
        output->scheduledRepaint = false;
        eventfd_read(output->_renderFd,&output->_renderValue);

        // Let the user do his painting
        output->_compositor->renderMutex.lock();
        output->_compositor->paintGL(output);
        //WWayland::flushClients();
        //WWayland::dispatchEvents();
        output->_compositor->renderMutex.unlock();

        // Tell the input loop to process events
        eventfd_write(output->_compositor->libinputFd,1);

        // Wait for the next frame
        poll(&output->timerPoll,1,-1);

        read(output->timerPoll.fd, &res, sizeof(res));
        timerfd_settime(output->timerPoll.fd, 0, &ts, NULL);

        // Show buffer on screen
        WBackend::flipPage(output);


        //usleep(20000);
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

