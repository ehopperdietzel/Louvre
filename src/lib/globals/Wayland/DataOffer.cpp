#include "DataOffer.h"
#include <LDataOfferPrivate.h>
#include <stdio.h>
#include <LDataSourcePrivate.h>
#include <LDataDevicePrivate.h>
#include <LClient.h>
#include <unistd.h>

using namespace Louvre::Globals;

void DataOffer::resource_destroy(wl_resource *resource)
{
    LDataOffer *lDataOffer = (LDataOffer*)wl_resource_get_user_data(resource);

    if(lDataOffer->dataSource())
        lDataOffer->dataSource()->imp()->m_dataOffers.remove(lDataOffer);

    delete lDataOffer;

    printf("DATA OFFER DESTROYED.\n");
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
    LDataOffer *lDataOffer = (LDataOffer*)wl_resource_get_user_data(resource);
    if(lDataOffer->dataSource())
    {
        printf("GDK WANTS %s\n",mime_type);
        wl_data_source_send_send(lDataOffer->dataSource()->resource(),mime_type,fd);
        close(fd);
    }
}

void DataOffer::set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions, UInt32 preferred_action)
{

}
