#include "WCompositor.h"
#include <stdio.h>
#include <WInput.h>
#include <WBackend.h>
#include <WWayland.h>
#include <sys/poll.h>
#include <WNamespaces.h>
#include <thread>
#include <unistd.h>

using namespace WaylandPlus;


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

    wl_event_loop_fd_func_t libinputFunc;

    int libinputFd = WInput::initInput(this,&libinputFunc);

    pfds[1].fd = libinputFd;
    pfds[1].events = POLLIN;
    pfds[1].revents = 0;

    // Bind the EGL context for OpenGL
    WBackend::initBackend(this);
    //std::thread eglThrd(WCompositor::eglThread, this);

    // Bind wayland

    pfds[0].fd = WWayland::initWayland(this,libinputFd,&libinputFunc);
    pfds[0].events = POLLIN;

    /*
    compositorFd = eventfd(0,EFD_SEMAPHORE);

    pfds[2].fd = compositorFd;
    pfds[2].events = POLLIN;
    pfds[2].revents = 0;
    */
    readyToDraw = true;

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
    readyToDraw = true;
    //cv.notify_one();
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

WSurface *WCompositor::getCursorSurface()
{
    return cursorSurface;
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
        //poll(&pfd, 1, -1);

        /*
        if(pfds[2].revents & POLLIN)
        {
            eventfd_read(compositorFd,&val);
            //printf("JAJA%lu\n",val);
            //printf("Wayland:%i\n",pfds[2].revents);
        }
        */
        /*
        while(!surfacesToRelease.empty())
        {
            wl_buffer_send_release(surfacesToRelease.front()->buffer);

            if (surfacesToRelease.front()->frame_callback != nullptr)
            {

                wl_callback_send_done(surfacesToRelease.front()->frame_callback,getMilliseconds());
                wl_resource_destroy(surfacesToRelease.front()->frame_callback);
                surfacesToRelease.front()->frame_callback = nullptr;
            }

            surfacesToRelease.pop();
        }
        */

        //eventfd_read(compositorFd,&val);

        //eventfd_write(compositorFd,0);

        //if(pfds[0].revents & POLLIN)
        WInput::processInput();

        //if(pfds[1].revents & POLLIN)
        WWayland::processWayland();





        //cv.notify_one();



        //eventfd_read(compositorFd,NULL);

        if(readyToDraw)
        {
            paintGL();
            WBackend::paintDRM();
            readyToDraw = false;
        }

    }

}

void WCompositor::addTextureToRingBuffer(WSurface *surface, Int32 width, Int32 height, void *data, WTexture::Type textureType)
{
    texturesToProcess.push({surface,width,height,data,textureType});
}

void WCompositor::eglThread(WCompositor *comp)
{
    WBackend::initBackend(comp);
    WCompositor::TextureRingBufferElement e;
    std::mutex mtx;
    std::unique_lock<std::mutex> lk(mtx);
    bool dam = false;
    while(true)
    {

        comp->cv.wait(lk);

        // Create and damage the textures
        while(!comp->texturesToProcess.empty())
        {
            e = comp->texturesToProcess.front();

            e.surface->_texture->setData(
               e.width,
               e.height,
               e.data,
               e.textureType
            );

            //comp->surfacesToRelease.push(e.surface);
            comp->texturesToProcess.pop();
            dam = true;
            comp->readyToDraw = true;


            wl_buffer_send_release(e.surface->buffer);

            if (e.surface->frame_callback != nullptr)
            {

                wl_callback_send_done(e.surface->frame_callback,comp->getMilliseconds());
                wl_resource_destroy(e.surface->frame_callback);
                e.surface->frame_callback = nullptr;
            }

            //surfacesToRelease.pop();

        }

        if(dam)
            eventfd_write(comp->compositorFd,1);

        dam = false;

        if(comp->readyToDraw)
        {
            comp->paintGL();
            WBackend::paintDRM();
            comp->readyToDraw = false;
        }


    }
}
