#include <LDataSourcePrivate.h>

using namespace Louvre;

LDataSource::LDataSource(wl_resource *resource, LClient *client)
{
    m_imp = new LDataSourcePrivate();
    m_imp->m_resource = resource;
    m_imp->m_client = client;
}

LDataSource::~LDataSource()
{
    delete m_imp;
}

wl_resource *LDataSource::resource() const
{
    return m_imp->m_resource;
}

LClient *LDataSource::client() const
{
    return m_imp->m_client;
}

const std::list<const char *> &LDataSource::mimeTypes() const
{
    return m_imp->m_mimeTypes;
}

const std::list<LDataOffer *> &LDataSource::dataOffers() const
{
    return m_imp->m_dataOffers;
}


#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3

    UInt32 LDataSource::dndActions() const
    {
        return m_imp->m_dndActions;
    }

#endif

LDataSource::LDataSourcePrivate *LDataSource::imp()
{
    return m_imp;
}
