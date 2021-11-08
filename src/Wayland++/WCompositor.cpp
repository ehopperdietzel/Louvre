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


void WCompositor::mainLoop()
{

    while(true)
    {
        poll(pfds, 2, -1);

        if(pfds[0].revents & POLLIN)
            processInput();

        if(pfds[1].revents & POLLIN)
            processWayland();

        if(readyToDraw)
        {
            paintGL();
            paintDRM();
            readyToDraw = false;
        }
    }

}
