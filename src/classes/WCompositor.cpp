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
#include <WSeat.h>

using namespace Wpp;

WCompositor::WCompositor()
{
    //signal(SIGINT,SIG_IGN);
    //signal(SIGABRT,SIG_IGN);

    // Store the main thread id for later use (in WCursor)
    p_threadId = std::this_thread::get_id();

    libinputFd = eventfd(0,EFD_SEMAPHORE);
    WWayland::initWayland(this);
}

void WCompositor::start()
{    
    if(_started)
        return;

    // Ask the developer to return a WSeat
    p_seat = configureSeat();
    p_seat->p_compositor = this;
    WWayland::setSeat(p_seat);

    // Init wayland
    _started = true;
    WWayland::runLoop();
}

void WCompositor::repaintAllOutputs()
{
    for(list<WOutput*>::iterator it = p_outputs.begin(); it != p_outputs.end(); ++it)
        (*it)->repaint();
}

void WCompositor::addOutput(WOutput *output)
{
    // Add the output to the compositor list
    p_outputs.push_back(output);

    // This method inits the Output rendering thread and its OpenGL context
    output->setCompositor(this);

    // If the main thread has no OpenGL context yet
    if(!WWayland::isGlContextInitialized())
    {
        // Wait for the added output to create his OpenGL context in his own thread
        while(output->initializeResult() == WOutput::Pending)
            usleep(100);

        /* The next method creates a shared OpenGL context in the main thread.
         * This context is used only to allow the library to copy the surfaces buffers
         * into OpenGL textures from the main thread and release clients buffers
         * immediatly to allow them to reuse it.
         * This fix the Qt clients decoration bug while resizing. */
        WWayland::initGLContext();
    }
}

void WCompositor::removeOutput(WOutput *output)
{

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


