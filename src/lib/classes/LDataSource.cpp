#include "LDataSource.h"

using namespace Louvre;

LDataSource::LDataSource(wl_resource *resource, LClient *client)
{
    p_resource = resource;
    p_client = client;
}

wl_resource *LDataSource::resource() const
{
    return p_resource;
}

LClient *LDataSource::client() const
{
    return p_client;
}

const std::list<const char *> &LDataSource::mimeTypes() const
{
    return p_mimeTypes;
}

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3

    UInt32 LDataSource::dndActions() const
    {
        return p_dndActions;
    }

#endif
