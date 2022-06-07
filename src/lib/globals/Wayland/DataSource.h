#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <LNamespaces.h>

class Louvre::Globals::DataSource
{
public:
    static void resource_destroy(wl_resource *resource);
    static void destroy(wl_client *client, wl_resource *resource);
    static void offer(wl_client *client, wl_resource *resource, const char *mime_type);
    static void set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions);
};


#endif // DATASOURCE_H
