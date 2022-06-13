#ifndef LDATASOURCE_H
#define LDATASOURCE_H

#include <LNamespaces.h>

class Louvre::LDataSource
{
public:   
    LDataSource(wl_resource *resource, LClient *client);
    wl_resource *resource() const;
    LClient *client() const;
    const std::list<const char*>&mimeTypes() const;

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

private:
    friend class Globals::DataSource;
    LClient *p_client = nullptr;
    wl_resource *p_resource = nullptr;
    std::list<const char*>p_mimeTypes;

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3
    UInt32 p_dndActions = 0;
#endif
};

#endif // LDATASOURCE_H
