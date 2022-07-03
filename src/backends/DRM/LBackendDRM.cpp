#include <sys/epoll.h>

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <fcntl.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm.h>

#include <LBackend.h>
#include <LCompositorPrivate.h>
#include <LOutputPrivate.h>
#include <LOutputManagerPrivate.h>
#include <LWayland.h>
#include <LOpenGL.h>
#include <LSizeF.h>
#include <LTime.h>

using namespace Louvre;

// Store the found outputs
std::list<LOutput*>outputs;

struct GL_CONF
{
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    GLuint program;
    GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
    GLuint vbo;
    GLuint positionsoffset, colorsoffset, normalsoffset;
};

struct GBM
{
    struct gbm_device *dev;
    struct gbm_surface *surface;
};

struct drm_fb {
    struct gbm_bo *bo;
    uint32_t fb_id;
};

struct DRM
{
    int deviceFd;
    const char *deviceName;
    drmModeConnector *connector;
    uint32_t crtc_id;
    drmModeEncoder *encoder;
    drmModeModeInfo *mode;

    drmEventContext evctx = {};
    gbm_bo *bo;
    drm_fb *fb;
    int ret;
    fd_set fds;

    GBM gbm;
    GL_CONF gl;

    gbm_bo *cursor_bo = nullptr;
    uint32_t cursorBoHandleU32;
    EGLImage cursorEGLImage;
    GLuint cursorTexture;
    GLuint cursoFramebuffer = 0;
    bool cursorVisible = false;
    bool cursorInitialized = false;
};

struct FB_DATA
{
    drm_fb *fb;
    DRM *data;
};

struct LDevice
{
    LCompositor *lCompositor;
    int fd;
    struct udev *udev;
    udev_monitor *monitor;
    udev_device *dev = nullptr;
}lDevice;

static int init_gbm(DRM *data)
{

    if(LWayland::isGlContextInitialized())
    {
        LOutput *mainOutput = LWayland::mainOutput();
        DRM *mainOutputData = (DRM*)mainOutput->getData();
        data->gbm.dev = mainOutputData->gbm.dev;
    }
    else
        data->gbm.dev = gbm_create_device(data->deviceFd);

    data->gbm.surface = gbm_surface_create(
        data->gbm.dev,
        data->mode->hdisplay, data->mode->vdisplay,
        GBM_FORMAT_XRGB8888,
        GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);

    if (!data->gbm.surface)
    {
        printf("Failed to create gbm surface.\n");
        return -1;
    }

    return 0;
}

static int init_gl(DRM *data, LOutput *output)
{

    EGLint major, minor, n;

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };


    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;
    get_platform_display =(void *(*)(unsigned int,void*,const int*)) eglGetProcAddress("eglGetPlatformDisplayEXT");
    assert(get_platform_display != NULL);

    if(LWayland::isGlContextInitialized())
        data->gl.display = LWayland::eglDisplay();
    else
        data->gl.display = get_platform_display(EGL_PLATFORM_GBM_KHR, data->gbm.dev, NULL);

    if (!eglInitialize(data->gl.display, &major, &minor))
    {
        printf("Failed to initialize EGL.\n");
        return -1;
    }

    printf("Using display %p with EGL version %d.%d.\n", data->gl.display, major, minor);
    printf("EGL Version \"%s\"\n", eglQueryString(data->gl.display, EGL_VERSION));
    printf("EGL Vendor \"%s\"\n", eglQueryString(data->gl.display, EGL_VENDOR));
    printf("EGL Extensions \"%s\"\n", eglQueryString(data->gl.display, EGL_EXTENSIONS));

    if (!eglBindAPI(EGL_OPENGL_ES_API))
    {
        printf("Failed to bind api EGL_OPENGL_ES_API.\n");
        return -1;
    }

    if (!eglChooseConfig(data->gl.display, config_attribs, &data->gl.config, 1, &n) || n != 1)
    {
        printf("failed to choose config: %d.\n", n);
        return -1;
    }

    EGLContext ctx = EGL_NO_CONTEXT;

    if(LWayland::isGlContextInitialized())
        ctx = LWayland::eglContext();

    data->gl.context = eglCreateContext(data->gl.display, data->gl.config, ctx, context_attribs);

    if (data->gl.context == NULL)
    {
        printf("Failed to create context.\n");
        return -1;
    }

    data->gl.surface = eglCreateWindowSurface(data->gl.display, data->gl.config, data->gbm.surface, NULL);

    if (data->gl.surface == EGL_NO_SURFACE)
    {
        printf("Failed to create EGL surface.\n");
        return -1;
    }

    // connect the context to the surface
    eglMakeCurrent(data->gl.display, data->gl.surface, data->gl.surface, data->gl.context);

    if(!LWayland::isGlContextInitialized())
        LWayland::setContext(output, data->gl.display,data->gl.context);

    //printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

    return 0;
}

