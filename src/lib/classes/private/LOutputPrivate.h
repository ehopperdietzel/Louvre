#ifndef LOUTPUTPRIVATE_H
#define LOUTPUTPRIVATE_H

#include <LOutput.h>

class Louvre::LOutput::LOutputPrivate
{

public:
    LOutputPrivate() = default;
    ~LOutputPrivate() = default;

    LOutput *m_output;
    LRect m_rect;
    LRect m_rectScaled;

    // Painter
    LOpenGL *m_painter = nullptr;

    InitializeResult m_initializeResult = InitializeResult::Pending;

    void *m_userData;

    wl_event_source *timer;
    timespec lastDrawTime;
    pollfd timerPoll;

    // Data structure allocated by the backend
    void *data = nullptr;

    // Setup Methods
    void setCompositor(LCompositor *compositor);
    void initialize();

    // Paint
    static void startRenderLoop(void *data);

    // Compositor
    LCompositor *m_compositor = nullptr;


    // Params
    char *_devName = nullptr;
    //drmModeConnector *_connector = nullptr;
    UInt32 _crtc_id = 0;
    Int32 _outputScale = 1;


    // Render thread
    int _renderFd;
    eventfd_t _renderValue = 0;
    std::thread *m_renderThread;
    pollfd _renderPoll;

    bool scheduledRepaint = false;

    // Setup

};

#endif // LOUTPUTPRIVATE_H
