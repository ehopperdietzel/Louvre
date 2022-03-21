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

    // Init wayland
    libinputFd = eventfd(0,EFD_SEMAPHORE);

    // Render loop
    renderFd = eventfd(0,EFD_SEMAPHORE);
    std::thread renderThrd(&WCompositor::renderLoop, this);

    //WBackend::initBackend(this);
    WInput::initInput(this);
    WWayland::initWayland(this);

    /*
    pfds[0].fd = WWayland::initWayland(this);
    pfds[0].revents = 0;
    pfds[0].events = POLLIN;
    pfds[1].fd = waylandFd;
    pfds[1].revents = 0;
    pfds[1].events = POLLIN;




    // Process libinput events in another thread
    libinputFd = eventfd(0,EFD_SEMAPHORE);
    std::thread eglThrd(WInput::initInput, this);

    mainLoop();
    */
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
    //WWayland::scheduleDraw(this);
    eventfd_write(renderFd,1);
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

timespec WCompositor::getNanoseconds()
{
    timespec endTime;
    clock_gettime(CLOCK_MONOTONIC_RAW, &endTime);
    return endTime;
}

void WCompositor::renderLoop(WCompositor *comp)
{
    WBackend::initBackend(comp);

    pollfd p;
    p.fd = comp->renderFd;
    p.revents = 0;
    p.events = POLLIN;

    while(true)
    {
        poll(&p,1,-1);
        comp->paintGL();
        eventfd_read(comp->renderFd,&comp->renderVal);
        eventfd_write(comp->libinputFd,1);
        WBackend::paintDRM();
    }
}
