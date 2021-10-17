#ifndef WCOMPOSITOR_H
#define WCOMPOSITOR_H

#include <libinput.h>
#include <stdio.h>
#include <GLES2/gl2.h>

class WCompositor
{
public:
    WCompositor();
    void start();
    virtual void initializeGL() = 0;
    virtual void paintGL() = 0;
    virtual void libinputEvent(struct libinput_event *ev) = 0;
    void repaint();
private:
    bool readyToDraw = false;
    void updateGL();
};

#endif // WCOMPOSITOR_H
