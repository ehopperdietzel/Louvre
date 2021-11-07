#ifndef WBACKENDDRM_H
#define WBACKENDDRM_H

#include <EGL/egl.h>
#include <EGL/eglext.h>

class WCompositor;

void initBackend(WCompositor *compositor);
void paintDRM();
int backendWidth();
int backendHeight();

EGLDisplay getEGLDisplay();

#endif // WBACKENDDRM_H