static void drm_fb_destroy_callback(gbm_bo *bo, void *data)
{
    (void)bo;

    FB_DATA *fb_data = (FB_DATA*)data;

    drm_fb *fb = fb_data->fb;

    if (fb->fb_id)
        drmModeRmFB(fb_data->data->deviceFd, fb->fb_id);

    free(fb);
    free(fb_data);
}

static struct FB_DATA * drm_fb_get_from_bo(gbm_bo *bo, DRM *data)
{
    FB_DATA *fb_data = (FB_DATA*)gbm_bo_get_user_data(bo);


    if(!fb_data)
    {
        fb_data = (FB_DATA*)calloc(1, sizeof *fb_data);
        fb_data->data = data;
        fb_data->fb = nullptr;
    }


    if (fb_data->fb)
        return fb_data;

    uint32_t width, height, stride, handle;
    int ret;

    fb_data->fb = (drm_fb*)calloc(1, sizeof *fb_data->fb);
    fb_data->fb->bo = bo;

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);

    stride = gbm_bo_get_stride(bo);
    handle = gbm_bo_get_handle(bo).u32;

    ret = drmModeAddFB(data->deviceFd, width, height, 24, 32, stride, handle, &fb_data->fb->fb_id);
    if (ret)
    {
        printf("failed to create fb: %s\n", strerror(errno));
        free(fb_data->fb);
        free(fb_data);
        return nullptr;
    }

    gbm_bo_set_user_data(bo, fb_data, drm_fb_destroy_callback);

    return fb_data;
}

static void page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data)
{
    (void)fd;(void)frame;(void)sec;(void)usec;

    int *waiting_for_flip = (int*)data;
    *waiting_for_flip = 0;
}

static uint32_t find_crtc_for_encoder(const drmModeRes *resources, const drmModeEncoder *encoder)
{
    for (int i = 0; i < resources->count_crtcs; i++)
    {
        const uint32_t crtc_mask = 1 << i;
        const uint32_t crtc_id = resources->crtcs[i];

        bool encoderIsFree = true;


        for(LOutput *output : outputs)
        {
            DRM *data = (DRM*)output->imp()->data;
            if(crtc_id == data->crtc_id)
            {
                encoderIsFree = false;
                break;
            }
        }

        if (encoder->possible_crtcs & crtc_mask && encoderIsFree)
            return crtc_id;
    }
    return -1;
}

static uint32_t find_crtc_for_connector(int fd, const drmModeRes *resources, const drmModeConnector *connector)
{
    for (int i = 0; i < connector->count_encoders; i++)
    {
        const uint32_t encoder_id = connector->encoders[i];
        drmModeEncoder *encoder = drmModeGetEncoder(fd, encoder_id);
        if (encoder)
        {
            const uint32_t crtc_id = find_crtc_for_encoder(resources, encoder);

            drmModeFreeEncoder(encoder);
            if (crtc_id != 0)
                return crtc_id;
        }
    }
    return -1;
}

void destroyOutput(LOutput *output)
{
    // Destoy output
    DRM *data = (DRM*)output->imp()->data;
    drmModeFreeModeInfo(data->mode);
    gbm_bo_destroy(data->bo);
    gbm_bo_destroy(data->cursor_bo);
    drmModeFreeEncoder(data->encoder);
    free(data->fb);
    gbm_surface_destroy(data->gbm.surface);

    delete data;
    delete output;
}

