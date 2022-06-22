#include "LDataDevice.h"

using namespace Louvre;

LDataDevice::LDataDevice(wl_resource *resource, LClient *client)
{
    m_resource = resource;
    m_client = client;
}

wl_resource *LDataDevice::resource() const
{
    return m_resource;
}

LClient *LDataDevice::client() const
{
    return m_client;
}

void LDataDevice::sendDataOfferEvent()
{
    if(m_dataOffer)
        return;
}
