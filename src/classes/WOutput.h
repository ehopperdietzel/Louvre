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

    WOutput();
    ~WOutput();
    void setOutputScale(Int32 scale);
    Int32 getOutputScale();
    void repaint();

    EGLDisplay getDisplay();
    const drmModeModeInfo getCurrentMode();
    const drmModeConnector getCurrentConnector();

private:

    friend class WBackend;
    friend class WOutputManager;
    friend class WCompositor;

    wl_event_source *timer;
    timespec lastDrawTime;
    pollfd timerPoll;

    // Data structure allocated by the backend
    void *data = nullptr;

    // Setup Methods
    void setCompositor(WCompositor *compositor);
    void initialize();

    // Paint
    static void startRenderLoop(void *data);

    // Compositor
    WCompositor *_compositor = nullptr;

    // Params
    char *_devName = nullptr;
    drmModeConnector *_connector = nullptr;
    UInt32 _crtc_id = 0;
    Int32 _outputScale = 1;


    // Render thread
    int _renderFd;
    eventfd_t _renderValue = 1;
    std::thread *_renderThread;
    pollfd _renderPoll;

    bool scheduledRepaint = false;

    // Setup

};

#endif // WOUTPUT_H
