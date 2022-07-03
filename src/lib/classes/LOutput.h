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
#include <LRegion.h>

class Louvre::LOutput
{

public:
    bool sc = false;

    LRegion damage[3];

    enum InitializeResult
    {
        Pending = 0,
        Initialized = 1,
        Failed = 2,
        Stopped = 3
    };

    enum LCurrentBuffer : UInt32
    {
        Front = 0,
        Back = 1
    };

    LOutput();
    virtual ~LOutput();

    virtual void initializeGL();
    virtual void paintGL(Int32 currentBuffer);
    virtual void plugged();
    virtual void unplugged();

    LCompositor *compositor();

    void setOutputScale(Int32 scale);
    Int32 getOutputScale() const;
    void repaint();

    const LRect &rect(bool scaled = true) const;

    EGLDisplay getDisplay();
    InitializeResult initializeResult() const;
    const char *name() const;

    UInt32 refreshRate = 60;

    void setPos(const LPoint &pos);

    void setUserData(void *userData);
    void *userData() const;

    void setPainter(LOpenGL *painter);
    LOpenGL *painter() const;

    void *getData() const;

    class LOutputPrivate;
    LOutputPrivate *imp() const;
private:

    LOutputPrivate *m_imp = nullptr;

};

#endif // LOUTPUT_H
