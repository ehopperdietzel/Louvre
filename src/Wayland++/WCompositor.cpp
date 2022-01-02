#include "WCompositor.h"
#include <stdio.h>
#include <WInput.h>
#include <WBackend.h>
#include <WWayland.h>
#include <sys/poll.h>

// Libinput and Wayland file descriptors
pollfd pfds[2];

WCompositor::WCompositor()
{

    // Initialize milliseconds clock
    clock_gettime(CLOCK_MONOTONIC_RAW, &startTime);
}

void WCompositor::start()
{

    // Bind the libinput events
    pfds[0].fd = initInput(this);
    pfds[0].events = POLLIN;
    pfds[0].revents = 0;

    // Bind the EGL context for OpenGL
    initBackend(this);

    // Bind wayland
    pfds[1].fd = initWayland(this);
    pfds[1].events = POLLIN;

    readyToDraw = true;

    mainLoop();
}

void WCompositor::repaint()
{
    readyToDraw = true;
}

int WCompositor::screenWidth()
{
    return backendWidth();
}

int WCompositor::screenHeight()
{
    return backendHeight();
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
    if(x > screenWidth())
        x = screenWidth();
    if(y > screenHeight())
        y = screenHeight();

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
    return (endTime.tv_sec - startTime.tv_sec) * 1000000 + (endTime.tv_nsec - startTime.tv_nsec) / 1000;
}


void WCompositor::mainLoop()
{

    while(true)
    {
        poll(pfds, 2, -1);

        //if(pfds[0].revents & POLLIN)
            processInput();

        //if(pfds[1].revents & POLLIN)
            processWayland();

        if(readyToDraw)
        {
            paintGL();
            paintDRM();
            readyToDraw = false;
        }
    }

}
