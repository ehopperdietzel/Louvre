#ifndef LDATADEVICEPRIVATE_H
#define LDATADEVICEPRIVATE_H

#include <LDataDevice.h>

class Louvre::LDataDevice::LDataDevicePrivate
{
public:
    LDataDevicePrivate() = default;
    ~LDataDevicePrivate() = default;

    LClient *m_client = nullptr;
    wl_resource *m_resource = nullptr;
    LDataOffer *m_dataOffered = nullptr;
    UInt32 m_lastDataOfferId = 0;
};

#endif // LDATADEVICEPRIVATE_H
