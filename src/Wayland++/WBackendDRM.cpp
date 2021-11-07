#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm/drm.h>

#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>


#include <assert.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#include <WBackendDRM.h>
#include <WCompositor.h>

int drmWidth = 0;
int drmHeight = 0;

static struct {
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    GLuint program;
    GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
    GLuint vbo;
    GLuint positionsoffset, colorsoffset, normalsoffset;
} gl;

static struct
{
    struct gbm_device *dev;
    struct gbm_surface *surface;
} gbm;

static struct {
    int fd;
    drmModeModeInfo *mode;
    uint32_t crtc_id;
    uint32_t connector_id;
} drm;

struct drm_fb {
    struct gbm_bo *bo;
    uint32_t fb_id;
};

static uint32_t find_crtc_for_encoder(const drmModeRes *resources, const drmModeEncoder *encoder)
{
    int i;

    for (i = 0; i < resources->count_crtcs; i++)
    {
        /* possible_crtcs is a bitmask as described here:
         * https://dvdhrm.wordpress.com/2012/09/13/linux-drm-mode-setting-api
         */
        const uint32_t crtc_mask = 1 << i;
        const uint32_t crtc_id = resources->crtcs[i];
        if (encoder->possible_crtcs & crtc_mask)
        {
            return crtc_id;
        }
    }

    /* no match found */
    return -1;
}

static uint32_t find_crtc_for_connector(const drmModeRes *resources,
                    const drmModeConnector *connector) {
    int i;

    for (i = 0; i < connector->count_encoders; i++) {
        const uint32_t encoder_id = connector->encoders[i];
        drmModeEncoder *encoder = drmModeGetEncoder(drm.fd, encoder_id);

        if (encoder) {
            const uint32_t crtc_id = find_crtc_for_encoder(resources, encoder);

            drmModeFreeEncoder(encoder);
            if (crtc_id != 0) {
                return crtc_id;
            }
        }
    }

    /* no match found */
    return -1;
}

static int init_drm(void)
{
    drmModeRes *resources;
    drmModeConnector *connector = NULL;
    drmModeEncoder *encoder = NULL;
    int i, area;

    drm.fd = open("/dev/dri/card0", O_RDWR);

    if (drm.fd < 0) {
        printf("could not open drm device\n");
        return -1;
    }

    resources = drmModeGetResources(drm.fd);
    if (!resources) {
        printf("drmModeGetResources failed: %s\n", strerror(errno));
        return -1;
    }

    /* find a connected connector: */
    for (i = 0; i < resources->count_connectors; i++) {
        connector = drmModeGetConnector(drm.fd, resources->connectors[i]);
        if (connector->connection == DRM_MODE_CONNECTED) {
            /* it's connected, let's use this! */
            break;
        }
        drmModeFreeConnector(connector);
        connector = NULL;
    }

    if (!connector) {
        /* we could be fancy and listen for hotplug events and wait for
         * a connector..
         */
        printf("no connected connector!\n");
        return -1;
    }

    /* find prefered mode or the highest resolution mode: */
    for (i = 0, area = 0; i < connector->count_modes; i++) {
        drmModeModeInfo *current_mode = &connector->modes[i];

        if (current_mode->type & DRM_MODE_TYPE_PREFERRED) {
            drm.mode = current_mode;
        }

        int current_area = current_mode->hdisplay * current_mode->vdisplay;
        if (current_area > area) {
            drm.mode = current_mode;
            area = current_area;
        }
    }

    if (!drm.mode) {
        printf("could not find mode!\n");
        return -1;
    }

    /* find encoder: */
    for (i = 0; i < resources->count_encoders; i++) {
        encoder = drmModeGetEncoder(drm.fd, resources->encoders[i]);
        if (encoder->encoder_id == connector->encoder_id)
            break;
        drmModeFreeEncoder(encoder);
        encoder = NULL;
    }

    if (encoder) {
        drm.crtc_id = encoder->crtc_id;
    } else {
        uint32_t crtc_id = find_crtc_for_connector(resources, connector);
        if (crtc_id == 0) {
            printf("no crtc found!\n");
            return -1;
        }

        drm.crtc_id = crtc_id;
    }

    drm.connector_id = connector->connector_id;

    return 0;
}

