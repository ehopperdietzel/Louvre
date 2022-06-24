#include "DataDevice.h"
#include <DataOffer.h>
#include <LDataOffer.h>
#include <LDataDevice.h>
#include <LClientPrivate.h>
#include <LCompositorPrivate.h>
#include <LDataSourcePrivate.h>
#include <stdio.h>

using namespace Louvre::Globals;

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
    LDataDevice *lDataDevice = (LDataDevice*)wl_resource_get_user_data(resource);

    if(source != NULL)
    {
        LDataSource *lDataSource = (LDataSource*)wl_resource_get_user_data(source);
        lDataDevice->client()->compositor()->imp()->m_dataSelection = lDataSource;
    }
    else
    {
       printf("Data unselected.\n");
       lDataDevice->client()->compositor()->imp()->m_dataSelection = nullptr;
    }

}


