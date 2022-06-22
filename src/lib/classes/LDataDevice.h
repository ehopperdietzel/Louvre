#ifndef LDATADEVICE_H
#define LDATADEVICE_H

#include <LNamespaces.h>

class Louvre::LDataDevice
{
public:
    LDataDevice(wl_resource *resource, LClient *client);
    wl_resource *resource() const;
    LClient *client() const;
    void sendDataOfferEvent();

private:
    LClient *m_client = nullptr;
    wl_resource *m_resource = nullptr;
    LDataOffer *m_dataOffer = nullptr;
};

#endif // LDATADEVICE_H
