#include <LDataDevicePrivate.h>

using namespace Louvre;

LDataDevice::LDataDevice(wl_resource *resource, LClient *client)
{
    m_imp = new LDataDevicePrivate();
    m_imp->m_resource = resource;
    m_imp->m_client = client;
}

LDataDevice::~LDataDevice()
{
    delete m_imp;
}

wl_resource *LDataDevice::resource() const
{
    return m_imp->m_resource;
}

LClient *LDataDevice::client() const
{
    return m_imp->m_client;
}

LDataOffer *LDataDevice::dataOffered() const
{
    return m_imp->m_dataOffered;
}

UInt32 LDataDevice::lastDataOfferId() const
{
    return m_imp->m_lastDataOfferId;
}

LDataDevice::LDataDevicePrivate *LDataDevice::imp() const
{
    return m_imp;
}

