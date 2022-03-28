#ifndef WOUTPUT_H
#define WOUTPUT_H

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm/drm.h>
#include <WNamespaces.h>
#include <thread>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>

#include <sys/eventfd.h>
#include <sys/poll.h>


class WaylandPlus::WOutput
{

public:

    struct drm_fb
    {
        gbm_bo *bo;
        uint32_t fb_id;
    };

    WOutput(const char *drmDeviceName,drmModeConnector *connector,UInt32 crtcId);
    ~WOutput();
    void setOutputScale(Int32 scale);
    Int32 getOutputScale();
    void repaint();

    EGLDisplay getDisplay();
    const drmModeModeInfo getCurrentMode();
    const drmModeConnector getCurrentConnector();

private:

    friend class WOutputManager;
    friend class WCompositor;

    // Setup Methods
    void setCompositor(WCompositor *compositor);
    void initialize();
    void initializeGBM();
    void initializeGL();

    // Paint
    void flipPage();
    static void startRenderLoop(void *data);


    drm_fb *drmGetFbFromBo(gbm_bo *bo);
    static void drmFbDestroyCallback(struct gbm_bo *bo, void *data);

    // Compositor
    WaylandPlus::WCompositor *_compositor = nullptr;

    // Params
    char *_devName = nullptr;
    drmModeConnector *_connector = nullptr;
    UInt32 _crtc_id = 0;
    Int32 _outputScale = 1;

    // DRM
    int _drmFd;
    drmModeModeInfo *_currentMode;
    drm_fb *_drmFb = nullptr;

    // DRM and GBM
    drmEventContext evctx = {};
    gbm_bo *bo;
    int ret;
    fd_set fds;

    // GBM
    gbm_device *_gbmDevice;
    gbm_surface *_gbmSurface;

    // GL
    EGLDisplay eglDisplay;
    EGLConfig eglConfig;
    EGLContext eglContext;
    EGLSurface eglSurface;

    // Render thread
    int _renderFd;
    eventfd_t _renderValue = 1;
    std::thread *_renderThread;
    pollfd _renderPoll;

    // Setup

};

#endif // WOUTPUT_H
