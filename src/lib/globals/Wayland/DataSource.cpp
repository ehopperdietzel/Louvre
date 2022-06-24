#include "DataSource.h"
#include <LDataSourcePrivate.h>
#include <LClient.h>
#include <string.h>
#include <LCompositorPrivate.h>
#include <LDataOfferPrivate.h>
#include <LDataDevicePrivate.h>

using namespace Louvre::Globals;

void DataSource::resource_destroy(wl_resource *resource)
{
    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);

    for(LDataOffer *offer : lDataSource->dataOffers())
        offer->imp()->m_dataSource = nullptr;

    if(lDataSource->client()->compositor()->dataSelection() == lDataSource)
        lDataSource->client()->compositor()->imp()->m_dataSelection= nullptr;

    if(lDataSource->client()->compositor()->dataDrag() == lDataSource)
        lDataSource->client()->compositor()->imp()->m_dataDrag= nullptr;

    while(!lDataSource->imp()->m_mimeTypes.empty())
    {
        delete []lDataSource->imp()->m_mimeTypes.front();
        lDataSource->imp()->m_mimeTypes.pop_front();
    }

    delete lDataSource;

    printf("DATA SOURCE DESTROYED.\n");
}

void DataSource::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void DataSource::offer(wl_client *client, wl_resource *resource, const char *mime_type)
{
    printf("ASK MIME %s\n",mime_type);

    int len = strlen(mime_type);
    char *mime = new char[len+1];
    strcpy(mime,mime_type);

    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);
    lDataSource->imp()->m_mimeTypes.push_back(mime);
}

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3
void DataSource::set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions)
{
    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);
    lDataSource->imp()->m_dndActions = dnd_actions;
}
#endif
