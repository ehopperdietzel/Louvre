#include "WCompositor.h"
#include <stdio.h>
#include <WInput.h>
#include <WBackendX11.h>
#include <WWayland.h>


WCompositor::WCompositor()
{

}


void WCompositor::start()
{

    // Bind the libinput events
    initInput(this);

    // Bind the EGL context for OpenGL
    initBackend(this);

    // Bind wayland
    initWayland(this);

    readyToDraw = true;

    updateGL();
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


void WCompositor::updateGL()
{
    while(!readyToDraw){}
    paintGL();
    paintDRM();
    readyToDraw = false;
    updateGL();
}
