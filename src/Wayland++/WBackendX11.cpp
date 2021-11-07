#include <WBackendX11.h>
#include <wayland-server.h>
#include <X11/Xlib.h>
#include <linux/input.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <poll.h>
#include <WCompositor.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

static struct {
    Window window;
    EGLContext context;
    EGLSurface surface;
} window;
static Display *x_display;
static EGLDisplay egl_display;

static void create_window ()
{
    // setup EGL
    EGLint attribs[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE};
    EGLConfig config;
    EGLint num_configs_returned;
    eglChooseConfig (egl_display, attribs, &config, 1, &num_configs_returned);

    // get the visual from the EGL config
    EGLint visual_id;
    eglGetConfigAttrib (egl_display, config, EGL_NATIVE_VISUAL_ID, &visual_id);
    XVisualInfo visual_template;
    visual_template.visualid = visual_id;
    int num_visuals_returned;
    XVisualInfo *visual = XGetVisualInfo (x_display, VisualIDMask, &visual_template, &num_visuals_returned);

    // create a window
    XSetWindowAttributes window_attributes;
    window_attributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask;
    window_attributes.colormap = XCreateColormap (x_display, RootWindow(x_display,DefaultScreen(x_display)), visual->visual, AllocNone);
    window.window = XCreateWindow (
        x_display,
        RootWindow(x_display, DefaultScreen(x_display)),
        0, 0,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0, // border width
        visual->depth, // depth
        InputOutput, // class
        visual->visual, // visual
        CWEventMask|CWColormap, // attribute mask
        &window_attributes // attributes
    );

    // EGL context and surface
    if(!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Failed to bind api EGL_OPENGL_ES_API\n");
        exit(-1);
    }

    window.context = eglCreateContext (egl_display, config, EGL_NO_CONTEXT, NULL);
    if(window.context == NULL)
    {
        printf("Failed to create context\n");
        exit(-1);
    }

    window.surface = eglCreateWindowSurface (egl_display, config, window.window, NULL);
    if(window.surface == EGL_NO_SURFACE)
    {
        printf("Failed to create egl surface\n");
        exit(-1);
    }

    eglMakeCurrent (egl_display, window.surface, window.surface, window.context);

    XFree (visual);

    XMapWindow (x_display, window.window);
}

int backendWidth()
{
    return 800;
}
int backendHeight()
{
    return 600;
}

void initBackend(WCompositor *compositor)
{
    x_display = XOpenDisplay (NULL);
    egl_display = eglGetDisplay (x_display);
    eglInitialize (egl_display, NULL, NULL);
    create_window();
    compositor->initializeGL();
}

EGLDisplay getEGLDisplay()
{
    return egl_display;
}

void paintDRM()
{
    eglSwapBuffers (egl_display, window.surface);
}


