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
#include <WSizeF.h>

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
    Int32 windowNumber;
};

static void create_window(WOutput *output)
{

    X11 *data = (X11*)output->getData();

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

    eglInitialize(data->egl_display, NULL, NULL);

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

    output->size = WSize(W_WIDTH, W_HEIGHT);


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

    EGLContext ctx = EGL_NO_CONTEXT;

    if(WWayland::isGlContextInitialized())
        ctx = WWayland::eglContext();

    data->window.context = eglCreateContext(data->egl_display, config, ctx, context_attribs);

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

    if(data->windowNumber == 0)
        XMoveWindow(data->x_display,data->window.window,0,0);
    else
        XMoveWindow(data->x_display,data->window.window,W_WIDTH/2,0);

    XFixesHideCursor(data->x_display, data->window.window);

    XSetInputFocus(data->x_display,data->window.window,RevertToParent,CurrentTime);

    printf("X11 Window created.\n");

    if(!WWayland::isGlContextInitialized())
        WWayland::setContext(output,data->egl_display,data->window.context);

}



std::list<WOutput*> WBackend::getAvaliableOutputs()
{
    std::list<WOutput*>outputs;

    WOutput *output = new WOutput();
    output->data = malloc(sizeof(X11));
    output->refreshRate = 60;
    X11 *data = (X11*)output->data;
    data->x_display = XOpenDisplay(NULL);
    data->egl_display = eglGetDisplay(data->x_display);
    data->windowNumber = 0;
    outputs.push_back(output);


    /*
    WOutput *output1 = new WOutput();
    output1->refreshRate = 10;
    output1->data = malloc(sizeof(X11));
    X11 *data1 = (X11*)output1->data;
    data1->x_display = data->x_display;
    data1->egl_display = data->egl_display;
    data1->windowNumber = 1;
    outputs.push_back(output1);
    */

    return outputs;
}

EGLDisplay WBackend::getEGLDisplay(WOutput *output)
{
    X11 *data = (X11*)output->data;
    return data->egl_display;
}

void WBackend::createGLContext(WOutput *output)
{
    create_window(output);
    output->p_initializeResult = Wpp::WOutput::InitializeResult::Initialized;
    printf("X11 backend initialized.\n");
}

void WBackend::flipPage(WOutput *output)
{
    X11 *data = (X11*)output->data;
    eglSwapBuffers(data->egl_display, data->window.surface);
    XWarpPointer(data->x_display, None, data->window.window, 0, 0, 0, 0, W_WIDTH/2, W_HEIGHT/2);
}

bool WBackend::hasHardwareCursorSupport()
{
    return false;
}

void WBackend::setCursor(WOutput *, WTexture *, const WSizeF&)
{

}
void WBackend::setCursorPosition(WOutput *, const WPoint&)
{

}

#endif
