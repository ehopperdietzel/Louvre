#include "WCompositor.h"
#include <stdio.h>
#include <WInput.h>
#include <WBackendDRM.h>

WCompositor::WCompositor()
{

}

void WCompositor::start()
{
    initInput(this);
    initBackend(this);
    readyToDraw = true;
    updateGL();
}

void WCompositor::repaint()
{
    readyToDraw = true;
}

void WCompositor::updateGL()
{
    while(!readyToDraw){}
    paintGL();
    paintDRM();
    readyToDraw = false;
    updateGL();
}
