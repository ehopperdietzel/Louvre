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

#include <LBackend.h>
#include <LCompositor.h>
#include <LOutputPrivate.h>
#include <LWayland.h>
#include <LSizeF.h>
#include <LCursor.h>

using namespace Louvre;

std::list<LOutput*>outputs;

struct X11_Window
{
    Window window;
    EGLContext context;
    EGLSurface surface;
    Int32 width;
    Int32 height;
};

struct X11
{
    X11_Window window;
    Display *x_display;
    EGLDisplay egl_display;
    Int32 windowNumber;
};


static void create_window(LOutput *output)
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

    // Get the screen size
    int snum = DefaultScreen(data->x_display);


#if LOUBRE_DEBUG == 0
    data->window.width = DisplayWidth(data->x_display, snum);
    data->window.height = DisplayHeight(data->x_display, snum);
#else
    data->window.width = DisplayWidth(data->x_display, snum)/2;
    data->window.height = DisplayHeight(data->x_display, snum)/2;
#endif

    // create a window
    XSetWindowAttributes window_attributes;
    window_attributes.event_mask = ExposureMask | StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask;
    window_attributes.colormap = XCreateColormap (data->x_display, RootWindow(data->x_display,DefaultScreen(data->x_display)), visual->visual, AllocNone);
    data->window.window = XCreateWindow (
        data->x_display,
        RootWindow(data->x_display, DefaultScreen(data->x_display)),
        0, 0,
        data->window.width, data->window.height,
        0, // border width
        visual->depth, // depth
        InputOutput, // class
        visual->visual, // visual
        CWEventMask|CWColormap, // attribute mask
        &window_attributes // attributes
    );


    // Make it fullscreen
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

    if(LWayland::isGlContextInitialized())
        ctx = LWayland::eglContext();

    data->window.context = eglCreateContext(data->egl_display, config, ctx, context_attribs);

    if(data->window.context == NULL)
    {
        printf("Failed to create context.\n");
        exit(-1);
    }

    /*
    static const EGLint surf_attribute_list[] = {
       //EGL_RENDER_BUFFER, EGL_SINGLE_BUFFER,
        EGL_SWAP_BEHAVIOR,  EGL_BUFFER_PRESERVED,
        EGL_NONE // attribute list is termintated with EGL_NONE
    };
    */

    data->window.surface = eglCreateWindowSurface(data->egl_display, config, data->window.window, NULL);// surf_attribute_list);

    if(data->window.surface == EGL_NO_SURFACE)
    {
        printf("Failed to create egl surface.\n");
        exit(-1);
    }

    eglMakeCurrent(data->egl_display, data->window.surface, data->window.surface, data->window.context);

    XFree (visual);

    XMapWindow(data->x_display, data->window.window);
    XMoveWindow(data->x_display,data->window.window,0,0);

#if LOUBRE_DEBUG == 0
    XFixesHideCursor(data->x_display, data->window.window);
    XSetInputFocus(data->x_display,data->window.window,RevertToParent,CurrentTime);
#endif



    printf("X11 Window created.\n");

    if(!LWayland::isGlContextInitialized())
        LWayland::setContext(output,data->egl_display,data->window.context);

    //eglSwapBuffers(data->egl_display, data->window.surface);
}



void LBackend::initialize(LCompositor *compositor)
{

}

std::list<LOutput *> *LBackend::getAvaliableOutputs(LCompositor *compositor)
{

    LOutput *output = compositor->createOutputRequest();
    output->imp()->data = malloc(sizeof(X11));
    output->refreshRate = 55;
    X11 *data = (X11*)output->imp()->data;
    data->x_display = XOpenDisplay(NULL);
    data->egl_display = eglGetDisplay(data->x_display);
    data->windowNumber = 0;
    outputs.push_back(output);

    return &outputs;
}

EGLDisplay LBackend::getEGLDisplay(LOutput *output)
{
    X11 *data = (X11*)output->imp()->data;
    return data->egl_display;
}

void LBackend::createGLContext(LOutput *output)
{
    //XInitThreads();
    create_window(output);
    X11 *data = (X11*)output->imp()->data;
    output->imp()->m_rect.setW(data->window.width);
    output->imp()->m_rect.setH(data->window.height);

    output->imp()->m_rectScaled = output->imp()->m_rect/output->getOutputScale();
    output->imp()->m_initializeResult = Louvre::LOutput::InitializeResult::Initialized;
    printf("X11 backend initialized.\n");
}


void LBackend::flipPage(LOutput *output)
{
    X11 *data = (X11*)output->imp()->data;
    eglSwapBuffers(data->egl_display, data->window.surface);
}

bool LBackend::hasHardwareCursorSupport()
{
    return false;
}

void LBackend::initializeCursor(LOutput *output)
{

}

void LBackend::setCursor(LOutput *, LTexture *, const LSizeF&)
{

}
void LBackend::setCursorPosition(LOutput *output, const LPoint &)
{

    //X11 *data = (X11*)output->data;
    //XWarpPointer(data->x_display, None, data->window.window, 0, 0, 0, 0, output->rect(false).w()/2, output->rect(false).h()/2);

}


LGraphicBackend LBackendAPI;

extern "C" LGraphicBackend *getAPI()
{
   printf("Using Louvre X11 backend.\n");
   LBackendAPI.initialize               = &LBackend::initialize;
   LBackendAPI.getAvaliableOutputs      = &LBackend::getAvaliableOutputs;
   LBackendAPI.getEGLDisplay            = &LBackend::getEGLDisplay,
   LBackendAPI.createGLContext          = &LBackend::createGLContext;
   LBackendAPI.flipPage                 = &LBackend::flipPage;
   LBackendAPI.hasHardwareCursorSupport = &LBackend::hasHardwareCursorSupport;
   LBackendAPI.initializeCursor         = &LBackend::initializeCursor;
   LBackendAPI.setCursor                = &LBackend::setCursor;
   LBackendAPI.setCursorPosition        = &LBackend::setCursorPosition;

   return &LBackendAPI;
}

