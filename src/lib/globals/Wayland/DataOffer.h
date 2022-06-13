#ifndef DATAOFFER_H
#define DATAOFFER_H

#include <LNamespaces.h>

class Louvre::Globals::DataOffer
{
public:
    static void resource_destroy(wl_resource *resource);
    static void destroy(wl_client *client, wl_resource *resource);
    static void accept(wl_client *client, wl_resource *resource, UInt32 serial, const char *mime_type);
    static void finish(wl_client *client, wl_resource *resource);
    static void receive(wl_client *client, wl_resource *resource, const char *mime_type, Int32 fd);
    static void set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions, UInt32 preferred_action);
};

#endif // DATAOFFER_H
