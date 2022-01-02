#ifndef WWAYLAND_H
#define WWAYLAND_H

#include <WTypes.h>
#include <wayland-server.h>

class WCompositor;

int initWayland(WCompositor *comp);
void terminateDisplay();
void processWayland();



#endif // WWAYLAND_H
