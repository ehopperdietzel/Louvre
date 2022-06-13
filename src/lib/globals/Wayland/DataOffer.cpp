#include "DataOffer.h"
#include <LDataOffer.h>

using namespace Louvre::Globals;

void DataOffer::resource_destroy(wl_resource *resource)
{
    LDataOffer *lDataOffer = (LDataOffer*)wl_resource_get_user_data(resource);
    delete lDataOffer;
}

void DataOffer::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void DataOffer::accept(wl_client *client, wl_resource *resource, UInt32 serial, const char *mime_type)
{

}

void DataOffer::finish(wl_client *client, wl_resource *resource)
{

}

void DataOffer::receive(wl_client *client, wl_resource *resource, const char *mime_type, Int32 fd)
{

}

void DataOffer::set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions, UInt32 preferred_action)
{

}
