#ifndef LDATASOURCEPRIVATE_H
#define LDATASOURCEPRIVATE_H

#include <LDataSource.h>

class Louvre::LDataSource::LDataSourcePrivate
{
public:
    LDataSourcePrivate() = default;
    ~LDataSourcePrivate() = default;

    LClient *m_client = nullptr;
    wl_resource *m_resource = nullptr;
    std::list<const char*>m_mimeTypes;
    std::list<LDataOffer*>m_dataOffers;

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3
    UInt32 m_dndActions = 0;
#endif

};

#endif // LDATASOURCEPRIVATE_H
