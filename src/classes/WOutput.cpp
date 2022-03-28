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
WOutput::WOutput(const char *drmDeviceName,drmModeConnector *connector,UInt32 crtcId)
{
    int devNameLen = strlen(drmDeviceName);
    _devName = new char(devNameLen+1);
    strcpy(_devName,drmDeviceName);
    _devName[devNameLen] = '\0';
    _connector = connector;
    _crtc_id = crtcId;
    o = this;
}

WOutput::~WOutput()
{
    drmModeFreeConnector(_connector);
    delete []_devName;
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

static void page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data)
{
    (void)fd;(void)frame;(void)sec;(void)usec;
    int *waiting_for_flip = (int*)data;
    *waiting_for_flip = 0;
}


void WOutput::initialize()
{
    evctx =
    {
        .version = DRM_EVENT_CONTEXT_VERSION,
        .page_flip_handler = page_flip_handler,
    };

    FD_ZERO(&fds);
    FD_SET(0, &fds);
    FD_SET(_drmFd, &fds);

    // Initilize the Generic Buffer Manager
    initializeGBM();

    // Create the Open GL ES 2 context
    initializeGL();

    // clear the color buffer
    eglSwapBuffers(eglDisplay, eglSurface);
    bo = gbm_surface_lock_front_buffer(_gbmSurface);
    _drmFb = drmGetFbFromBo(bo);

    // set mode:
    ret = drmModeSetCrtc(_drmFd, _crtc_id, _drmFb->fb_id, 0, 0, &_connector->connector_id, 1, _currentMode);

    if (ret)
    {
        printf("Failed to set mode: %s\n", strerror(errno));
        return;
    }

    _renderFd = eventfd(0,EFD_SEMAPHORE);
    _renderPoll.fd = _renderFd;
    _renderPoll.revents = 0;
    _renderPoll.events = POLLIN;

    printf("DRM backend initialized.\n");

    _compositor->initializeGL(this);

    WWayland::bindEGLDisplay(eglDisplay);

    return;
}

void WOutput::initializeGBM()
{
    _gbmDevice = gbm_create_device(_drmFd);

    _gbmSurface = gbm_surface_create(_gbmDevice,
            _currentMode->hdisplay, _currentMode->vdisplay,
            GBM_FORMAT_XRGB8888,
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!_gbmSurface)
        printf("Failed to create GBM surface.\n");
}

void WOutput::initializeGL()
{
    EGLint major, minor, n;

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;
    get_platform_display =(void *(*)(unsigned int,void*,const int*)) eglGetProcAddress("eglGetPlatformDisplayEXT");
    assert(get_platform_display != NULL);

    eglDisplay = get_platform_display(EGL_PLATFORM_GBM_KHR,_gbmDevice, NULL);

    if (!eglInitialize(eglDisplay, &major, &minor))
    {
        printf("Failed to initialize EGL display.\n");
        return;
    }

    printf("Using display %p with EGL version %d.%d\n",eglDisplay, major, minor);
    printf("EGL Version \"%s\"\n", eglQueryString(eglDisplay, EGL_VERSION));
    printf("EGL Vendor \"%s\"\n", eglQueryString(eglDisplay, EGL_VENDOR));
    printf("EGL Extensions \"%s\"\n", eglQueryString(eglDisplay, EGL_EXTENSIONS));

    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Failed to bind api EGL_OPENGL_ES_API\n");
        return;
    }

    if (!eglChooseConfig(eglDisplay, config_attribs, &eglConfig, 1, &n) || n != 1)
    {
        printf("Failed to choose config: %d\n", n);
        return;
    }

    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, context_attribs);

    if (eglContext == NULL)
    {
        printf("Failed to create GL context.\n");
        return;
    }

    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, _gbmSurface, NULL);
    if (eglSurface == EGL_NO_SURFACE) {
        printf("Failed to create EGL surface\n");
        return;
    }

    // connect the context to the surface
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    //printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

}

