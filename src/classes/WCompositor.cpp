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
#include <WOutput.h>

using namespace WaylandPlus;

WCompositor::WCompositor()
{
    signal(SIGINT,SIG_IGN);
    //signal(SIGABRT,SIG_IGN);
    libinputFd = eventfd(0,EFD_SEMAPHORE);
    WInput::initInput(this);
    WWayland::initWayland(this);
}

void WCompositor::start()
{    
    if(_started)
        return;

    // Init wayland
    _started = true;
    WWayland::runLoop();
}

void WCompositor::repaintAllOutputs()
{
    for(list<WOutput*>::iterator it = _outputs.begin(); it != _outputs.end(); ++it)
        (*it)->repaint();
}

void WCompositor::addOutput(WOutput *output)
{
    _outputs.push_back(output);
    output->setCompositor(this);
}

void WCompositor::removeOutput(WOutput *output)
{

}

const list<WOutput *> WCompositor::getOutputs()
{
    return _outputs;
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