void getPlanes()
{
    drmModePlaneResPtr planes = drmModeGetPlaneResources(lDevice.fd);
    for(UInt32 i = 0; i < planes->count_planes; i++)
    {
        drmModePlanePtr plane = drmModeGetPlane(lDevice.fd,planes->planes[i]);
        printf("Plane found: %i\n",plane->plane_id);
        drmModeFreePlane(plane);
    }
    drmModeFreePlaneResources(planes);
}

static const char *conn_name(uint32_t type)
{
    switch (type) {
    case DRM_MODE_CONNECTOR_Unknown:     return "unknown";
    case DRM_MODE_CONNECTOR_VGA:         return "VGA";
    case DRM_MODE_CONNECTOR_DVII:        return "DVI-I";
    case DRM_MODE_CONNECTOR_DVID:        return "DVI-D";
    case DRM_MODE_CONNECTOR_DVIA:        return "DVI-A";
    case DRM_MODE_CONNECTOR_Composite:   return "composite";
    case DRM_MODE_CONNECTOR_SVIDEO:      return "S-VIDEO";
    case DRM_MODE_CONNECTOR_LVDS:        return "LVDS";
    case DRM_MODE_CONNECTOR_Component:   return "component";
    case DRM_MODE_CONNECTOR_9PinDIN:     return "DIN";
    case DRM_MODE_CONNECTOR_DisplayPort: return "DisplayPort";
    case DRM_MODE_CONNECTOR_HDMIA:       return "HDMI-A";
    case DRM_MODE_CONNECTOR_HDMIB:       return "HDMI-B";
    case DRM_MODE_CONNECTOR_TV:          return "TV";
    case DRM_MODE_CONNECTOR_eDP:         return "eDP";
    case DRM_MODE_CONNECTOR_VIRTUAL:     return "virtual";
    case DRM_MODE_CONNECTOR_DSI:         return "DSI";
    case DRM_MODE_CONNECTOR_DPI:         return "DPI";
    case DRM_MODE_CONNECTOR_WRITEBACK:   return "writeback";
    default:                             return "unknown";
    }
}

