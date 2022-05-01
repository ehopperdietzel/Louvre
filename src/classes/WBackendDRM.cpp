#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <libudev.h>
#include <locale.h>
#include <unistd.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm/drm.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <WBackend.h>
#include <WCompositor.h>
#include <WOutput.h>
#include <WWayland.h>

#if W_BACKEND == 1

using namespace Wpp;


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
};

struct FB_DATA
{
    drm_fb *fb;
    DRM *data;
};

static int init_gbm(DRM *data)
{
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

static int init_gl(DRM *data)
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

    data->gl.context = eglCreateContext(data->gl.display, data->gl.config, EGL_NO_CONTEXT, context_attribs);

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

    WWayland::setContext(data->gl.display,data->gl.context);

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

static struct FB_DATA * drm_fb_get_from_bo(struct gbm_bo *bo, DRM *data)
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



EGLDisplay WBackend::getEGLDisplay(WOutput *output)
{
    DRM *data = (DRM*)output->data;
    return data->gl.display;
}

void WBackend::createGLContext(WOutput *output)
{
    DRM *data = (DRM*)output->data;

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

    data->ret = init_gl(data);
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

    printf("DRM backend initialized.\n");

    return;
}

void WBackend::flipPage(WOutput *output)
{
    DRM *data = (DRM*)output->data;

    gbm_bo *next_bo;
    int waiting_for_flip = 1;

    eglSwapBuffers(data->gl.display, data->gl.surface);
    next_bo = gbm_surface_lock_front_buffer(data->gbm.surface);
    data->fb = drm_fb_get_from_bo(next_bo,data)->fb;

    // Here you could also update drm plane layers if you want
    // hw composition


    data->ret = drmModePageFlip(data->deviceFd, data->crtc_id, data->fb->fb_id,DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
    if (data->ret)
    {
        printf("Failed to queue page flip: %s\n", strerror(errno));
        return;
    }

    while (waiting_for_flip)
    {
        data->ret = select(data->deviceFd + 1, &data->fds, NULL, NULL, NULL);
        if (data->ret < 0)
        {
            printf("Select err: %s\n", strerror(errno));
            return;
        }
        else if (data->ret == 0)
        {
            printf("Select timeout!\n");
            return;
        }
        else if (FD_ISSET(0, &data->fds))
        {
            printf("User interrupted!\n");
            break;
        }
        drmHandleEvent(data->deviceFd, &data->evctx);
    }

    // release last buffer to render on again:
    gbm_surface_release_buffer(data->gbm.surface, data->bo);
    data->bo = next_bo;
}

static uint32_t find_crtc_for_encoder(const drmModeRes *resources, const drmModeEncoder *encoder)
{
    for (int i = 0; i < resources->count_crtcs; i++)
    {
        const uint32_t crtc_mask = 1 << i;
        const uint32_t crtc_id = resources->crtcs[i];
        if (encoder->possible_crtcs & crtc_mask)
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

std::list<WOutput *> WBackend::getAvaliableOutputs()
{

    // Store the found outputs
    std::list<WOutput*>outputs;

    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;

    udev = udev_new();
    if (!udev)
    {
        printf("Can't create udev.\n");
        exit(1);
    }

    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "drm");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "drm_minor");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry *entry;

    // Loop every DRM device and create a WOutput for each avaliable connector
    udev_list_entry_foreach(entry, devices)
    {
        // Path where device is mounted
        const char *path = udev_list_entry_get_name(entry);

        // Get the device
        udev_device *dev = udev_device_new_from_syspath(udev, path);

        // We need the DEVNAME property (Ej:/dev/dri/card0)
        const char *devName = udev_device_get_property_value(dev, "DEVNAME");
        printf("DEVNAME: %s\n",devName);

        drmModeRes *resources;
        drmModeConnector *connector = NULL; // Y
        drmModeEncoder *encoder = NULL; // N
        drmModeModeInfo *defaultMode = NULL; // N
        uint32_t crtc_id; // Y
        //uint32_t connector_id; // Y
        int area, drmDeviceFd;

        drmDeviceFd = open(devName, O_RDWR);

        if (drmDeviceFd < 0)
        {
            printf("Could not open drm device\n");
            continue;
        }

        resources = drmModeGetResources(drmDeviceFd);

        if (!resources)
        {
            printf("drmModeGetResources failed: %s\n", strerror(errno));
            continue;
        }

        // Find connected connectors
        for (int i = 0; i < resources->count_connectors; i++)
        {
            connector = drmModeGetConnector(drmDeviceFd, resources->connectors[i]);

            // Check if connected
            if (connector->connection == DRM_MODE_CONNECTED)
            {
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
                    encoder = drmModeGetEncoder(drmDeviceFd, resources->encoders[j]);
                    if (encoder->encoder_id == connector->encoder_id)
                        break;
                    drmModeFreeEncoder(encoder);
                    encoder = NULL;
                }

                if (encoder)
                    crtc_id = encoder->crtc_id;
                else
                {
                    uint32_t _crtc_id = find_crtc_for_connector(drmDeviceFd,resources, connector);

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
                WOutput *newOutput = new WOutput();

                newOutput->data = malloc(sizeof(DRM));
                DRM *data = (DRM*)newOutput->data;

                data->deviceFd = drmDeviceFd;
                data->deviceName = devName;
                data->connector = connector;
                data->crtc_id = crtc_id;
                data->encoder = encoder;
                data->mode = defaultMode;

                outputs.push_front(newOutput);
                printf("New output with id: %i.\n",crtc_id);
            }
            else
            {
                drmModeFreeConnector(connector);
                connector = NULL;
            }
        }

        udev_device_unref(dev);
        //close(drmDeviceFd);
    }
    udev_enumerate_unref(enumerate);
    return outputs;
}

/*
void Wpp::WBackend::setHWCursor()
{

    gbm_bo *m_bo = nullptr;
    m_bo = gbm_bo_create(gbm.dev, 64, 64,GBM_FORMAT_ARGB8888, GBM_BO_USE_CURSOR_64X64 | GBM_BO_USE_WRITE);

    unsigned char cursorPixels[4*64*64];
    for(int i = 0; i < 4*64*64; i++)
        cursorPixels[i] = 255;
    gbm_bo_write(m_bo,cursorPixels, 4*64*64);
    uint32_t handle = gbm_bo_get_handle(m_bo).u32;
    drmModeSetCursor(drm.fd, drm.crtc_id, handle,64, 64);

}

void WBackend::setHWCursorPos(Int32 x, Int32 y)
{
    (void)x;(void)y;
    //drmModeMoveCursor(drm.fd, drm.crtc_id,x,y);
}
*/

#endif








































