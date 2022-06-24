#ifndef LDATASOURCE_H
#define LDATASOURCE_H

#include <LNamespaces.h>

class Louvre::LDataSource
{
public:   

    LDataSource(wl_resource *resource, LClient *client);
    ~LDataSource();
    wl_resource *resource() const;
    LClient *client() const;
    const std::list<const char*>&mimeTypes() const;
    UChar8 action() const;
    const std::list<LDataOffer*>&dataOffers() const;

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3

    enum DND_ACTIONS : UInt32
    {
        NONE = 0,
        COPY = 1,
        MOVE = 2,
        ASK = 4
    };

    UInt32 dndActions() const;
#endif
    class LDataSourcePrivate;
    LDataSourcePrivate *imp();
private:
    LDataSourcePrivate *m_imp = nullptr;
};

#endif // LDATASOURCE_H