void manageOutputs(bool notify)
{

    drmModeRes *resources;
    drmModeConnector *connector = NULL;
    drmModeEncoder *encoder = NULL;
    drmModeModeInfo *defaultMode = NULL;
    uint32_t crtc_id;
    int area;

    resources = drmModeGetResources(lDevice.fd);
    getPlanes();

    // Find connected connectors
    for (int i = 0; i < resources->count_connectors; i++)
    {
        connector = drmModeGetConnector(lDevice.fd, resources->connectors[i]);

        // Check if connected
        if (connector->connection == DRM_MODE_CONNECTED)
        {
            bool wasConnected = false;

            // Check if the output was already connected
            for(LOutput *output : outputs)
            {
                DRM *data = (DRM*)output->imp()->data;
                if(data->connector->connector_id == connector->connector_id)
                {
                    wasConnected = true;
                    break;
                }
            }

            if(wasConnected)
            {
                drmModeFreeConnector(connector);
                continue;
            }


            // Get the default mode
            area = 0;
            for (int j = 0; j < connector->count_modes; j++)
            {
                drmModeModeInfo *current_mode = &connector->modes[j];

                if (current_mode->type & DRM_MODE_TYPE_PREFERRED)
                {
                    defaultMode = current_mode;
                    break;
                }

                int current_area = current_mode->hdisplay * current_mode->vdisplay;

                if (current_area > area)
                {
                    defaultMode = current_mode;
                    area = current_area;
                }
            }

            if (!defaultMode)
            {
                printf("could not find mode!\n");
                drmModeFreeConnector(connector);
                continue;
            }


            // Find default encoder
            for (int j = 0; j < resources->count_encoders; j++)
            {
                encoder = drmModeGetEncoder(lDevice.fd, resources->encoders[j]);

                bool encoderIsFree = true;

                for(LOutput *output : outputs)
                {
                    DRM *data = (DRM*)output->imp()->data;
                    if(encoder->crtc_id == data->crtc_id)
                    {
                        encoderIsFree = false;
                        break;
                    }
                }

                if(encoderIsFree && encoder->encoder_id == connector->encoder_id)
                {
                    break;
                }
                drmModeFreeEncoder(encoder);
                encoder = NULL;
            }

            printf("CHECKPOINT!!\n");


            if(encoder)
            {
                crtc_id = encoder->crtc_id;
            }
            else
            {
                uint32_t _crtc_id = find_crtc_for_connector(lDevice.fd,resources, connector);

                if (_crtc_id == 0)
                {
                    printf("No crtc found!\n");
                    drmModeFreeConnector(connector);
                    continue;
                }

                crtc_id = _crtc_id;
            }

            //connector_id = connector->connector_id;

            // SUCCESS! NEW AVALAIBLE OUTPUT!
            LOutput *newOutput = lDevice.lCompositor->createOutputRequest();

            newOutput->imp()->data = malloc(sizeof(DRM));
            DRM *data = (DRM*)newOutput->imp()->data;

            data->deviceFd = lDevice.fd;
            data->connector = connector;
            data->crtc_id = crtc_id;
            data->encoder = encoder;
            data->mode = defaultMode;

            newOutput->imp()->m_name = (char*)conn_name(connector->connector_type);
            printf("NAME:%s\n",newOutput->name());
            newOutput->imp()->m_rect.setW(defaultMode->hdisplay);
            newOutput->imp()->m_rect.setH(defaultMode->vdisplay);
            newOutput->imp()->m_rectScaled = newOutput->imp()->m_rect/newOutput->getOutputScale();

            newOutput->refreshRate = defaultMode->vrefresh;
            outputs.push_front(newOutput);
            printf("New output with id: %i and vrefresh: %i, w: %i h: %i \n",crtc_id,defaultMode->vrefresh,defaultMode->hdisplay,defaultMode->vdisplay);

            if(notify)
                lDevice.lCompositor->outputManager()->connectedOutputRequest(newOutput);

        }
        else
        {
            // Check if the output was connected
            for(list<LOutput*>::iterator it = outputs.begin(); it != outputs.end(); it++)
            {
                LOutput *output = *it;
                DRM *data = (DRM*)output->imp()->data;
                if(data->connector->connector_id == connector->connector_id)
                {
                    printf("OUTPUT DISCONNECTED.\n");
                    outputs.erase(it);
                    lDevice.lCompositor->outputManager()->disonnectedOutputRequest(output);
                    destroyOutput(output);
                    break;
                }
            }

            drmModeFreeConnector(connector);
            connector = NULL;
        }
    }

    drmModeFreeResources(resources);

}

int hotplugEvent(int, unsigned int, void*)
{
    udev_device* monitorDev = udev_monitor_receive_device(lDevice.monitor);

    if(monitorDev)
    {
        if(udev_device_get_devnum(lDevice.dev) == udev_device_get_devnum(monitorDev))
        {
            printf("HOTPLUG EVENT.\n");
            manageOutputs(true);
        }
    }

    udev_device_unref(monitorDev);

    return 0;
}

