#ifndef OUTPUT_H
#define OUTPUT_H

#include <LNamespaces.h>

class Louvre::Globals::Output
{
public:
    static void release(wl_client *client,wl_resource *resource);
    static void resource_destroy(wl_resource *resource);
    static void bind(wl_client *client, void *data, UInt32 version, UInt32 id);
};
#endif // OUTPUT_H
