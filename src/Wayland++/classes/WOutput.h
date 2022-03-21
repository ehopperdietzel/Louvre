#ifndef WOUTPUT_H
#define WOUTPUT_H

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>
#include <drm/drm.h>
#include <WNamespaces.h>


class WaylandPlus::WOutput
{
public:
    WOutput(const char *drmDeviceName,drmModeConnector *connector,UInt32 crtcId);
    ~WOutput();

private:
    char *_devName = nullptr;
    drmModeConnector *_connector = nullptr;
    UInt32 _crtc_id = 0;
};

#endif // WOUTPUT_H