void LBackend::initialize(LCompositor *compositor)
{
    lDevice.lCompositor = compositor;
    lDevice.udev = udev_new();

    if (!lDevice.udev)
    {
        printf("Can't create udev.\n");
        exit(1);
    }

    udev_enumerate *enumerate;
    udev_list_entry *devices;

    enumerate = udev_enumerate_new(lDevice.udev);
    udev_enumerate_add_match_subsystem(enumerate, "drm");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "drm_minor");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry *entry;

    // Find the first useful device (GPU)
    udev_list_entry_foreach(entry, devices)
    {
        // Path where device is mounted
        const char *path = udev_list_entry_get_name(entry);

        // Get the device
        lDevice.dev = udev_device_new_from_syspath(lDevice.udev, path);

        // Get the device name (Ej:/dev/dri/card0)
        const char *devName = udev_device_get_property_value(lDevice.dev, "DEVNAME");

        drmModeRes *resources;

        lDevice.fd = open(devName, O_RDWR);

        if (lDevice.fd < 0)
        {
            printf("Could not open drm device\n");
            udev_device_unref(lDevice.dev);
            lDevice.dev = nullptr;
            continue;
        }

        resources = drmModeGetResources(lDevice.fd);

        if (!resources)
        {
            printf("drmModeGetResources failed: %s\n", strerror(errno));
            drmModeFreeResources(resources);
            udev_device_unref(lDevice.dev);
            lDevice.dev = nullptr;
            close(lDevice.fd);
            continue;
        }

        printf("Using GPU: %s\n",devName);
        drmModeFreeResources(resources);
        break;
    }

    udev_enumerate_unref(enumerate);

    if(!lDevice.dev)
    {
        printf("No GPU card found.\n");
        exit(1);
    }


    /******* SUCCESS CARD FOUND ! *******/

    // Add a monitor to detect connectors hotplug
    lDevice.monitor = udev_monitor_new_from_netlink(lDevice.udev,"udev");
    udev_monitor_filter_add_match_subsystem_devtype(lDevice.monitor, "drm", "drm_minor");

    udev_list_entry *tags = udev_device_get_tags_list_entry(lDevice.dev);

    // Add tag filters to the monitor
    udev_list_entry_foreach(entry, tags)
    {
        const char *tag = udev_list_entry_get_name(entry);
        udev_monitor_filter_add_match_tag(lDevice.monitor,tag);
    }

    udev_monitor_enable_receiving(lDevice.monitor);

    // Add the FD to the wayland event loop
    LWayland::addFdListener(udev_monitor_get_fd(lDevice.monitor), nullptr, &hotplugEvent);

    // Check current avaliable outputs
    manageOutputs(false);
}

std::list<LOutput *> *LBackend::getAvaliableOutputs(LCompositor *compositor)
{
    return &outputs;
}

void LBackend::createGLContext(LOutput *output)
{

    DRM *data = (DRM*)output->imp()->data;

    data->evctx =
    {
        .version = DRM_EVENT_CONTEXT_VERSION,
        .page_flip_handler = page_flip_handler,
    };

    FD_ZERO(&data->fds);
    FD_SET(0, &data->fds);
    FD_SET(data->deviceFd, &data->fds);

    data->ret = init_gbm(data);
    if (data->ret)
    {
        printf("Failed to initialize GBM\n");
        return;
    }

    data->ret = init_gl(data,output);
    if (data->ret)
    {
        printf("Failed to initialize EGL\n");
        return;
    }

    // Clear the color buffer
    eglSwapBuffers(data->gl.display, data->gl.surface);
    data->bo = gbm_surface_lock_front_buffer(data->gbm.surface);
    data->fb = drm_fb_get_from_bo(data->bo,data)->fb;

    // set mode:
    data->ret = drmModeSetCrtc(data->deviceFd, data->crtc_id, data->fb->fb_id, 0, 0, &data->connector->connector_id, 1, data->mode);
    if (data->ret)
    {
        printf("Failed to set mode: %s\n", strerror(errno));
        return;
    }

    initializeCursor(output);
    output->imp()->m_initializeResult = Louvre::LOutput::InitializeResult::Initialized;
    printf("DRM backend initialized.\n");

    return;
}

