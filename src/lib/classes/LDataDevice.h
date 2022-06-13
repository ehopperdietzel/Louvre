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
    LClient *p_client = nullptr;
    wl_resource *p_resource = nullptr;
    LDataOffer *p_dataOffer = nullptr;
};

#endif // LDATADEVICE_H