static int init_gbm(void)
{
    gbm.dev = gbm_create_device(drm.fd);

    gbm.surface = gbm_surface_create(gbm.dev,
            drm.mode->hdisplay, drm.mode->vdisplay,
            GBM_FORMAT_XRGB8888,
            GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    if (!gbm.surface) {
        printf("failed to create gbm surface\n");
        return -1;
    }

    return 0;
}

static int init_gl(void)
{
    EGLint major, minor, n;
    GLuint vertex_shader, fragment_shader;
    GLint ret;

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

    gl.display = get_platform_display(EGL_PLATFORM_GBM_KHR, gbm.dev, NULL);

    if (!eglInitialize(gl.display, &major, &minor)) {
        printf("failed to initialize\n");
        return -1;
    }

    printf("Using display %p with EGL version %d.%d\n",
            gl.display, major, minor);

    printf("EGL Version \"%s\"\n", eglQueryString(gl.display, EGL_VERSION));
    printf("EGL Vendor \"%s\"\n", eglQueryString(gl.display, EGL_VENDOR));
    printf("EGL Extensions \"%s\"\n", eglQueryString(gl.display, EGL_EXTENSIONS));

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        printf("failed to bind api EGL_OPENGL_ES_API\n");
        return -1;
    }

    if (!eglChooseConfig(gl.display, config_attribs, &gl.config, 1, &n) || n != 1) {
        printf("failed to choose config: %d\n", n);
        return -1;
    }

    gl.context = eglCreateContext(gl.display, gl.config,
            EGL_NO_CONTEXT, context_attribs);
    if (gl.context == NULL) {
        printf("failed to create context\n");
        return -1;
    }

    gl.surface = eglCreateWindowSurface(gl.display, gl.config, gbm.surface, NULL);
    if (gl.surface == EGL_NO_SURFACE) {
        printf("failed to create egl surface\n");
        return -1;
    }

    /* connect the context to the surface */
    eglMakeCurrent(gl.display, gl.surface, gl.surface, gl.context);

    printf("GL Extensions: \"%s\"\n", glGetString(GL_EXTENSIONS));

    return 0;
}

static void drm_fb_destroy_callback(struct gbm_bo *bo, void *data)
{
    struct drm_fb *fb = (struct drm_fb*)data;
    struct gbm_device *gbm = gbm_bo_get_device(bo);

    if (fb->fb_id)
        drmModeRmFB(drm.fd, fb->fb_id);

    free(fb);
}

static struct drm_fb * drm_fb_get_from_bo(struct gbm_bo *bo)
{
    struct drm_fb *fb = (struct drm_fb*)gbm_bo_get_user_data(bo);
    uint32_t width, height, stride, handle;
    int ret;

    if (fb)
        return fb;

    fb = (struct drm_fb*)calloc(1, sizeof *fb);
    fb->bo = bo;

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);
    drmWidth = width;
    drmHeight = height;

    stride = gbm_bo_get_stride(bo);
    handle = gbm_bo_get_handle(bo).u32;

    ret = drmModeAddFB(drm.fd, width, height, 24, 32, stride, handle, &fb->fb_id);
    if (ret)
    {
        printf("failed to create fb: %s\n", strerror(errno));
        free(fb);
        return nullptr;
    }

    gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

    return fb;
}

static void page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data)
{
    int *waiting_for_flip = (int*)data;
    *waiting_for_flip = 0;
}


drmEventContext evctx ={};
struct gbm_bo *bo;
struct drm_fb *fb;
int ret;
fd_set fds;

void paintDRM()
{
    struct gbm_bo *next_bo;
    int waiting_for_flip = 1;

    eglSwapBuffers(gl.display, gl.surface);
    next_bo = gbm_surface_lock_front_buffer(gbm.surface);
    fb = drm_fb_get_from_bo(next_bo);

    /*
     * Here you could also update drm plane layers if you want
     * hw composition
     */

    ret = drmModePageFlip(drm.fd, drm.crtc_id, fb->fb_id,DRM_MODE_PAGE_FLIP_EVENT, &waiting_for_flip);
    if (ret)
    {
        printf("Failed to queue page flip: %s\n", strerror(errno));
        return;
    }

    while (waiting_for_flip)
    {
        ret = select(drm.fd + 1, &fds, NULL, NULL, NULL);
        if (ret < 0)
        {
            printf("Select err: %s\n", strerror(errno));
            return;
        }
        else if (ret == 0)
        {
            printf("Select timeout!\n");
            return;
        }
        else if (FD_ISSET(0, &fds))
        {
            printf("User interrupted!\n");
            break;
        }
        drmHandleEvent(drm.fd, &evctx);
    }

    /* release last buffer to render on again: */
    gbm_surface_release_buffer(gbm.surface, bo);
    bo = next_bo;


}
void initBackend(WCompositor *compositor)
{
    evctx =
    {
        .version = DRM_EVENT_CONTEXT_VERSION,
        .page_flip_handler = page_flip_handler,
    };

    ret = init_drm();
    if (ret)
    {
        printf("failed to initialize DRM\n");
        return;
    }

    FD_ZERO(&fds);
    FD_SET(0, &fds);
    FD_SET(drm.fd, &fds);

    ret = init_gbm();
    if (ret)
    {
        printf("failed to initialize GBM\n");
        return;
    }

    ret = init_gl();
    if (ret)
    {
        printf("failed to initialize EGL\n");
        return;
    }

    compositor->initializeGL();

    /* clear the color buffer */
    eglSwapBuffers(gl.display, gl.surface);
    bo = gbm_surface_lock_front_buffer(gbm.surface);
    fb = drm_fb_get_from_bo(bo);

    /* set mode: */
    ret = drmModeSetCrtc(drm.fd, drm.crtc_id, fb->fb_id, 0, 0, &drm.connector_id, 1, drm.mode);
    if (ret)
    {
        printf("Failed to set mode: %s\n", strerror(errno));
        return;
    }


    return;
}

int backendWidth()
{
    return drmWidth;
}
int backendHeight()
{
    return drmHeight;
}

EGLDisplay getEGLDisplay()
{
    return gl.display;
}