long t = 0;
void LBackend::flipPage(LOutput *output)
{
    DRM *data = (DRM*)output->imp()->data;
    gbm_bo *next_bo;
    int waiting_for_flip = 1;

    eglSwapBuffers(data->gl.display, data->gl.surface);
    next_bo = gbm_surface_lock_front_buffer(data->gbm.surface);
    data->fb = drm_fb_get_from_bo(next_bo,data)->fb;

    drmVBlank vbl;
    vbl.request.sequence = 0;
    vbl.request.signal = 0,
    vbl.request.type =  DRM_VBLANK_RELATIVE;


    if(true || lDevice.lCompositor->outputs().size() == 1)
    {
        //t = LTime::us().tv_nsec;
        data->ret = drmWaitVBlank(lDevice.fd,&vbl);
        //printf("LAPSE: %lu\n",(LTime::us().tv_nsec - t));
        if (data->ret != 0)
        {
            printf("FAILED WAIT VBLANK\n");
        }

        drmModeSetCrtc(data->deviceFd, data->crtc_id, data->fb->fb_id, 0, 0, &data->connector->connector_id, 1, data->mode);

        /*
        data->ret = drmModePageFlip(data->deviceFd, data->crtc_id, data->fb->fb_id, DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);

        if(data->ret)
        {
            drmModeSetCrtc(data->deviceFd, data->crtc_id, data->fb->fb_id, 0, 0, &data->connector->connector_id, 1, data->mode);
            //printf("Failed to queue page flip: %s\n", strerror(errno));
            //return;
        }
        */

        /*
        int w = 0;
        while (waiting_for_flip)
        {
            printf("WAITING %i\n",w);
            drmHandleEvent(data->deviceFd, &data->evctx);
            w++;
        }
        */

            //data->ret = select(data->deviceFd + 1, &data->fds, NULL, NULL, NULL);


    }
    else
    {
        drmModeSetCrtc(data->deviceFd, data->crtc_id, data->fb->fb_id, 0, 0, &data->connector->connector_id, 1, data->mode);
    }

    // release last buffer to render on again:
    gbm_surface_release_buffer(data->gbm.surface, data->bo);
    data->bo = next_bo;
}

EGLDisplay LBackend::getEGLDisplay(LOutput *output)
{
    DRM *data = (DRM*)output->imp()->data;
    return data->gl.display;
}

bool LBackend::hasHardwareCursorSupport()
{
    return true;
}

void LBackend::initializeCursor(LOutput *output)
{
    DRM *data = (DRM*)output->imp()->data;

    if(data->cursorInitialized)
        return;

    data->cursorInitialized = true;

    // Create cursor bo
    data->cursor_bo = gbm_bo_create(data->gbm.dev, 64, 64, GBM_FORMAT_ARGB8888, GBM_BO_USE_CURSOR | GBM_BO_USE_RENDERING); //GBM_BO_USE_WRITE |

    // Cursor
    PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress ("eglCreateImageKHR");
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) eglGetProcAddress ("glEGLImageTargetTexture2DOES");
    EGLImage image = eglCreateImageKHR(data->gl.display,data->gl.context,EGL_NATIVE_PIXMAP_KHR,data->cursor_bo,NULL);

    glGenFramebuffers(1, &data->cursoFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, data->cursoFramebuffer);

    glActiveTexture(GL_TEXTURE0+1);
    glGenTextures(1, &data->cursorTexture);
    glBindTexture(GL_TEXTURE_2D, data->cursorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D,image);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data->cursorTexture, 0);
    data->cursorBoHandleU32 = gbm_bo_get_handle(data->cursor_bo).u32;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void LBackend::setCursor(LOutput *output, LTexture *texture, const LSizeF &size)
{
    DRM *data = (DRM*)output->imp()->data;

    if(!texture)
    {
        drmModeSetCursor(data->deviceFd, data->crtc_id, 0, 0, 0);
        data->cursorVisible = false;
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, data->cursoFramebuffer);
    output->painter()->scaleCursor(texture,LRect(0,0,texture->size().w(),-texture->size().h()),LRect(0,0,size.w(),size.h()));

    if(!data->cursorVisible)
        drmModeSetCursor(data->deviceFd, data->crtc_id, data->cursorBoHandleU32, 64, 64);

    // Goes back to main framebuffer
    //if(std::this_thread::get_id() != output->compositor()->mainThreadId())
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glFlush();

}

void LBackend::setCursorPosition(LOutput *output, const LPoint &position)
{
    DRM *data = (DRM*)output->imp()->data;
    drmModeMoveCursor(data->deviceFd, data->crtc_id, position.x(), position.y());
}


LGraphicBackend LBackendAPI;

extern "C" LGraphicBackend *getAPI()
{
   printf("Louvre DRM backend loaded.\n");
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
































