#include "DataDevice.h"
#include <DataOffer.h>
#include <LDataOffer.h>
#include <stdio.h>

using namespace Louvre::Globals;

struct wl_data_offer_interface dataOffer_implementation =
{
   .accept = &DataOffer::accept,
   .receive = &DataOffer::receive,
   .destroy = &DataOffer::destroy,
   .finish = &DataOffer::finish,
   .set_actions = &DataOffer::set_actions
};

void DataDevice::resource_destroy(wl_resource *resource)
{
    printf("DATA DEVICE DESTROYED.\n");
}

void DataDevice::release(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void DataDevice::start_drag(wl_client *client, wl_resource *resource, wl_resource *source, wl_resource *origin, wl_resource *icon, UInt32 serial)
{

}

void DataDevice::set_selection(wl_client *client, wl_resource *resource, wl_resource *source, UInt32 serial)
{
    printf("SELECTION SET.\n");
    Int32 version = wl_resource_get_version(resource);
    wl_resource *dataOffer = wl_resource_create(client,&wl_data_offer_interface,version,0);
    LDataOffer *lDataOffer = new LDataOffer(dataOffer);
    wl_resource_set_implementation(dataOffer, &dataOffer_implementation, lDataOffer, &DataOffer::resource_destroy);
    wl_data_device_send_data_offer(resource,dataOffer);
}


