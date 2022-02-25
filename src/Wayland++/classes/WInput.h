#ifndef WINPUT_H
#define WINPUT_H

#include <WNamespaces.h>

class WaylandPlus::WInput
{
public:
    static int initInput(WCompositor *compositor);
    static void processInput();
    static int getKeymapFD(bool privated);
    static int getKeymapSize();
};



#endif // WINPUT_H
