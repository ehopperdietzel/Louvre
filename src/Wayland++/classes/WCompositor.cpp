#include "WCompositor.h"
#include <stdio.h>
#include <WInput.h>
#include <WBackend.h>
#include <WWayland.h>
#include <sys/poll.h>
#include <WNamespaces.h>
#include <thread>
#include <unistd.h>
#include <signal.h>


using namespace WaylandPlus;

// Compositor and Wayland file descriptors
pollfd pfds[2];


WCompositor::WCompositor()
{
    signal(SIGINT,SIG_IGN);
}

void WCompositor::start()
{    
    // Process libinput events in another thread
    std::thread eglThrd(WInput::initInput, this);

    // Init the EGL context for OpenGL
    WBackend::initBackend(this);

    // Init wayland
    pfds[0].fd = WWayland::initWayland(this);
    pfds[0].events = POLLIN;

    // Unblock loop
    compositorFd = eventfd(0,EFD_SEMAPHORE);

    pfds[1].fd = compositorFd;
    pfds[1].events = POLLIN;
    pfds[1].revents = 0;

    mainLoop();
}

void WCompositor::setOutputScale(Int32 scale)
{
    _outputScale = scale;
}

Int32 WCompositor::getOutputScale()
{
    return _outputScale;
}

void WCompositor::repaint()
{
    eventfd_write(compositorFd,1);
}

int WCompositor::screenWidth()
{
    return WBackend::backendWidth();
}

int WCompositor::screenHeight()
{
    return WBackend::backendHeight();
}

double WCompositor::getPointerX()
{
    return _pointerX;
}

double WCompositor::getPointerY()
{
    return _pointerY;
}

void WCompositor::setPointerPos(double x, double y, UInt32 milliseconds)
{
    if(x < 0.0)
        x = 0.0;
    if(y < 0.0)
        y = 0.0;
    if(x > screenWidth()/getOutputScale())
        x = screenWidth()/getOutputScale();
    if(y > screenHeight()/getOutputScale())
        y = screenHeight()/getOutputScale();

    _pointerX = x;
    _pointerY = y;

    pointerPosChanged(x,y, milliseconds);
}

WSurface *WCompositor::getPointerFocusSurface()
{
    return _pointerFocusSurface;
}

WSurface *WCompositor::getKeyboardFocusSurface()
{
    return _keyboardFocusSurface;
}

WSurface *WCompositor::getCursorSurface()
{
    return _cursorSurface;
}

void WCompositor::clearPointerFocus()
{
    _pointerFocusSurface = nullptr;
}

void WCompositor::clearKeyboardFocus()
{
    _keyboardFocusSurface = nullptr;
}

UInt32 WCompositor::getMilliseconds()
{
    timespec endTime;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
    return endTime.tv_sec*1000 + endTime.tv_nsec/1000000;
}


void WCompositor::mainLoop()
{
    while(true)
    {
        // Wait for events
        poll(pfds, 2, -1);

        if(pfds[0].revents & POLLIN)
            WWayland::dispatchEvents();

        paintGL();

        WBackend::paintDRM();
        WWayland::flushClients();
    }
}
