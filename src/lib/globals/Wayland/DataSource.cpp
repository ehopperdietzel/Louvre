#include "DataSource.h"
#include <LDataSource.h>
#include <LClient.h>
#include <LCompositor.h>

using namespace Louvre::Globals;

void DataSource::resource_destroy(wl_resource *resource)
{
    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);

    if(lDataSource->client()->compositor()->dataSource() == lDataSource)
        lDataSource->client()->compositor()->p_dataSource = nullptr;

    delete lDataSource;
}

void DataSource::destroy(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}

void DataSource::offer(wl_client *client, wl_resource *resource, const char *mime_type)
{
    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);
    lDataSource->p_mimeTypes.push_back(mime_type);
}

#if LOUVRE_DATA_DEVICE_MANAGER_VERSION >= 3
void DataSource::set_actions(wl_client *client, wl_resource *resource, UInt32 dnd_actions)
{
    LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(resource);
    lDataSource->p_dndActions = dnd_actions;
}
#endif
