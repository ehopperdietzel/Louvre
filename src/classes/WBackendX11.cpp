#include <WBackend.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
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
#include <WOutput.h>
#include <WWayland.h>

#if W_BACKEND == 2

using namespace Wpp;

struct X11_Window{
    Window window;
    EGLContext context;
    EGLSurface surface;
};

struct X11
{
    X11_Window window;
    Display *x_display;
    EGLDisplay egl_display;
};

static void create_window(X11 *data)
{

    static const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs_returned;

    eglChooseConfig(data->egl_display, attribs, &config, 1, &num_configs_returned);

    // get the visual from the EGL config
    EGLint visual_id;
    eglGetConfigAttrib(data->egl_display, config, EGL_NATIVE_VISUAL_ID, &visual_id);
    XVisualInfo visual_template;
    visual_template.visualid = visual_id;
    int num_visuals_returned;
    XVisualInfo *visual = XGetVisualInfo(data->x_display, VisualIDMask, &visual_template, &num_visuals_returned);

    // create a window
    XSetWindowAttributes window_attributes;
    window_attributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask;
    window_attributes.colormap = XCreateColormap (data->x_display, RootWindow(data->x_display,DefaultScreen(data->x_display)), visual->visual, AllocNone);
    data->window.window = XCreateWindow (
        data->x_display,
        RootWindow(data->x_display, DefaultScreen(data->x_display)),
        0, 0,
        W_WIDTH, W_HEIGHT,
        0, // border width
        visual->depth, // depth
        InputOutput, // class
        visual->visual, // visual
        CWEventMask|CWColormap, // attribute mask
        &window_attributes // attributes
    );

    unsigned long valuemask = CWOverrideRedirect;
    XSetWindowAttributes attributes;
    attributes.override_redirect = True;
    XChangeWindowAttributes(data->x_display, data->window.window, valuemask, &attributes);

    // EGL context and surface
    if(!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Failed to bind api EGL_OPENGL_ES_API\n");
        exit(-1);
    }

    data->window.context = eglCreateContext(data->egl_display, config, EGL_NO_CONTEXT, context_attribs);

    if(data->window.context == NULL)
    {
        printf("Failed to create context.\n");
        exit(-1);
    }

    data->window.surface = eglCreateWindowSurface(data->egl_display, config, data->window.window, NULL);

    if(data->window.surface == EGL_NO_SURFACE)
    {
        printf("Failed to create egl surface.\n");
        exit(-1);
    }

    eglMakeCurrent(data->egl_display, data->window.surface, data->window.surface, data->window.context);

    XFree (visual);

    XMapWindow(data->x_display, data->window.window);

    XMoveWindow(data->x_display,data->window.window,0,0);

    XFixesHideCursor(data->x_display, data->window.window);

    XSetInputFocus(data->x_display,data->window.window,RevertToParent,CurrentTime);

    printf("X11 Window created.\n");

    WWayland::setContext(data->egl_display,data->window.context);
}



std::list<WOutput*> WBackend::getAvaliableOutputs()
{
    std::list<WOutput*>outputs;
    WOutput *output = new WOutput();
    output->data = malloc(sizeof(X11));
    X11 *data = (X11*)output->data;
    data->x_display = XOpenDisplay(NULL);
    data->egl_display = eglGetDisplay(data->x_display);
    outputs.push_back(output);
    return outputs;
}

EGLDisplay WBackend::getEGLDisplay(WOutput *output)
{
    X11 *data = (X11*)output->data;
    return data->egl_display;
}

void WBackend::createGLContext(WOutput *output)
{

    X11 *data = (X11*)output->data;
    eglInitialize(data->egl_display, NULL, NULL);
    create_window(data);
    printf("X11 backend initialized.\n");
    //compositor->initializeGL();
}

void WBackend::flipPage(WOutput *output)
{
    X11 *data = (X11*)output->data;
    eglSwapBuffers(data->egl_display, data->window.surface);
    XWarpPointer(data->x_display, None, data->window.window, 0, 0, 0, 0, W_WIDTH/2, W_HEIGHT/2);
}

#endif
