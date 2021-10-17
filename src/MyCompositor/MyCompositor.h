#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>

class MyCompositor : public WCompositor
{
public:
    MyCompositor();
    void initializeGL() override;
    void paintGL() override;
    void libinputEvent(struct libinput_event *ev) override;
};

#endif // MYCOMPOSITOR_H
