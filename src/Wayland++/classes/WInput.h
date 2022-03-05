#ifndef WINPUT_H
#define WINPUT_H

#include <WNamespaces.h>

class WaylandPlus::WInput
{
public:
    static int initInput(WCompositor *compositor, wl_event_loop_fd_func_t *libinputFunc);
    //static int processInput(int, unsigned int, void *);
    static int processInput();
    static int getKeymapFD(bool privated);
    static int getKeymapSize();
};



#endif // WINPUT_H
