#include "LDataSource.h"

using namespace Louvre;

LDataSource::LDataSource(wl_resource *resource, LClient *client)
{
    m_resource = resource;
    m_client = client;
}

wl_resource *LDataSource::resource() const
{
    return m_resource;
}

LClient *LDataSource::client() const
{
    return m_client;
}

const std::list<const char *> &LDataSource::mimeTypes() const
{
    return m_mimeTypes;
}

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3

    UInt32 LDataSource::dndActions() const
    {
        return m_dndActions;
    }

#endif
