#ifndef WOUTPUT_H
#define WOUTPUT_H

#include <WNamespaces.h>
#include <thread>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <EGL/egl.h>

#include <sys/eventfd.h>
#include <sys/poll.h>
#include <WSize.h>

class Wpp::WOutput
{

public:

    enum InitializeResult
    {
        Pending = 0,
        Initialized = 1,
        Failed = 2
    };

    WOutput();
    virtual ~WOutput();


    virtual void initializeGL();
    virtual void paintGL();
    virtual void plugged();
    virtual void unplugged();

    WCompositor *compositor();

    void setOutputScale(Int32 scale);
    Int32 getOutputScale() const;
    void repaint();

    EGLDisplay getDisplay();
    //const drmModeModeInfo getCurrentMode();
    //const drmModeConnector getCurrentConnector();

    InitializeResult initializeResult() const { return p_initializeResult; }

    UInt32 refreshRate = 60;

    WSize size;

    void setUserData(void *userData){p_userData = userData;}
    void *userData() const {return p_userData;}

    void setPainter(WOpenGL *painter);
    WOpenGL *painter() const {return p_painter;}

    void *getData() const { return data; };

private:

    friend class WBackend;
    friend class WOutputManager;
    friend class WCompositor;

    // Painter
    WOpenGL *p_painter = nullptr;

    InitializeResult p_initializeResult = InitializeResult::Pending;

    void *p_userData;

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
    WCompositor *p_compositor = nullptr;

   
    // Params
    char *_devName = nullptr;
    //drmModeConnector *_connector = nullptr;
    UInt32 _crtc_id = 0;
    Int32 _outputScale = 1;
    

    // Render thread
    int _renderFd;
    eventfd_t _renderValue = 0;
    std::thread *_renderThread;
    pollfd _renderPoll;

    bool scheduledRepaint = false;

    // Setup

};

#endif // WOUTPUT_H