void WOutput::flipPage()
{
    struct gbm_bo *next_bo;
    int waiting_for_flip = 1;

    eglSwapBuffers(eglDisplay, eglSurface);
    next_bo = gbm_surface_lock_front_buffer(_gbmSurface);
    _drmFb = drmGetFbFromBo(next_bo);

    // Here you could also update drm plane layers if you want
    // hw composition

    ret = drmModePageFlip(_drmFd, _crtc_id, _drmFb->fb_id,DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
    if (ret)
    {
        printf("Failed to queue page flip: %s\n", strerror(errno));
        return;
    }

    while (waiting_for_flip)
    {

        /*
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        FD_SET(drm.fd, &fds);
        */

        ret = select(_drmFd + 1, &fds, NULL, NULL, NULL);
        if (ret < 0)
        {
            printf("Select err: %s\n", strerror(errno));
            return;
        }
        else if (ret == 0)
        {
            printf("Select timeout!\n");
            return;
        }
        else if (FD_ISSET(0, &fds))
        {
            printf("User interrupted!\n");
            break;
        }
        drmHandleEvent(_drmFd, &evctx);

    }


    // release last buffer to render on again:
    //if (gbm.surface)
    gbm_surface_release_buffer(_gbmSurface, bo);

    bo = next_bo;

}

void WOutput::startRenderLoop(void *data)
{
    WOutput *output = (WOutput*)data;

    output->initialize();
    output->repaint();

    while(true)
    {
        poll(&output->_renderPoll,1,-1);
        output->_compositor->paintGL(output);
        //eventfd_write(output->_compositor->libinputFd,1);
        eventfd_read(output->_renderFd,&output->_renderValue);
        output->flipPage();
    }
}

void WOutput::repaint()
{
    eventfd_write(_renderFd,1);
}

EGLDisplay WOutput::getDisplay()
{
    return eglDisplay;
}

const drmModeModeInfo WOutput::getCurrentMode()
{
    return *_currentMode;
}

const drmModeConnector WOutput::getCurrentConnector()
{
    return *_connector;
}

WOutput::drm_fb *WOutput::drmGetFbFromBo(gbm_bo *bo)
{
    drm_fb *fb = (drm_fb*)gbm_bo_get_user_data(bo);
    uint32_t width, height, stride, handle;

    int ret;

    if(fb)
        return fb;

    fb = (drm_fb*)calloc(1, sizeof(drm_fb));
    fb->bo = bo;

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);

    stride = gbm_bo_get_stride(bo);
    handle = gbm_bo_get_handle(bo).u32;

    ret = drmModeAddFB(_drmFd, width, height, 24, 32, stride, handle, &fb->fb_id);

    if (ret)
    {
        printf("Failed to create fb: %s\n", strerror(errno));
        free(fb);
        return nullptr;
    }

    gbm_bo_set_user_data(bo, fb, &WOutput::drmFbDestroyCallback);

    return fb;
}

void WOutput::drmFbDestroyCallback(gbm_bo *bo, void *data)
{
    (void)bo;

    drm_fb *fb = (drm_fb*)data;

    if (fb->fb_id)
        drmModeRmFB(o->_drmFd, fb->fb_id);

    free(fb);
}

/*
void WaylandPlus::WBackend::setHWCursor()
{

    gbm_bo *m_bo = nullptr;
    m_bo = gbm_bo_create(gbm.dev, 64, 64,GBM_FORMAT_ARGB8888, GBM_BO_USE_CURSOR_64X64 | GBM_BO_USE_WRITE);

    unsigned char cursorPixels[4*64*64];
    for(int i = 0; i < 4*64*64; i++)
        cursorPixels[i] = 255;
    gbm_bo_write(m_bo,cursorPixels, 4*64*64);
    uint32_t handle = gbm_bo_get_handle(m_bo).u32;
    drmModeSetCursor(drm.fd, drm.crtc_id, handle,64, 64);

}

void WBackend::setHWCursorPos(Int32 x, Int32 y)
{
    (void)x;(void)y;
    //drmModeMoveCursor(drm.fd, drm.crtc_id,x,y);
}
*/
