#include "LDataDevice.h"

using namespace Louvre;

LDataDevice::LDataDevice(wl_resource *resource, LClient *client)
{
    p_resource = resource;
    p_client = client;
}

wl_resource *LDataDevice::resource() const
{
    return p_resource;
}

LClient *LDataDevice::client() const
{
    return p_client;
}

void LDataDevice::sendDataOfferEvent()
{
    if(p_dataOffer)
        return;
}
