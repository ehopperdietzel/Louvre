#include "WOutput.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <xf86drm.h>
#include <xf86drmMode.h>


#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <WBackend.h>
#include <WCompositor.h>
#include <WWayland.h>

using namespace WaylandPlus;

WOutput *o;
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

    // Fd for repaint event
    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;


    WWayland::bindEGLDisplay(WBackend::getEGLDisplay(this));

    _compositor->initializeGL(this);


    return;
}

void WOutput::startRenderLoop(void *data)
{
    WOutput *output = (WOutput*)data;

    output->initialize();
    output->repaint();

    while(true)
    {
        // Listen for the repaint event
        poll(&output->_renderPoll,1,-1);

        // Let user do his drawing
        output->_compositor->paintGL(output);

        // Block the render
        eventfd_read(output->_renderFd,&output->_renderValue);

        // Tell the input loop to process events
        eventfd_write(output->_compositor->libinputFd,1);

        // Show buffer on screen
        WBackend::flipPage(output);
    }
}

void WOutput::repaint()
{
    eventfd_write(_renderFd,1);
}

EGLDisplay WOutput::getDisplay()
{
    return WBackend::getEGLDisplay(this);
}


