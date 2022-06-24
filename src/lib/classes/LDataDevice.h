#ifndef LDATADEVICE_H
#define LDATADEVICE_H

#include <LNamespaces.h>

class Louvre::LDataDevice
{
public:
    LDataDevice(wl_resource *resource, LClient *client);
    ~LDataDevice();
    wl_resource *resource() const;
    LClient *client() const;
    LDataOffer *dataOffered() const;
    UInt32 lastDataOfferId() const;
    class LDataDevicePrivate;
    LDataDevicePrivate *imp() const;

private:
    LDataDevicePrivate *m_imp = nullptr;
};

#endif // LDATADEVICE_H
