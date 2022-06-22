#ifndef LOUTPUT_H
#define LOUTPUT_H

#include <LNamespaces.h>
#include <thread>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>

#include <sys/eventfd.h>
#include <sys/poll.h>
#include <LSize.h>
#include <LRect.h>

class Louvre::LOutput
{

public:

    enum InitializeResult
    {
        Pending = 0,
        Initialized = 1,
        Failed = 2
    };

    LOutput();
    virtual ~LOutput();


    virtual void initializeGL();
    virtual void paintGL();
    virtual void plugged();
    virtual void unplugged();

    LCompositor *compositor();

    void setOutputScale(Int32 scale);
    Int32 getOutputScale() const;
    void repaint();

    const LRect &rect(bool scaled = true) const;

    EGLDisplay getDisplay();
    //const drmModeModeInfo getCurrentMode();
    //const drmModeConnector getCurrentConnector();

    InitializeResult initializeResult() const { return m_initializeResult; }

    UInt32 refreshRate = 60;

    void setUserData(void *userData){m_userData = userData;}
    void *userData() const {return m_userData;}

    void setPainter(LOpenGL *painter);
    LOpenGL *painter() const {return m_painter;}

    void *getData() const { return data; };

private:

    friend class LBackend;
    friend class LOutputManager;
    friend class LCompositor;

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

#endif // LOUTPUT_H
