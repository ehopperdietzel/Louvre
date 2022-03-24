#include <WOutputManager.h>

#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm/drm.h>

#include <WOutput.h>

using namespace WaylandPlus;

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
WOutputManager::WOutputManager(WCompositor *compositor)
{
    _compositor = compositor;
    struct udev *udev;
    struct udev_enumerate *enumerate;
    struct udev_list_entry *devices;
    struct udev_device *dev;

    int fd_udev = -1;
    struct epoll_event ep_udev = { 0 };

    struct udev_monitor *mon;
    int fd_ep = -1;

    udev = udev_new();
    if (!udev)
    {
        printf("Can't create udev\n");
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
        uint32_t connector_id; // Y
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
                for (int j = 0, area = 0; j < connector->count_modes; j++)
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

                connector_id = connector->connector_id;

                // SUCCESS! NEW AVALAIBLE OUTPUT!
                WOutput *newOutput = new WOutput(devName,connector,crtc_id);
                newOutput->_currentMode = defaultMode;
                newOutput->_drmFd = drmDeviceFd;
                _outputs.push_front(newOutput);
                printf("New output with id: %i\n",crtc_id);
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
}

WCompositor *WOutputManager::getCompositor()
{
    return _compositor;
}

const list<WOutput *> *WOutputManager::getOutputsList()
{
    return &_outputs;
}

