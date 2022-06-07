#include "DataSource.h"


void Louvre::Globals::DataSource::resource_destroy(wl_resource *resource)
{

}

void Louvre::Globals::DataSource::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void Louvre::Globals::DataSource::offer(wl_client *client, wl_resource *resource, const char *mime_type)
{

}

void Louvre::Globals::DataSource::set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions)
{

}
