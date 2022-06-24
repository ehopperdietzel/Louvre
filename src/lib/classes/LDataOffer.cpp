#include <LDataOfferPrivate.h>

using namespace Louvre;

LDataOffer::LDataOffer(wl_resource *resource, LDataDevice *dataDevice)
{
    m_imp = new LDataOfferPrivate();
    m_imp->m_resource = resource;
    m_imp->m_dataDevice = dataDevice;
}

LDataOffer::~LDataOffer()
{
    delete m_imp;
}

wl_resource *LDataOffer::resource() const
{
    return m_imp->m_resource;
}

LDataSource *LDataOffer::dataSource() const
{
    return m_imp->m_dataSource;
}

LDataDevice *LDataOffer::dataDevice() const
{
    return m_imp->m_dataDevice;
}

LDataOffer::LDataOfferPrivate *LDataOffer::imp() const
{
    return m_imp;
}
