#ifndef WBACKENDX11_H
#define WBACKENDX11_H

#include <EGL/egl.h>
#include <EGL/eglext.h>

class WCompositor;

void initBackend(WCompositor *compositor);
void paintDRM();
int backendWidth();
int backendHeight();
EGLDisplay getEGLDisplay();


#endif // WBACKENDX11_H
